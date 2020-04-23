// Copyright (c) 2018-2020, Michael Polukarov (Russia).
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

#ifndef BFC_UTILITIES_H
#define BFC_UTILITIES_H

#include <iso646.h> // mostly for Microsoft compilers
#include <stdint.h> // part of Visual Studio 2010 and better
#include <stddef.h> // for size_t in C
#include <limits.h> // for size_t in C

/*!
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline uint32_t __fastrange_u32(uint32_t word, uint32_t p) {
    return (uint32_t)(((uint64_t)word * (uint64_t)p) >> 32);
}

#if defined(_MSC_VER) && defined (_WIN64)
#include <intrin.h>// should be part of all recent Visual Studio
#pragma intrinsic(_umul128)
#endif // defined(_MSC_VER) && defined (_WIN64)


/*!
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline uint64_t __fastrange_u64(uint64_t word, uint64_t p) {
#ifdef __SIZEOF_INT128__ // then we know we have a 128-bit int
    return (uint64_t)(((__uint128_t)word * (__uint128_t)p) >> 64);
#elif defined(_MSC_VER) && defined(_WIN64)
    // supported in Visual Studio 2005 and better
    uint64_t highProduct;
    _umul128(word, p, &highProduct); // ignore output
    return highProduct;
    unsigned __int64 _umul128(
        unsigned __int64 Multiplier,
        unsigned __int64 Multiplicand,
        unsigned __int64 *HighProduct
    );
#else
    return word % p; // fallback
#endif // __SIZEOF_INT128__
}


#ifndef UINT32_MAX
#define UINT32_MAX  (0xffffffff)
#endif // UINT32_MAX

/*!
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline size_t __fastrange(size_t word, size_t p) {
#if (SIZE_MAX == UINT32_MAX)
    return (size_t)__fastrange_u32(word, p);
#else // assume 64-bit
    return (size_t)__fastrange_u64(word, p);
#endif // SIZE_MAX == UINT32_MAX
}

/*!
 * Given a value "word", produces an integer in [0,p) without division.
 * The function is as fair as possible in the sense that if you iterate
 * through all possible values of "word", then you will generate all
 * possible outputs as uniformly as possible.
 */
static inline int __ifastrange(int word, int p) {
#if (SIZE_MAX == UINT32_MAX)
    return (int)__fastrange_u32(word, p);
#else // assume 64-bit
    return (int)__fastrange_u64(word, p);
#endif // (SIZE_MAX == UINT32_MAX)
}


static inline uint32_t __murmur ( uint32_t h )
{
    h ^= h >> 16;
    h *= UINT32_C(0x85ebca6b);
    h ^= h >> 13;
    h *= UINT32_C(0xc2b2ae35);
    h ^= h >> 16;

    return h;
}

static inline uint64_t __murmur ( uint64_t k )
{
    k ^= k >> 33;
    k *= UINT64_C(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= UINT64_C(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;

    return k;
}

#endif // BFC_UTILITIES_H
