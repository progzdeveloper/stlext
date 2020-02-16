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

#include "platform.h"
#include "compiler.h"
#include "arch.h"

#ifdef STDX_PROCESSOR_X86_64
#ifdef STDX_OS_WIN
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#endif // STDX_PROCESSOR_X86_64

// Macro for counting bits in word
#define _BITS_PER_WORD(_Var) (sizeof(_Var) * CHAR_BIT) 


// Microsoft Visual C++ Compiler
#if defined(STDX_CMPLR_MSVC)
// Bit builtin's make these assumptions when calling _BitScanForward/Reverse
// etc. These assumptions are expected to be true for Win32/Win64 which this
// file supports.
static_assert(sizeof(unsigned long long) == 8, "");
static_assert(sizeof(unsigned long) == 4, "");
static_assert(sizeof(unsigned int) == 4, "");

__FORCE_INLINE unsigned int __builtin_popcount(unsigned int x) {
	return __popcnt(x);
}

__FORCE_INLINE unsigned int __builtin_popcountl(unsigned long x)
{
	return __popcnt(static_cast<unsigned int>(x));
}

__FORCE_INLINE unsigned int __builtin_popcountll(unsigned long long x)
{
#if defined(_WIN64)
	return static_cast<unsigned int>(__popcnt64(x));
#elif defined(_WIN32)
	return static_cast<unsigned int>(__popcnt(static_cast<unsigned int>(x)) + __popcnt(static_cast<unsigned int>(x >> 32)));
#endif
}

// Returns the number of trailing 0-bits in x, starting at the least significant
// bit position. If x is 0, the result is undefined.
__FORCE_INLINE int __builtin_ctzll(unsigned long long mask)
{
	unsigned long where;
	// Search from LSB to MSB for first set bit.
	// Returns zero if no set bit is found.
#if defined(_WIN64)
	if (_BitScanForward64(&where, mask))
		return static_cast<int>(where);
#elif defined(_WIN32)
	// Win32 doesn't have _BitScanForward64 so emulate it with two 32 bit calls.
	// Scan the Low Word.
	if (_BitScanForward(&where, static_cast<unsigned long>(mask)))
		return static_cast<int>(where);
	// Scan the High Word.
	if (_BitScanForward(&where, static_cast<unsigned long>(mask >> 32)))
		return static_cast<int>(where + 32); // Create a bit offset from the LSB.
#else
#error "Implementation of __builtin_ctzll required"
#endif
	return 64;
}

__FORCE_INLINE int __builtin_ctzl(unsigned long mask)
{
	unsigned long where;
	// Search from LSB to MSB for first set bit.
	// Returns zero if no set bit is found.
	if (_BitScanForward(&where, mask))
		return static_cast<int>(where);
	return 32;
}

__FORCE_INLINE int __builtin_ctz(unsigned int mask)
{
	// Win32 and Win64 expectations.
	static_assert(sizeof(mask) == 4, "");
	static_assert(sizeof(unsigned long) == 4, "");
	return __builtin_ctzl(static_cast<unsigned long>(mask));
}

// Returns the number of leading 0-bits in x, starting at the most significant
// bit position. If x is 0, the result is undefined.
__FORCE_INLINE int __builtin_clzll(unsigned long long mask)
{
	unsigned long where;
	// BitScanReverse scans from MSB to LSB for first set bit.
	// Returns 0 if no set bit is found.
#if defined(_WIN64)
	if (_BitScanReverse64(&where, mask))
		return static_cast<int>(63 - where);
#elif defined(_WIN32)
	// Scan the high 32 bits.
	if (_BitScanReverse(&where, static_cast<unsigned long>(mask >> 32)))
		return static_cast<int>(63 -
		(where + 32)); // Create a bit offset from the MSB.
	// Scan the low 32 bits.
	if (_BitScanReverse(&where, static_cast<unsigned long>(mask)))
		return static_cast<int>(63 - where);
#else
#error "Implementation of __builtin_clzll required"
#endif
	return 64; // Undefined Behavior.
}

__FORCE_INLINE int __builtin_clzl(unsigned long mask)
{
	unsigned long where;
	// Search from LSB to MSB for first set bit.
	// Returns zero if no set bit is found.
	if (_BitScanReverse(&where, mask))
		return static_cast<int>(31 - where);
	return 32; // Undefined Behavior.
}

__FORCE_INLINE int __builtin_clz(unsigned int x)
{
	return __builtin_clzl(x);
}

// IBM Compiler
#elif defined(STDX_CMPLR_XLC)
extern "builtin" int __popcnt4(unsigned int);
extern "builtin" int __popcnt8(unsigned long long);
extern "builtin" unsigned int __cnttz4(unsigned int);
extern "builtin" unsigned int __cnttz8(unsigned long long);
extern "builtin" unsigned int __cntlz4(unsigned int);
extern "builtin" unsigned int __cntlz8(unsigned long long);

// Builtin functions for counting population
#define __builtin_popcount(x) __popcnt4(x)
#define __builtin_popcountll(x) __popcnt8(x)
#if defined(__64BIT__)
#define __builtin_popcountl(x) __builtin_popcountll(x)
#else
#define __builtin_popcountl(x) __builtin_popcount(x)
#endif

// Builtin functions for counting trailing zeros
#define __builtin_ctz(x) __cnttz4(x)
#define __builtin_ctzll(x) __cnttz8(x)
#if defined(__64BIT__)
#define __builtin_ctzl(x) __builtin_ctzll(x)
#else
#define __builtin_ctzl(x) __builtin_ctz(x)
#endif

// Builtin functions for counting leading zeros
#define __builtin_clz(x) __cntlz4(x)
#define __builtin_clzll(x) __cntlz8(x)
#if defined(__64BIT__)
#define __builtin_clzl(x) __builtin_clzll(x)
#else
#define __builtin_clzl(x) __builtin_clz(x)
#endif 

// GNU compatible compiles
#elif defined(STDX_CMPLR_GNU) || defined(STDX_CMPLR_INTEL) || defined(STDX_CMPLR_CLANG)

// does nothing because it is already 
// has compatible intrinsics

#else // Generic implementation
__FORCE_INLINE int __builtin_popcount(unsigned char x) 
{
	static const char *const __bitsperbyte =
		"\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
		"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
		"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
		"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
		"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
		"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
		"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
		"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
		"\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
		"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
		"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
		"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
		"\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
		"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
		"\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
		"\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";

	size_t val = 0;
	for (; first != last; ++first)
		val += __bitsperbyte[*first];
	return (val);
}

__FORCE_INLINE int __builtin_popcount(unsigned int x)
{
	// Binary: 0101...
	static const unsigned int m1 = 0x55555555;
	// Binary: 00110011..
	static const unsigned int m2 = 0x33333333;
	// Binary:  4 zeros,  4 ones ...
	static const unsigned int m4 = 0x0f0f0f0f;
	// The sum of 256 to the power of 0,1,2,3...
	static const unsigned int h01 = 0x01010101;
	// Put count of each 2 bits into those 2 bits.
	x -= (x >> 1) & m1;
	// Put count of each 4 bits into those 4 bits.
	x = (x & m2) + ((x >> 2) & m2);
	// Put count of each 8 bits into those 8 bits.
	x = (x + (x >> 4)) & m4;
	// Returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24).
	return (x * h01) >> 24;
}

__FORCE_INLINE int __builtin_popcountl(unsigned long x)
{
	return __builtin_popcount(static_cast<int>(x));
}

__FORCE_INLINE int __builtin_popcountll(unsigned long long x)
{
	// Binary: 0101...
	static const unsigned long long m1 = 0x5555555555555555;
	// Binary: 00110011..
	static const unsigned long long m2 = 0x3333333333333333;
	// Binary:  4 zeros,  4 ones ...
	static const unsigned long long m4 = 0x0f0f0f0f0f0f0f0f;
	// The sum of 256 to the power of 0,1,2,3...
	static const unsigned long long h01 = 0x0101010101010101;
	// Put count of each 2 bits into those 2 bits.
	x -= (x >> 1) & m1;
	// Put count of each 4 bits into those 4 bits.
	x = (x & m2) + ((x >> 2) & m2);
	// Put count of each 8 bits into those 8 bits.
	x = (x + (x >> 4)) & m4;
	// Returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
	return static_cast<int>((x * h01) >> 56);
}

#endif

namespace stdx {

// Precondition:  __x != 0
__FORCE_INLINE unsigned           __ctz(unsigned           __x) { return static_cast<unsigned>(__builtin_ctz(__x)); }
__FORCE_INLINE unsigned long      __ctz(unsigned      long __x) { return static_cast<unsigned long>(__builtin_ctzl(__x)); }
__FORCE_INLINE unsigned long long __ctz(unsigned long long __x) { return static_cast<unsigned long long>(__builtin_ctzll(__x)); }

// Precondition:  __x != 0
__FORCE_INLINE unsigned           __clz(unsigned           __x) { return static_cast<unsigned>(__builtin_clz(__x)); }
__FORCE_INLINE unsigned long      __clz(unsigned      long __x) { return static_cast<unsigned long>(__builtin_clzl(__x)); }
__FORCE_INLINE unsigned long long __clz(unsigned long long __x) { return static_cast<unsigned long long>(__builtin_clzll(__x)); }

__FORCE_INLINE unsigned __pop_count(unsigned           __x) { return __builtin_popcount(__x);   }
__FORCE_INLINE unsigned __pop_count(unsigned      long __x) { return __builtin_popcountl(__x);  }
__FORCE_INLINE unsigned __pop_count(unsigned long long __x) { return __builtin_popcountll(__x); }


// Swap bytes of a 1 byte
__FORCE_INLINE uint8_t  __byte_swap(uint8_t   __x) noexcept { return (__x); }
// Swap bytes of a 2 byte word
__FORCE_INLINE uint16_t __byte_swap(uint16_t  __x) noexcept { return static_cast<unsigned>(__builtin_bswap16(__x)); }
// Swap bytes of a 4 byte word
__FORCE_INLINE uint32_t __byte_swap(uint32_t  __x) noexcept { return static_cast<unsigned>(__builtin_bswap32(__x)); }
// Swap bytes of a 8 byte word
__FORCE_INLINE uint64_t __byte_swap(uint64_t  __x) noexcept { return static_cast<unsigned>(__builtin_bswap64(__x)); }



// Reverse bits in a 8-bit word
__FORCE_INLINE uint8_t __bit_swap(uint8_t __x) noexcept {
#ifdef STDX_PROCESSOR_X86_64
    return (((__x * 0x80200802ULL) & 0x0884422110ULL) * 0x0101010101ULL >> 32);
#else
    return (((__x * 0x0802LU & 0x22110LU) | (__x * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
#endif
}

// Reverse bits in a 32-bit word
__FORCE_INLINE uint32_t __bit_swap(uint32_t __x) noexcept {
    __x = (__x >> 16) | (__x << 16);
    __x = ((__x & 0xFF00FF00) >> 8) | ((__x & 0x00FF00FF) << 8);
    __x = ((__x & 0xF0F0F0F0) >> 4) | ((__x & 0x0F0F0F0F) << 4);
    __x = ((__x & 0xCCCCCCCC) >> 2) | ((__x & 0x33333333) << 2);
    return (((__x & 0xAAAAAAAA) >> 1) | ((__x & 0x55555555) << 1));
}

// Reverse bits in a 64-bit word
__FORCE_INLINE uint64_t __bit_swap(uint64_t __x) noexcept {

    __x = ((__x & 0xFFFFFFFF00000000ULL) >> 32) | ((__x & 0x00000000FFFFFFFFULL) << 32);
    __x = ((__x & 0xFFFF0000FFFF0000ULL) >> 16) | ((__x & 0x0000FFFF0000FFFFULL) << 16);
    __x = ((__x & 0xFF00FF00FF00FF00ULL) >> 8)  | ((__x & 0x00FF00FF00FF00FFULL) << 8);
    __x = ((__x & 0xF0F0F0F0F0F0F0F0ULL) >> 4)  | ((__x & 0x0F0F0F0F0F0F0F0FULL) << 4);
    __x = ((__x & 0xCCCCCCCCCCCCCCCCULL) >> 2)  | ((__x & 0x3333333333333333ULL) << 2);
    return (((__x & 0xAAAAAAAAAAAAAAAAULL) >> 1) | ((__x & 0x5555555555555555ULL) << 1));
}


// Replaces len bits of src0 by the ones of src1 where the mask is true
template <class T>
constexpr T __bit_blend(T src0, T src1, T msk) noexcept
{
    return src0 ^ ((src0 ^ src1) & msk);
}

// Replaces len bits of src0 by the ones of src1 starting at start
template <class T>
constexpr T __bit_blend(T src0, T src1, T start, T len) noexcept
{
    constexpr T bpw = sizeof(T) * CHAR_BIT;
    constexpr T one = 1;
    const T msk = ((one << len) * (len < bpw) - one) << start;
    return src0 ^ ((src0 ^ src1) & msk * (start < bpw));
}



// Left shifts dst by cnt bits, filling the lsbs of dst by the msbs of src
template <class T>
constexpr T __shld(T dst, T src, T cnt) noexcept
{
    constexpr T bpw = sizeof(T) * CHAR_BIT;
    if (cnt < bpw) {
        dst = (dst << cnt) | (src >> (bpw - cnt));
    } else {
        dst = (src << (cnt - bpw)) * (cnt < bpw + bpw);
    }
    return dst;
}


// Right shifts dst by cnt bits, filling the msbs of dst by the lsbs of src
template <class T>
constexpr T __shrd(T dst, T src, T cnt) noexcept
{
    constexpr T bpw = sizeof(T) * CHAR_BIT;
    if (cnt < bpw) {
        dst = (dst >> cnt) | (src << (bpw - cnt));
    } else {
        dst = (src >> (cnt - bpw)) * (cnt < bpw + bpw);
    }
    return dst;
}

}


