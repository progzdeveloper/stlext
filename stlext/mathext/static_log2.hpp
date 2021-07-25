// Copyright (c) 2016, Michael Polukarov (Russia).
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer listed
//   in this license in the documentation and/or other materials
//   provided with the distribution.
//
// - Neither the name of the copyright holders nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include <cstdint>
#include "../platform/common.h"

_STDX_BEGIN

namespace detail {

template <uintmax_t n>
struct choose_initial_n {
    static const bool c = ((uintmax_t(1) << n << n) != 0);
    static const uintmax_t value = (!c*n + choose_initial_n<2 * c*n>::value);
};

template <>
struct choose_initial_n<0> {
    static const uintmax_t value = 0;
};

// start computing from n_zero - must be a power of two
static const uintmax_t n_zero = 16;
static const uintmax_t initial_n = choose_initial_n<n_zero>::value;

template <uintmax_t x, uintmax_t n = initial_n>
struct static_log2_impl {
    static const bool c = ((x >> n) > 0); // x >= 2**n ?
    static const uintmax_t value = c*n + (static_log2_impl< (x >> c*n), n / 2 >::value);
};

template <>
struct static_log2_impl<1, 0> {
    static const uintmax_t value = 0;
};

} // end namespace detail

template <uintmax_t X>
struct static_log2
{
private:
    // choose_initial_n<>
    //
    // Recursively doubles its integer argument, until it
    // becomes >= of the "width" (C99, 6.2.6.2p4) of
    // static_log2_argument_type.
    //
    // Used to get the maximum power of two less then the width.
    //
    // Example: if on your platform argument_type has 48 value
    //          bits it yields n=32.
    //
    // It's easy to prove that, starting from such a value
    // of n, the core algorithm works correctly for any width
    // of static_log2_argument_type and that recursion always
    // terminates with x = 1 and n = 0 (see the algorithm's
    // invariant).

    typedef uintmax_t argument_type;
    typedef uintmax_t result_type;


    // static_log2_impl<>
    //
    // * Invariant:
    //                 2n
    //  1 <= x && x < 2    at the start of each recursion
    //                     (see also choose_initial_n<>)
    //
    // * Type requirements:
    //
    //   argument_type maybe any unsigned type with at least n_zero + 1
    //   value bits. (Note: If larger types will be standardized -e.g.
    //   unsigned long long- then the argument_type typedef can be
    //   changed without affecting the rest of the code.)
    //

public:
    static const uintmax_t value = detail::static_log2_impl<X>::value;
};


template <>
struct static_log2<0> { };



_STDX_END




