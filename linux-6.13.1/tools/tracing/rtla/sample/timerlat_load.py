#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2024 Red Hat, Inc. Daniel Bristot de Oliveira <bristot@kernel.org>
#
# This is a sample code about how to use timerlat's timer by any workload
# so rtla can measure and provide auto-analysis for the overall latency (IOW
# the response time) for a task.
#
# Before running it, you need to dispatch timerlat with -U option in a terminal.
# Then # run this script pinned to a CPU on another terminal. For example:
#
# timerlat_load.py 1 -p 95
#
# The "Timerlat IRQ" is the IRQ latency, The thread latency is the latency
# for the python process to get the CPU. The Ret from user Timer Latency is
# the overall latency. In other words, it is the response time for that
# activation.
#
# This is just an example, the load is reading 20MB of data from /dev/full
# It is in python because it is easy to read :-)

import argparse
import sys
import os

parser = argparse.ArgumentParser(description='user-space timerlat thread in Python')
parser.add_argument("cpu", type=int, help='CPU to run timerlat thread')
parser.add_argument("-p", "--prio", type=int, help='FIFO priority')
args = parser.parse_args()

try:
    affinity_mask = {args.cpu}
    os.sched_setaffinity(0, affinity_mask)
except Exception as e:
    print(f"Error setting affinity: {e}")
    sys.exit(1)

if args.prio:
    try:
        param = os.sched_param(args.prio)
        os.sched_setscheduler(0, os.SCHED_FIFO, param)
    except Exception as e:
        print(f"Error setting priority: {e}")
        sys.exit(1)

try:
    timerlat_path = f"/sys/kernel/tracing/osnoise/per_cpu/cpu{args.cpu}/timerlat_fd"
    timerlat_fd = open(timerlat_path, 'r')
except PermissionError:
    print("Permission denied. Please check your access rights.")
    sys.exit(1)
except OSError:
    print("Error opening timerlat fd, did you run timerlat -U?")
    sys.exit(1)

try:
    data_fd = open("/dev/full", 'r')
except Exception as e:
    print(f"Error opening data fd: {e}")
    sys.exit(1)

while True:
    try:
        timerlat_fd.read(1)
        data_fd.read(20 * 1024 * 1024)
    except KeyboardInterrupt:
        print("Leaving")
        break
    except IOError as e:
        print(f"I/O error occurred: {e}")
        break
    except Exception as e:
        print(f"Unexpected error: {e}")
        break

timerlat_fd.close()
data_fd.close()
