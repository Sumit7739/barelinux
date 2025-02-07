// SPDX-License-Identifier: GPL-2.0
#include <test_progs.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "test_send_signal_kern.skel.h"
#include "io_helpers.h"

static int sigusr1_received;

static void sigusr1_handler(int signum)
{
	sigusr1_received = 8;
}

static void sigusr1_siginfo_handler(int s, siginfo_t *i, void *v)
{
	sigusr1_received = (int)(long long)i->si_value.sival_ptr;
}

static void test_send_signal_common(struct perf_event_attr *attr,
				    bool signal_thread, bool remote)
{
	struct test_send_signal_kern *skel;
	struct sigaction sa;
	int pipe_c2p[2], pipe_p2c[2];
	int err = -1, pmu_fd = -1;
	volatile int j = 0;
	int retry_count;
	char buf[256];
	pid_t pid;
	int old_prio;

	if (!ASSERT_OK(pipe(pipe_c2p), "pipe_c2p"))
		return;

	if (!ASSERT_OK(pipe(pipe_p2c), "pipe_p2c")) {
		close(pipe_c2p[0]);
		close(pipe_c2p[1]);
		return;
	}

	pid = fork();
	if (!ASSERT_GE(pid, 0, "fork")) {
		close(pipe_c2p[0]);
		close(pipe_c2p[1]);
		close(pipe_p2c[0]);
		close(pipe_p2c[1]);
		return;
	}

	if (pid == 0) {
		/* install signal handler and notify parent */
		if (remote) {
			sa.sa_sigaction = sigusr1_siginfo_handler;
			sa.sa_flags = SA_RESTART | SA_SIGINFO;
			ASSERT_NEQ(sigaction(SIGUSR1, &sa, NULL), -1, "sigaction");
		} else {
			ASSERT_NEQ(signal(SIGUSR1, sigusr1_handler), SIG_ERR, "signal");
		}

		close(pipe_c2p[0]); /* close read */
		close(pipe_p2c[1]); /* close write */

		/* boost with a high priority so we got a higher chance
		 * that if an interrupt happens, the underlying task
		 * is this process.
		 */
		if (!remote) {
			errno = 0;
			old_prio = getpriority(PRIO_PROCESS, 0);
			ASSERT_OK(errno, "getpriority");
			ASSERT_OK(setpriority(PRIO_PROCESS, 0, -20), "setpriority");
		}

		/* notify parent signal handler is installed */
		ASSERT_EQ(write(pipe_c2p[1], buf, 1), 1, "pipe_write");

		/* make sure parent enabled bpf program to send_signal */
		ASSERT_EQ(read(pipe_p2c[0], buf, 1), 1, "pipe_read");

		/* wait a little for signal handler */
		for (int i = 0; i < 1000000000 && !sigusr1_received; i++) {
			j /= i + j + 1;
			if (remote)
				sleep(1);
			else
				if (!attr)
					/* trigger the nanosleep tracepoint program. */
					usleep(1);
		}

		buf[0] = sigusr1_received;

		ASSERT_EQ(sigusr1_received, 8, "sigusr1_received");
		ASSERT_EQ(write(pipe_c2p[1], buf, 1), 1, "pipe_write");

		/* wait for parent notification and exit */
		ASSERT_EQ(read(pipe_p2c[0], buf, 1), 1, "pipe_read");

		/* restore the old priority */
		if (!remote)
			ASSERT_OK(setpriority(PRIO_PROCESS, 0, old_prio), "setpriority");

		close(pipe_c2p[1]);
		close(pipe_p2c[0]);
		exit(0);
	}

	close(pipe_c2p[1]); /* close write */
	close(pipe_p2c[0]); /* close read */

	skel = test_send_signal_kern__open_and_load();
	if (!ASSERT_OK_PTR(skel, "skel_open_and_load"))
		goto skel_open_load_failure;

	/* boost with a high priority so we got a higher chance
	 * that if an interrupt happens, the underlying task
	 * is this process.
	 */
	if (remote) {
		errno = 0;
		old_prio = getpriority(PRIO_PROCESS, 0);
		ASSERT_OK(errno, "getpriority");
		ASSERT_OK(setpriority(PRIO_PROCESS, 0, -20), "setpriority");
	}

	if (!attr) {
		err = test_send_signal_kern__attach(skel);
		if (!ASSERT_OK(err, "skel_attach")) {
			err = -1;
			goto destroy_skel;
		}
	} else {
		if (!remote)
			pmu_fd = syscall(__NR_perf_event_open, attr, pid, -1 /* cpu */,
					 -1 /* group id */, 0 /* flags */);
		else
			pmu_fd = syscall(__NR_perf_event_open, attr, getpid(), -1 /* cpu */,
					 -1 /* group id */, 0 /* flags */);
		if (!ASSERT_GE(pmu_fd, 0, "perf_event_open")) {
			err = -1;
			goto destroy_skel;
		}

		skel->links.send_signal_perf =
			bpf_program__attach_perf_event(skel->progs.send_signal_perf, pmu_fd);
		if (!ASSERT_OK_PTR(skel->links.send_signal_perf, "attach_perf_event"))
			goto disable_pmu;
	}

	/* wait until child signal handler installed */
	ASSERT_EQ(read(pipe_c2p[0], buf, 1), 1, "pipe_read");

	/* trigger the bpf send_signal */
	skel->bss->signal_thread = signal_thread;
	skel->bss->sig = SIGUSR1;
	if (!remote) {
		skel->bss->target_pid = 0;
		skel->bss->pid = pid;
	} else {
		skel->bss->target_pid = pid;
		skel->bss->pid = getpid();
	}

	/* notify child that bpf program can send_signal now */
	ASSERT_EQ(write(pipe_p2c[1], buf, 1), 1, "pipe_write");

	for (retry_count = 0;;) {
		/* For the remote test, the BPF program is triggered from this
		 * process but the other process/thread is signaled.
		 */
		if (remote) {
			if (!attr) {
				for (int i = 0; i < 10; i++)
					usleep(1);
			} else {
				for (int i = 0; i < 100000000; i++)
					j /= i + 1;
			}
		}
		/* wait for result */
		err = read_with_timeout(pipe_c2p[0], buf, 1, 100);
		if (err == -EAGAIN && retry_count++ < 10000)
			continue;
		break;
	}
	if (!ASSERT_GE(err, 0, "reading pipe"))
		goto disable_pmu;
	if (!ASSERT_GT(err, 0, "reading pipe error: size 0")) {
		err = -1;
		goto disable_pmu;
	}

	ASSERT_EQ(buf[0], 8, "incorrect result");

	/* notify child safe to exit */
	ASSERT_EQ(write(pipe_p2c[1], buf, 1), 1, "pipe_write");

disable_pmu:
	close(pmu_fd);
destroy_skel:
	test_send_signal_kern__destroy(skel);
	/* restore the old priority */
	if (remote)
		ASSERT_OK(setpriority(PRIO_PROCESS, 0, old_prio), "setpriority");
skel_open_load_failure:
	close(pipe_c2p[0]);
	close(pipe_p2c[1]);
	wait(NULL);
}

static void test_send_signal_tracepoint(bool signal_thread, bool remote)
{
	test_send_signal_common(NULL, signal_thread, remote);
}

static void test_send_signal_perf(bool signal_thread, bool remote)
{
	struct perf_event_attr attr = {
		.freq = 1,
		.sample_freq = 1000,
		.type = PERF_TYPE_SOFTWARE,
		.config = PERF_COUNT_SW_CPU_CLOCK,
	};

	test_send_signal_common(&attr, signal_thread, remote);
}

static void test_send_signal_nmi(bool signal_thread, bool remote)
{
	struct perf_event_attr attr = {
		.freq = 1,
		.sample_freq = 1000,
		.type = PERF_TYPE_HARDWARE,
		.config = PERF_COUNT_HW_CPU_CYCLES,
	};
	int pmu_fd;

	/* Some setups (e.g. virtual machines) might run with hardware
	 * perf events disabled. If this is the case, skip this test.
	 */
	pmu_fd = syscall(__NR_perf_event_open, &attr, 0 /* pid */,
			 -1 /* cpu */, -1 /* group_fd */, 0 /* flags */);
	if (pmu_fd == -1) {
		if (errno == ENOENT || errno == EOPNOTSUPP) {
			printf("%s:SKIP:no PERF_COUNT_HW_CPU_CYCLES\n",
			       __func__);
			test__skip();
			return;
		}
		/* Let the test fail with a more informative message */
	} else {
		close(pmu_fd);
	}

	test_send_signal_common(&attr, signal_thread, remote);
}

void test_send_signal(void)
{
	if (test__start_subtest("send_signal_tracepoint"))
		test_send_signal_tracepoint(false, false);
	if (test__start_subtest("send_signal_perf"))
		test_send_signal_perf(false, false);
	if (test__start_subtest("send_signal_nmi"))
		test_send_signal_nmi(false, false);
	if (test__start_subtest("send_signal_tracepoint_thread"))
		test_send_signal_tracepoint(true, false);
	if (test__start_subtest("send_signal_perf_thread"))
		test_send_signal_perf(true, false);
	if (test__start_subtest("send_signal_nmi_thread"))
		test_send_signal_nmi(true, false);

	/* Signal remote thread and thread group */
	if (test__start_subtest("send_signal_tracepoint_remote"))
		test_send_signal_tracepoint(false, true);
	if (test__start_subtest("send_signal_perf_remote"))
		test_send_signal_perf(false, true);
	if (test__start_subtest("send_signal_nmi_remote"))
		test_send_signal_nmi(false, true);
	if (test__start_subtest("send_signal_tracepoint_thread_remote"))
		test_send_signal_tracepoint(true, true);
	if (test__start_subtest("send_signal_perf_thread_remote"))
		test_send_signal_perf(true, true);
	if (test__start_subtest("send_signal_nmi_thread_remote"))
		test_send_signal_nmi(true, true);
}
