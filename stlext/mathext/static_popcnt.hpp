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
#include <limits>
#include <cstdint>
#include "../platform/common.h"

_STDX_BEGIN

template<uintmax_t x>
struct static_popcnt
{
private:
    template<uintmax_t x, uintmax_t n>
	struct static_popcnt_impl
	{
        static const uintmax_t mask = (((uintmax_t)1) << n);
        static const uintmax_t value = ((x & mask) != 0) + static_popcnt_impl<x, n - 1>::value;
	};


	template<size_t x>
	struct static_popcnt_impl < x, 0 >
	{
        static const uintmax_t value = ((x & 1) != 0);
	};
    static const uintmax_t width = std::numeric_limits<uintmax_t>::digits - 1;

public:
    static const uintmax_t value = static_popcnt_impl<x, width>::value;
};


template<>
struct static_popcnt<0>
{
    static const uintmax_t value = 0;
};


template<>
struct static_popcnt<1>
{
    static const uintmax_t value = 1;
};

_STDX_END

