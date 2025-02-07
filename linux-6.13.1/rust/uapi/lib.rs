// SPDX-License-Identifier: GPL-2.0

//! UAPI Bindings.
//!
//! Contains the bindings generated by `bindgen` for UAPI interfaces.
//!
//! This crate may be used directly by drivers that need to interact with
//! userspace APIs.

#![no_std]
// See <https://github.com/rust-lang/rust-bindgen/issues/1651>.
#![cfg_attr(test, allow(deref_nullptr))]
#![cfg_attr(test, allow(unaligned_references))]
#![cfg_attr(test, allow(unsafe_op_in_unsafe_fn))]
#![allow(
    clippy::all,
    clippy::undocumented_unsafe_blocks,
    dead_code,
    missing_docs,
    non_camel_case_types,
    non_upper_case_globals,
    non_snake_case,
    improper_ctypes,
    unreachable_pub,
    unsafe_op_in_unsafe_fn
)]

// Manual definition of blocklisted types.
type __kernel_size_t = usize;
type __kernel_ssize_t = isize;
type __kernel_ptrdiff_t = isize;

include!(concat!(env!("OBJTREE"), "/rust/uapi/uapi_generated.rs"));
