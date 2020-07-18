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
#include <type_traits>
#include <algorithm>
#include <iterator>

#include <limits>
#include <climits>

#include "../platform/common.h"

#ifndef _NO_SSE_OPTIMIZATIONS
#if defined(i386) || defined(__amd64) || defined(_M_IX86) || defined(_M_X64)
#ifdef __SSE2__ // SSE2 optimized version

#include <cassert>
#include <cstdint>

#include <emmintrin.h>

#ifdef _MSC_VER
#include  <intrin.h>
#define ALIGN_PRE __declspec(align(16))
#define ALIGN_SUF
#else
#define ALIGN_PRE
#define ALIGN_SUF  __attribute__ ((aligned(16)))
#endif

#endif // _NO_SSE_OPTIMIZATIONS
#endif // defined(i386) || defined(__amd64) || defined(_M_IX86) || defined(_M_X64)
#endif // SSE2 optimized version

_STDX_BEGIN

namespace detail {


#ifndef _NO_SSE_OPTIMIZATIONS
#if defined(i386) || defined(__amd64) || defined(_M_IX86) || defined(_M_X64)
#ifdef __SSE2__ // SSE2 optimized version
static inline char* u32toa_sse2(uint32_t value, char* buffer);
static inline char* u64toa_sse2(uint64_t value, char* buffer);
#endif
#endif
#endif


template<
    class _Char,
    _Char... _Args
>
struct digit_alphabet {
    typedef _Char char_type;
    static constexpr size_t length = sizeof...(_Args);
    static const _Char digits[sizeof...(_Args)];
};

template<
    class _Char,
    _Char... _Args
>
const _Char digit_alphabet<_Char, _Args...>::digits[sizeof...(_Args)] = { _Args... };



template<unsigned _Base, class _Int, class _Char = char>
struct itos_base
{
    static constexpr const size_t max_size = 64;
    static const _Char digits[];
};

template<unsigned _Base, class _Int, class _Char>
const _Char itos_base<_Base, _Int, _Char>::digits[] = {
    _Char('0'), _Char('1'), _Char('2'), _Char('3'),
    _Char('4'), _Char('5'), _Char('6'), _Char('7'),
    _Char('8'), _Char('9'), _Char('a'), _Char('b'),
    _Char('c'), _Char('d'), _Char('e'), _Char('f'),
    _Char('g'), _Char('h'), _Char('i'), _Char('j'),
    _Char('k'), _Char('l'), _Char('m'), _Char('n'),
    _Char('o'), _Char('p'), _Char('q'), _Char('r'),
    _Char('s'), _Char('t'), _Char('u'), _Char('v'),
    _Char('w'), _Char('x'), _Char('y'), _Char('z')
};

template<class _Int, class _Char>
struct itos_base<0, _Int, _Char>
{
    static constexpr const size_t max_size = 64;
    static const _Char digits[];
};

template<class _Int, class _Char>
const _Char itos_base<0, _Int, _Char>::digits[] = {
    _Char('0'), _Char('1'), _Char('2'), _Char('3'),
    _Char('4'), _Char('5'), _Char('6'), _Char('7'),
    _Char('8'), _Char('9'), _Char('a'), _Char('b'),
    _Char('c'), _Char('d'), _Char('e'), _Char('f'),
    _Char('g'), _Char('h'), _Char('i'), _Char('j'),
    _Char('k'), _Char('l'), _Char('m'), _Char('n'),
    _Char('o'), _Char('p'), _Char('q'), _Char('r'),
    _Char('s'), _Char('t'), _Char('u'), _Char('v'),
    _Char('w'), _Char('x'), _Char('y'), _Char('z')
};


// TODO: assert thar CHAR_BIT == 8

template<class _Int>
struct itos_base<2, _Int, char>
{
    static constexpr const size_t max_size = sizeof(_Int)*CHAR_BIT;
    static constexpr const size_t length = (1ULL<<CHAR_BIT); // 256
    static const char* const digits[length];

};

template<class _Int>
const char* const itos_base<2, _Int, char>::digits[]= {
    "00000000","00000001","00000010","00000011","00000100","00000101","00000110","00000111",
    "00001000","00001001","00001010","00001011","00001100","00001101","00001110","00001111",
    "00010000","00010001","00010010","00010011","00010100","00010101","00010110","00010111",
    "00011000","00011001","00011010","00011011","00011100","00011101","00011110","00011111",
    "00100000","00100001","00100010","00100011","00100100","00100101","00100110","00100111",
    "00101000","00101001","00101010","00101011","00101100","00101101","00101110","00101111",
    "00110000","00110001","00110010","00110011","00110100","00110101","00110110","00110111",
    "00111000","00111001","00111010","00111011","00111100","00111101","00111110","00111111",
    "01000000","01000001","01000010","01000011","01000100","01000101","01000110","01000111",
    "01001000","01001001","01001010","01001011","01001100","01001101","01001110","01001111",
    "01010000","01010001","01010010","01010011","01010100","01010101","01010110","01010111",
    "01011000","01011001","01011010","01011011","01011100","01011101","01011110","01011111",
    "01100000","01100001","01100010","01100011","01100100","01100101","01100110","01100111",
    "01101000","01101001","01101010","01101011","01101100","01101101","01101110","01101111",
    "01110000","01110001","01110010","01110011","01110100","01110101","01110110","01110111",
    "01111000","01111001","01111010","01111011","01111100","01111101","01111110","01111111",
    "10000000","10000001","10000010","10000011","10000100","10000101","10000110","10000111",
    "10001000","10001001","10001010","10001011","10001100","10001101","10001110","10001111",
    "10010000","10010001","10010010","10010011","10010100","10010101","10010110","10010111",
    "10011000","10011001","10011010","10011011","10011100","10011101","10011110","10011111",
    "10100000","10100001","10100010","10100011","10100100","10100101","10100110","10100111",
    "10101000","10101001","10101010","10101011","10101100","10101101","10101110","10101111",
    "10110000","10110001","10110010","10110011","10110100","10110101","10110110","10110111",
    "10111000","10111001","10111010","10111011","10111100","10111101","10111110","10111111",
    "11000000","11000001","11000010","11000011","11000100","11000101","11000110","11000111",
    "11001000","11001001","11001010","11001011","11001100","11001101","11001110","11001111",
    "11010000","11010001","11010010","11010011","11010100","11010101","11010110","11010111",
    "11011000","11011001","11011010","11011011","11011100","11011101","11011110","11011111",
    "11100000","11100001","11100010","11100011","11100100","11100101","11100110","11100111",
    "11101000","11101001","11101010","11101011","11101100","11101101","11101110","11101111",
    "11110000","11110001","11110010","11110011","11110100","11110101","11110110","11110111",
    "11111000","11111001","11111010","11111011","11111100","11111101","11111110","11111111"
};


template<class _Int>
struct itos_base<2, _Int, wchar_t>
{
    static constexpr const size_t max_size = sizeof(_Int)*CHAR_BIT;
    static constexpr const size_t length = (1ULL<<CHAR_BIT); // 256
    static const wchar_t* const digits[length];
};

template<class _Int>
const wchar_t* const itos_base<2, _Int, wchar_t>::digits[]= {
    L"00000000",L"00000001",L"00000010",L"00000011",L"00000100",L"00000101",L"00000110",L"00000111",
    L"00001000",L"00001001",L"00001010",L"00001011",L"00001100",L"00001101",L"00001110",L"00001111",
    L"00010000",L"00010001",L"00010010",L"00010011",L"00010100",L"00010101",L"00010110",L"00010111",
    L"00011000",L"00011001",L"00011010",L"00011011",L"00011100",L"00011101",L"00011110",L"00011111",
    L"00100000",L"00100001",L"00100010",L"00100011",L"00100100",L"00100101",L"00100110",L"00100111",
    L"00101000",L"00101001",L"00101010",L"00101011",L"00101100",L"00101101",L"00101110",L"00101111",
    L"00110000",L"00110001",L"00110010",L"00110011",L"00110100",L"00110101",L"00110110",L"00110111",
    L"00111000",L"00111001",L"00111010",L"00111011",L"00111100",L"00111101",L"00111110",L"00111111",
    L"01000000",L"01000001",L"01000010",L"01000011",L"01000100",L"01000101",L"01000110",L"01000111",
    L"01001000",L"01001001",L"01001010",L"01001011",L"01001100",L"01001101",L"01001110",L"01001111",
    L"01010000",L"01010001",L"01010010",L"01010011",L"01010100",L"01010101",L"01010110",L"01010111",
    L"01011000",L"01011001",L"01011010",L"01011011",L"01011100",L"01011101",L"01011110",L"01011111",
    L"01100000",L"01100001",L"01100010",L"01100011",L"01100100",L"01100101",L"01100110",L"01100111",
    L"01101000",L"01101001",L"01101010",L"01101011",L"01101100",L"01101101",L"01101110",L"01101111",
    L"01110000",L"01110001",L"01110010",L"01110011",L"01110100",L"01110101",L"01110110",L"01110111",
    L"01111000",L"01111001",L"01111010",L"01111011",L"01111100",L"01111101",L"01111110",L"01111111",
    L"10000000",L"10000001",L"10000010",L"10000011",L"10000100",L"10000101",L"10000110",L"10000111",
    L"10001000",L"10001001",L"10001010",L"10001011",L"10001100",L"10001101",L"10001110",L"10001111",
    L"10010000",L"10010001",L"10010010",L"10010011",L"10010100",L"10010101",L"10010110",L"10010111",
    L"10011000",L"10011001",L"10011010",L"10011011",L"10011100",L"10011101",L"10011110",L"10011111",
    L"10100000",L"10100001",L"10100010",L"10100011",L"10100100",L"10100101",L"10100110",L"10100111",
    L"10101000",L"10101001",L"10101010",L"10101011",L"10101100",L"10101101",L"10101110",L"10101111",
    L"10110000",L"10110001",L"10110010",L"10110011",L"10110100",L"10110101",L"10110110",L"10110111",
    L"10111000",L"10111001",L"10111010",L"10111011",L"10111100",L"10111101",L"10111110",L"10111111",
    L"11000000",L"11000001",L"11000010",L"11000011",L"11000100",L"11000101",L"11000110",L"11000111",
    L"11001000",L"11001001",L"11001010",L"11001011",L"11001100",L"11001101",L"11001110",L"11001111",
    L"11010000",L"11010001",L"11010010",L"11010011",L"11010100",L"11010101",L"11010110",L"11010111",
    L"11011000",L"11011001",L"11011010",L"11011011",L"11011100",L"11011101",L"11011110",L"11011111",
    L"11100000",L"11100001",L"11100010",L"11100011",L"11100100",L"11100101",L"11100110",L"11100111",
    L"11101000",L"11101001",L"11101010",L"11101011",L"11101100",L"11101101",L"11101110",L"11101111",
    L"11110000",L"11110001",L"11110010",L"11110011",L"11110100",L"11110101",L"11110110",L"11110111",
    L"11111000",L"11111001",L"11111010",L"11111011",L"11111100",L"11111101",L"11111110",L"11111111"
};




template<class _Int, class _Char>
struct itos_base<8, _Int, _Char> {
    static constexpr const size_t max_size = sizeof(_Int)*3 + 1;
};



template<class _Int, class _Char>
struct itos_base<16, _Int, _Char>
{
    static constexpr const size_t max_size = sizeof(_Int)*2 + 1;
    static const _Char digits[];
};

template<class _Int, class _Char>
const _Char itos_base<16, _Int, _Char>::digits[] = {
    _Char('0'), _Char('1'), _Char('2'), _Char('3'),
    _Char('4'), _Char('5'), _Char('6'), _Char('7'),
    _Char('8'), _Char('9'), _Char('a'), _Char('b'),
    _Char('c'), _Char('d'), _Char('e'), _Char('f')
};


template<class _Int>
struct itos_base<10, _Int, char>
{
    static constexpr const size_t max_size = std::numeric_limits<_Int>::digits10 + 2;
    static constexpr const char* const digits =
            "0001020304050607080910111213141516171819"
            "2021222324252627282930313233343536373839"
            "4041424344454647484950515253545556575859"
            "6061626364656667686970717273747576777879"
            "8081828384858687888990919293949596979899";
};

template<class _Int>
struct itos_base<10, _Int, wchar_t>
{
    static constexpr const size_t max_size = std::numeric_limits<_Int>::digits10 + 2;
    static constexpr const wchar_t* const digits =
           L"0001020304050607080910111213141516171819"
            "2021222324252627282930313233343536373839"
            "4041424344454647484950515253545556575859"
            "6061626364656667686970717273747576777879"
            "8081828384858687888990919293949596979899";
};



//// IMPLEMENTATION ////

template<
    unsigned _Base,
    class _Int,
    class _OutIt,
    class _Char = char
>
struct __itos_impl :
        public itos_base<_Base, _Int, _Char>
{
    static_assert(_Base != 1, "incorrect base");

    typedef itos_base<_Base, _Int, _Char> base_type;

    static constexpr const size_t max_size = base_type::max_size;

    template<class _Up>
    static _OutIt cvt(_Up __val, _OutIt out)
    {

        _Char buf[max_size];
        unsigned __pos = max_size;

        while (__val >= (unsigned)_Base)
        {
            auto const __quo = __val / _Base;
            auto const __rem = __val % _Base;
            buf[--__pos] = base_type::digits[__rem];
            __val = __quo;
            if (__pos == 0) {
                out = std::copy(buf, buf + max_size, out);
                __pos = max_size;
            }
        }
        buf[__pos] = base_type::digits[__val];
        return std::copy(buf + __pos, buf + max_size, out);
    }
};

template<class _Int, class _OutIt, class _Char>
struct __itos_impl<0, _Int, _OutIt, _Char> :
        public itos_base<0, _Int, _Char>
{
    typedef itos_base<0, _Int, _Char> base_type;

    static constexpr const size_t max_size = base_type::max_size;

    template<class _Up>
    static _OutIt cvt(_Up __val, _OutIt out, unsigned base)
    {
        _Char buf[max_size];
        unsigned __pos = max_size;

        while (__val >= (unsigned)base)
        {
            auto const __quo = __val / base;
            auto const __rem = __val % base;
            buf[--__pos] = __itos_impl::digits[__rem];
            __val = __quo;
            if (__pos == 0) {
                out = std::copy(buf, buf + max_size, out);
                __pos = max_size;
            }
        }
        buf[__pos] = __itos_impl::digits[__val];
        return std::copy(buf + __pos, buf + max_size, out);
    }
};



template<class _Int, class _OutIt, class _Char>
struct __itos_impl<2, _Int, _OutIt, _Char> :
        public itos_base<2, _Int, _Char>
{
    typedef itos_base<2, _Int, _Char> base_type;

    static constexpr const size_t max_size = base_type::max_size;

    template<class _Up>
    static _OutIt cvt(_Up __val, _OutIt out)
    {
        static constexpr const unsigned bpw = sizeof(_Int)*CHAR_BIT;

        _Up mask = _Up(0xFF) << (bpw - CHAR_BIT);
        for(unsigned i = (bpw - CHAR_BIT); mask != 0; i -= CHAR_BIT, mask >>= CHAR_BIT) {
            const _Char* p = base_type::digits[(__val & mask) >> i];
            out = std::copy(p, p + CHAR_BIT, out);
        }
        return out;
    }
};




template<class _Int, class _OutIt, class _Char>
struct __itos_impl<8, _Int, _OutIt, _Char> :
        public itos_base<8, _Int, _Char>
{
    typedef itos_base<8, _Int, _Char> base_type;

    static constexpr const size_t max_size = base_type::max_size;

    static _OutIt cvt(_Int /*i*/, _OutIt out) {
        // TODO: implement me!
        return out;
    }
};




template<class _Int, class _OutIt, class _Char>
struct __itos_impl<10, _Int, _OutIt, _Char> :
        public itos_base<10, _Int, _Char>
{
    typedef itos_base<10, _Int, _Char> base_type;

    static constexpr const size_t max_size = base_type::max_size;

    template<class _Up>
    static _OutIt cvt(_Up __val, _OutIt out)
    {
        _Char buf[max_size];
        unsigned __pos = max_size;
        while (__val >= 100)
        {
            auto const __num = (__val % 100) * 2;
            __val /= 100;
            buf[--__pos] = base_type::digits[__num + 1];
            buf[--__pos] = base_type::digits[__num];
        }
        if (__val >= 10)
        {
            auto const __num = __val * 2;
            buf[--__pos] = base_type::digits[__num + 1];
            buf[--__pos] = base_type::digits[__num];
        }
        else {
            buf[--__pos] = '0' + __val;
        }

        return std::copy(buf + __pos, buf + max_size, out);
    }

#ifndef _NO_SSE_OPTIMIZATIONS
#if defined(i386) || defined(__amd64) || defined(_M_IX86) || defined(_M_X64)
#ifdef __SSE2__ // SSE2 optimized version
    static _OutIt cvt(uint32_t __val, _OutIt out) {
        char buf[max_size];
        char* highwater = u32toa_sse2(__val, buf);
        return std::copy(buf, highwater, out);
    }

    static _OutIt cvt(uint64_t __val, _OutIt out) {
        char buf[max_size];
        char* highwater = u64toa_sse2(__val, buf);
        return std::copy(buf, highwater, out);
    }
#endif
#endif
#endif
};


template<class _Int, class _OutIt, class _Char>
struct __itos_impl<16, _Int, _OutIt, _Char> :
        public itos_base<16, _Int, _Char>
{
    typedef itos_base<16, _Int, _Char> base_type;

    static constexpr const size_t max_size = base_type::max_size;

    template<class _Up>
    static _OutIt cvt(_Up __val, _OutIt out)
    {
        _Char buf[max_size];
        unsigned __pos = max_size;
        while (__val >= 0x100)
        {
            auto __num = __val & 0xF;
            __val >>= 4;
            buf[--__pos] = base_type::digits[__num];

            __num = __val & 0xF;
            __val >>= 4;
            buf[--__pos] = base_type::digits[__num];
        }
        if (__val >= 0x10)
        {
            const auto __num = __val & 0xF;
            __val >>= 4;
            buf[--__pos] = base_type::digits[__num];
            buf[--__pos] = base_type::digits[__val];
        }
        else
            buf[--__pos] = base_type::digits[__val];

        return std::copy(buf + __pos, buf + max_size, out);
    }
};



} // end namespace detail





template<unsigned _Base, class _Integer, class _OutIt>
inline _OutIt itos(_Integer i, _OutIt out)
{
    static_assert(_Base != 0 && _Base != 1, "incorrect base");
    using _Up = typename std::make_unsigned<_Integer>::type;
    if (i == 0) {
        if (_Base == 2) {
            std::fill_n(out, sizeof(_Integer)*CHAR_BIT, '0');
            return out;
        }
        *out = '0'; ++out;
        return out;
    }

    _Up __val = i;
    if (std::is_signed<_Integer>::value && _Base == 10) {
        if (i < 0) {
            *out = '-'; ++out;
            __val = _Up(~i) + _Up(1);
        }
    }

    return detail::__itos_impl<_Base, _Integer, _OutIt>::cvt(__val, out);
}



template<class _Integer, class _OutIt>
inline _OutIt itos(_Integer i, _OutIt out, unsigned base = 10)
{
    using _Up = typename std::make_unsigned<_Integer>::type;
    if (i == 0) {
        if (base == 2) {
            std::fill_n(out, sizeof(_Integer)*CHAR_BIT, '0');
            return out;
        }
        *out = '0'; ++out;
        return out;
    }

    _Up __val = i;
    if (std::is_signed<_Integer>::value) {
        if (i < 0 && base == 10) {
            *out = '-'; ++out;
            __val = _Up(~i) + _Up(1);
        }
    }

    switch (base)
    {
    case 16: // hexadecimal
        return itos<16>(__val, out);
    case 10: // decimal
        return itos<10>(__val, out);
    case 8:  // octal
        return itos<8>(__val, out);
    case 2:  // binary
        return itos<2>(__val, out);
    case 1:
    case 0: // invalid
        return out;
    default: // generic
        return detail::__itos_impl<0, _Integer, _OutIt>::cvt(__val, out, base);
    }
}





template<
   unsigned _Base,
   class _Integer,
   class _Char,
   class _Traits,
   class _Alloc
>
inline std::basic_string<_Char, _Traits, _Alloc>&
    itos(_Integer i, std::basic_string<_Char, _Traits, _Alloc>& s)
{
    using _Up = typename std::make_unsigned<_Integer>::type;

    using string_type = std::basic_string<_Char, _Traits, _Alloc>;
    using buf_type = detail::itos_base<_Base, _Integer, _Char>;
    using iterator_type = std::back_insert_iterator<string_type>;

    static constexpr const size_t max_size = buf_type::max_size;

    if (i == 0) {
        if (_Base == 2) {
            return s.append(sizeof(_Integer)*CHAR_BIT, _Char('0'));
        }
        s.push_back(_Char('0'));
        return s;
    }


    size_t n = s.size() + max_size;
    switch(_Base) {
    case 16:
    case 10:
    case 8:
    case 2:
        s.resize(n);
        s.erase(itos<_Base>(i, s.begin()), s.end());
        return s;
    case 1:
    case 0:
        return s;
    }

    // generic
    _Up __val = i;
    if (std::is_signed<_Integer>::value) {
        if (i < 0) {
            s.reserve(n+1);
            s.push_back(_Char('-'));
            __val = _Up(~i) + _Up(1);
        } else {
            s.reserve(n);
        }
    }

    detail::__itos_impl<_Base, _Integer, iterator_type, _Char>::cvt(__val, std::back_inserter(s));
    return s;
}




template<
   class _Integer,
   class _Char,
   class _Traits,
   class _Alloc
>
inline std::basic_string<_Char, _Traits, _Alloc>&
    itos(_Integer i, std::basic_string<_Char, _Traits, _Alloc>&  s, unsigned base = 10)
{
    using _Up = typename std::make_unsigned<_Integer>::type;

    using string_type = std::basic_string<_Char, _Traits, _Alloc>;
    using buf_type = detail::itos_base<0, _Integer, _Char>;
    using iterator_type = std::back_insert_iterator<string_type>;

    static constexpr const size_t max_size = buf_type::max_size;

    if (i == 0) {
        if (base == 2) {
            return s.append(sizeof(_Integer)*CHAR_BIT, _Char('0'));
        }
        s.push_back(_Char('0'));
        return s;
    }

    switch (base)
    {
    case 16: // hexadecimal
        return itos<16>(i, s);
    case 10: // decimal
        return itos<10>(i, s);
    case 8:  // octal
        return itos<8>(i, s);
    case 2:  // binary
        return itos<2>(i, s);
    case 1:
    case 0: // invalid
        return s;
    }

    size_t n = s.size() + max_size;
    // generic
    _Up __val = i;
    if (std::is_signed<_Integer>::value) {
        if (i < 0) {
            s.reserve(n+1);
            s.push_back(_Char('-'));
            __val = _Up(~i) + _Up(1);
        } else {
            s.reserve(n);
        }
    }

    detail::__itos_impl<0, _Integer, iterator_type, _Char>::cvt(__val, std::back_inserter(s), base);
    return s;
}


////// EXPERIMENTAL EXTENSIONS ///////


template<
    unsigned _Base,
    class _Int,
    class _Alphabet,
    size_t _MaxBufSize = 64
>
struct itos_custom : public _Alphabet
{
    typedef _Alphabet base_type;
    typedef _Alphabet alphabet;

    typedef typename _Alphabet::char_type char_type;

    typedef _Int int_type;

    static_assert(_Base != 1, "incorrect base");

    static constexpr const size_t max_size = _MaxBufSize;

    template<class _Up, class _OutIt>
    static _OutIt cvt(_Up __val, _OutIt out)
    {
        char_type buf[max_size];
        unsigned __pos = max_size;

        while (__val >= (unsigned)_Base)
        {
            auto const __quo = __val / _Base;
            auto const __rem = __val % _Base;
            buf[--__pos] = base_type::digits[__rem];
            __val = __quo;
            if (__pos == 0) {
                out = std::copy(buf, buf + max_size, out);
                __pos = max_size;
            }
        }
        buf[__pos] = base_type::digits[__val];
        return std::copy(buf + __pos, buf + max_size, out);
    }

    template<class _OutIt>
    static _OutIt zero(_OutIt out) {
        *out = char_type('0');
        return (++out);
    }
};




template<class _Policy, class _OutIt>
inline _OutIt itos_ext(typename _Policy::int_type i, _OutIt out)
{
    typedef typename _Policy::integer_type int_type;
    using _Up = typename std::make_unsigned<int_type>::type;
    if (i == 0) {
        return _Policy::zero(out);
    }

    _Up __val = i;
    if (std::is_signed<int_type>::value) {
        __val = _Up(~i) + _Up(1);
    }

    return _Policy::cvt(__val, out);
}



template<class _Policy, class _OutIt>
inline _OutIt itos_ext(typename _Policy::int_type i, _OutIt out, typename _Policy::char_type minus_symbol)
{
    typedef typename _Policy::integer_type int_type;
    using _Up = typename std::make_unsigned<int_type>::type;
    if (i == 0) {
        return _Policy::zero(out);
    }

    _Up __val = i;
    if (std::is_signed<int_type>::value) {
        if (i < 0) {
            *out = minus_symbol; ++out;
            __val = _Up(~i) + _Up(1);
        }
    }

    return _Policy::cvt(__val, out);
}




#ifndef _NO_SSE_OPTIMIZATIONS
#if defined(i386) || defined(__amd64) || defined(_M_IX86) || defined(_M_X64)
#ifdef __SSE2__ // SSE2 optimized version

namespace detail {

static const uint32_t kDiv10000 = 0xd1b71759;
ALIGN_PRE static const uint32_t kDiv10000Vector[4] ALIGN_SUF = { kDiv10000, kDiv10000, kDiv10000, kDiv10000 };
ALIGN_PRE static const uint32_t k10000Vector[4] ALIGN_SUF = { 10000, 10000, 10000, 10000 };
ALIGN_PRE static const uint16_t kDivPowersVector[8] ALIGN_SUF = { 8389, 5243, 13108, 32768, 8389, 5243, 13108, 32768 }; // 10^3, 10^2, 10^1, 10^0
ALIGN_PRE static const uint16_t kShiftPowersVector[8] ALIGN_SUF = {
    1 << (16 - (23 + 2 - 16)),
    1 << (16 - (19 + 2 - 16)),
    1 << (16 - 1 - 2),
    1 << (15),
    1 << (16 - (23 + 2 - 16)),
    1 << (16 - (19 + 2 - 16)),
    1 << (16 - 1 - 2),
    1 << (15)
};
ALIGN_PRE static const uint16_t k10Vector[8] ALIGN_SUF = { 10, 10, 10, 10, 10, 10, 10, 10 };
ALIGN_PRE static const char kAsciiZero[16] ALIGN_SUF = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };

static inline __m128i Convert8DigitsSSE2(uint32_t value) {
    assert(value <= 99999999);

    // abcd, efgh = abcdefgh divmod 10000
    const __m128i abcdefgh = _mm_cvtsi32_si128(value);
    const __m128i abcd = _mm_srli_epi64(_mm_mul_epu32(abcdefgh, reinterpret_cast<const __m128i*>(kDiv10000Vector)[0]), 45);
    const __m128i efgh = _mm_sub_epi32(abcdefgh, _mm_mul_epu32(abcd, reinterpret_cast<const __m128i*>(k10000Vector)[0]));

    // v1 = [ abcd, efgh, 0, 0, 0, 0, 0, 0 ]
    const __m128i v1 = _mm_unpacklo_epi16(abcd, efgh);

    // v1a = v1 * 4 = [ abcd * 4, efgh * 4, 0, 0, 0, 0, 0, 0 ]
    const __m128i v1a = _mm_slli_epi64(v1, 2);

    // v2 = [ abcd * 4, abcd * 4, abcd * 4, abcd * 4, efgh * 4, efgh * 4, efgh * 4, efgh * 4 ]
    const __m128i v2a = _mm_unpacklo_epi16(v1a, v1a);
    const __m128i v2 = _mm_unpacklo_epi32(v2a, v2a);

    // v4 = v2 div 10^3, 10^2, 10^1, 10^0 = [ a, ab, abc, abcd, e, ef, efg, efgh ]
    const __m128i v3 = _mm_mulhi_epu16(v2, reinterpret_cast<const __m128i*>(kDivPowersVector)[0]);
    const __m128i v4 = _mm_mulhi_epu16(v3, reinterpret_cast<const __m128i*>(kShiftPowersVector)[0]);

    // v5 = v4 * 10 = [ a0, ab0, abc0, abcd0, e0, ef0, efg0, efgh0 ]
    const __m128i v5 = _mm_mullo_epi16(v4, reinterpret_cast<const __m128i*>(k10Vector)[0]);

    // v6 = v5 << 16 = [ 0, a0, ab0, abc0, 0, e0, ef0, efg0 ]
    const __m128i v6 = _mm_slli_epi64(v5, 16);

    // v7 = v4 - v6 = { a, b, c, d, e, f, g, h }
    const __m128i v7 = _mm_sub_epi16(v4, v6);

    return v7;
}

static inline __m128i ShiftDigits_SSE2(__m128i a, unsigned digit) {
    assert(digit <= 8);
    switch (digit) {
        case 0: return a;
        case 1: return _mm_srli_si128(a, 1);
        case 2: return _mm_srli_si128(a, 2);
        case 3: return _mm_srli_si128(a, 3);
        case 4: return _mm_srli_si128(a, 4);
        case 5: return _mm_srli_si128(a, 5);
        case 6: return _mm_srli_si128(a, 6);
        case 7: return _mm_srli_si128(a, 7);
        case 8: return _mm_srli_si128(a, 8);
    }
    return a; // should not execute here.
}

static inline char* u32toa_sse2(uint32_t value, char* buffer) {
    if (value < 10000) {
        const uint32_t d1 = (value / 100) << 1;
        const uint32_t d2 = (value % 100) << 1;

        if (value >= 1000)
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1];
        if (value >= 100)
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1 + 1];
        if (value >= 10)
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2];
        *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2 + 1];
        *buffer++ = '\0'; // ###
    }
    else if (value < 100000000) {
        // Experiment shows that this case SSE2 is slower
#if 0
        const __m128i a = Convert8DigitsSSE2(value);

        // Convert to bytes, add '0'
        const __m128i va = _mm_add_epi8(_mm_packus_epi16(a, _mm_setzero_si128()), reinterpret_cast<const __m128i*>(kAsciiZero)[0]);

        // Count number of digit
        const unsigned mask = _mm_movemask_epi8(_mm_cmpeq_epi8(va, reinterpret_cast<const __m128i*>(kAsciiZero)[0]));
        unsigned long digit;
#ifdef _MSC_VER
        _BitScanForward(&digit, ~mask | 0x8000);
#else
        digit = __builtin_ctz(~mask | 0x8000);
#endif

        // Shift digits to the beginning
        __m128i result = ShiftDigits_SSE2(va, digit);
        //__m128i result = _mm_srl_epi64(va, _mm_cvtsi32_si128(digit * 8));
        _mm_storel_epi64(reinterpret_cast<__m128i*>(buffer), result);
        buffer[8 - digit] = '\0';
#else
        // value = bbbbcccc
        const uint32_t b = value / 10000;
        const uint32_t c = value % 10000;

        const uint32_t d1 = (b / 100) << 1;
        const uint32_t d2 = (b % 100) << 1;

        const uint32_t d3 = (c / 100) << 1;
        const uint32_t d4 = (c % 100) << 1;

        if (value >= 10000000)
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1];
        if (value >= 1000000)
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1 + 1];
        if (value >= 100000)
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2];
        *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2 + 1];

        *buffer++ = detail::itos_base<10, unsigned, char>::digits[d3];
        *buffer++ = detail::itos_base<10, unsigned, char>::digits[d3 + 1];
        *buffer++ = detail::itos_base<10, unsigned, char>::digits[d4];
        *buffer++ = detail::itos_base<10, unsigned, char>::digits[d4 + 1];
        *buffer++ = '\0'; // ###
#endif
    }
    else {
        // value = aabbbbbbbb in decimal

        const uint32_t a = value / 100000000; // 1 to 42
        value %= 100000000;

        if (a >= 10) {
            const unsigned i = a << 1;
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i + 1];
        }
        else
            *buffer++ = '0' + static_cast<char>(a);

        const __m128i b = Convert8DigitsSSE2(value);
        const __m128i ba = _mm_add_epi8(_mm_packus_epi16(_mm_setzero_si128(), b), reinterpret_cast<const __m128i*>(kAsciiZero)[0]);
        const __m128i result = _mm_srli_si128(ba, 8);
        _mm_storel_epi64(reinterpret_cast<__m128i*>(buffer), result);
        buffer[8] = '\0'; // ###
        return &buffer[8];
    }

    return buffer-1;
}



static inline char* u64toa_sse2(uint64_t value, char* buffer)
{
    if (value < 100000000) {
        uint32_t v = static_cast<uint32_t>(value);
        if (v < 10000) {
            const uint32_t d1 = (v / 100) << 1;
            const uint32_t d2 = (v % 100) << 1;

            if (v >= 1000)
                *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1];
            if (v >= 100)
                *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1 + 1];
            if (v >= 10)
                *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2 + 1];
            *buffer++ = '\0'; // ###
        }
        else {
            // Experiment shows that this case SSE2 is slower
#if 0
            const __m128i a = Convert8DigitsSSE2(v);

            // Convert to bytes, add '0'
            const __m128i va = _mm_add_epi8(_mm_packus_epi16(a, _mm_setzero_si128()), reinterpret_cast<const __m128i*>(kAsciiZero)[0]);

            // Count number of digit
            const unsigned mask = _mm_movemask_epi8(_mm_cmpeq_epi8(va, reinterpret_cast<const __m128i*>(kAsciiZero)[0]));
            unsigned long digit;
#ifdef _MSC_VER
            _BitScanForward(&digit, ~mask | 0x8000);
#else
            digit = __builtin_ctz(~mask | 0x8000);
#endif

            // Shift digits to the beginning
            __m128i result = ShiftDigits_SSE2(va, digit);
            _mm_storel_epi64(reinterpret_cast<__m128i*>(buffer), result);
            buffer[8 - digit] = '\0';
#else
            // value = bbbbcccc
            const uint32_t b = v / 10000;
            const uint32_t c = v % 10000;

            const uint32_t d1 = (b / 100) << 1;
            const uint32_t d2 = (b % 100) << 1;

            const uint32_t d3 = (c / 100) << 1;
            const uint32_t d4 = (c % 100) << 1;

            if (value >= 10000000)
                *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1];
            if (value >= 1000000)
                *buffer++ = detail::itos_base<10, unsigned, char>::digits[d1 + 1];
            if (value >= 100000)
                *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d2 + 1];

            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d3];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d3 + 1];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d4];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[d4 + 1];
            *buffer++ = '\0'; // ###
#endif
        }
    }
    else if (value < 10000000000000000) {
        const uint32_t v0 = static_cast<uint32_t>(value / 100000000);
        const uint32_t v1 = static_cast<uint32_t>(value % 100000000);

        const __m128i a0 = Convert8DigitsSSE2(v0);
        const __m128i a1 = Convert8DigitsSSE2(v1);

        // Convert to bytes, add '0'
        const __m128i va = _mm_add_epi8(_mm_packus_epi16(a0, a1), reinterpret_cast<const __m128i*>(kAsciiZero)[0]);

        // Count number of digit
        const unsigned mask = _mm_movemask_epi8(_mm_cmpeq_epi8(va, reinterpret_cast<const __m128i*>(kAsciiZero)[0]));
#ifdef _MSC_VER
        unsigned long digit;
        _BitScanForward(&digit, ~mask | 0x8000);
#else
        unsigned digit = __builtin_ctz(~mask | 0x8000);
#endif

        // Shift digits to the beginning
        __m128i result = ShiftDigits_SSE2(va, digit);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(buffer), result);
        buffer[16 - digit] = '\0'; // ###
        return &buffer[16 - digit];
    }
    else {
        const uint32_t a = static_cast<uint32_t>(value / 10000000000000000); // 1 to 1844
        value %= 10000000000000000;

        if (a < 10)
            *buffer++ = '0' + static_cast<char>(a);
        else if (a < 100) {
            const uint32_t i = a << 1;
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i + 1];
        }
        else if (a < 1000) {
            *buffer++ = '0' + static_cast<char>(a / 100);

            const uint32_t i = (a % 100) << 1;
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i + 1];
        }
        else {
            const uint32_t i = (a / 100) << 1;
            const uint32_t j = (a % 100) << 1;
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[i + 1];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[j];
            *buffer++ = detail::itos_base<10, unsigned, char>::digits[j + 1];
        }

        const uint32_t v0 = static_cast<uint32_t>(value / 100000000);
        const uint32_t v1 = static_cast<uint32_t>(value % 100000000);

        const __m128i a0 = Convert8DigitsSSE2(v0);
        const __m128i a1 = Convert8DigitsSSE2(v1);

        // Convert to bytes, add '0'
        const __m128i va = _mm_add_epi8(_mm_packus_epi16(a0, a1), reinterpret_cast<const __m128i*>(kAsciiZero)[0]);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(buffer), va);
        buffer[16] = '\0'; // ###
        return &buffer[16];
    }

    return buffer-1;
}

} // end namespace detail


_STDX_END


#undef ALIGN_PRE
#undef ALIGN_SUF

#endif // _NO_SSE_OPTIMIZATIONS
#endif // defined(i386) || defined(__amd64) || defined(_M_IX86) || defined(_M_X64)
#endif // __SSE2__


