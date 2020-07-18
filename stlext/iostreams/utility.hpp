// Copyright (c) 2013-2016, Michael Polukarov (Russia).
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
#include <clocale>
#include <locale>
#include <string>
#include <system_error>
#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "../platform/common.h"

_STDX_BEGIN

inline std::string error_message(const std::string& s,
                                 int val = errno,
                                 const std::error_category& c = std::generic_category()) {
    std::string text(s);
    text += c.message(val);
    return text;
}

inline std::string error_message(int val = errno,
                                 const std::error_category& c = std::generic_category()) {
    return c.message(val);
}

inline std::string error_message(const std::error_code& code) {
    return code.message();
}


static inline std::locale system_locale() {
    return std::locale(::setlocale(LC_ALL, ""));
}






template<class T>
inline unsigned count_digits(T x, size_t radix = 10) {
    std::size_t n = 1;
    while (x /= static_cast<T>(radix)) {
        ++n;
    }
    return n;
}

template<std::size_t _Radix, class T>
inline unsigned count_digits(T x) {
    std::size_t n = 1;
    while (x /= static_cast<T>(_Radix)) {
        ++n;
    }
    return n;
}



template<>
inline unsigned count_digits<10, uint32_t>(uint32_t n)
{
#if defined(_MSC_VER) || defined(__GNUC__)
    static const uint32_t powers_of_10[] = {
        0,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000
    };

#ifdef _MSC_VER
    unsigned long i = 0;
    _BitScanReverse(&i, n | 1);
    uint32_t t = (i + 1) * 1233 >> 12;
#elif __GNUC__
    uint32_t t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
#endif
    return t - (n < powers_of_10[t]) + 1;
#else
    // Simple pure C++ implementation
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
#endif
}


template<>
inline unsigned count_digits<10, int32_t>(int32_t n) {
    uint32_t u = static_cast<uint32_t>(n);
    if (n < 0) {
        u = ~u + 1;
    }
    return count_digits<10, uint32_t>(u);
}






template<>
inline unsigned count_digits<10, uint64_t>(uint64_t n)
{
#if defined(_MSC_VER) || defined(__GNUC__)
    static const uint64_t powers_of_10[] = {
        0,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000,
        100000000000,
        1000000000000,
        10000000000000,
        100000000000000,
        1000000000000000,
        10000000000000000,
        100000000000000000,
        1000000000000000000,
        10000000000000000000U
    };

#if __GNUC__
    uint32_t t = (64 - __builtin_clzll(n | 1)) * 1233 >> 12;
#elif _M_IX86
    unsigned long i = 0;
    uint64_t m = n | 1;
    if (_BitScanReverse(&i, m >> 32))
        i += 32;
    else
        _BitScanReverse(&i, m & 0xFFFFFFFF);
    uint32_t t = (i + 1) * 1233 >> 12;
#elif _M_X64
    unsigned long i = 0;
    _BitScanReverse64(&i, n | 1);
    uint32_t t = (i + 1) * 1233 >> 12;
#endif
    return t - (n < powers_of_10[t]) + 1;
#else
    // Simple pure C++ implementation
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    if (n < 10000000000) return 10;
    if (n < 100000000000) return 11;
    if (n < 1000000000000) return 12;
    if (n < 10000000000000) return 13;
    if (n < 100000000000000) return 14;
    if (n < 1000000000000000) return 15;
    if (n < 10000000000000000) return 16;
    if (n < 100000000000000000) return 17;
    if (n < 1000000000000000000) return 18;
    if (n < 10000000000000000000) return 19;
    return 20;
#endif
}

template<>
inline unsigned count_digits<10, int64_t>(int64_t n) {
    uint64_t u = static_cast<uint64_t>(n);
    if (n < 0) {
        u = ~u + 1;
    }
    return count_digits<10, uint64_t>(u);
}





_STDX_END
