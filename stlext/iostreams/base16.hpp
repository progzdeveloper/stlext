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
#include <ios>
#include <iosfwd>
#include <algorithm>
#include <cstring>

#include "../platform/common.h"
#include "codecs_alpha.hpp"

_STDX_BEGIN



typedef detail::alphabet<char,
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
> base16_alphabet;


template<class _Alphabet>
class base16_codec
{
    static const size_t num_hex_digits = (2 * sizeof(char));
    static_assert(_Alphabet::length == 16 * num_hex_digits, "incorrect alphbet length");

public:
    typedef std::char_traits<char> traits_type;
    typedef _Alphabet alphabet;

    base16_codec() :
        __m_flags(std::ios::fmtflags(0)) {
        std::memset(&__m_buf, 0, sizeof(__m_buf));
    }

    base16_codec(std::ios::fmtflags flags) :
        __m_flags(flags) {
        std::memset(&__m_buf, 0, sizeof(__m_buf));
    }

    template<class _OutIt>
    void decode(int ch, _OutIt& out)
    {
        auto it = std::find(std::begin(alphabet::value), std::end(alphabet::value), (char)ch);
        if (it != std::end(alphabet::value))
        {
            unsigned val = static_cast<unsigned>(it - std::begin(alphabet::value));
            __m_buf.buf = (__m_buf.buf << 4) + val;
            ++__m_buf.offset;
            if (__m_buf.offset == num_hex_digits) {
                *out = __m_buf.buf; ++out;
                __m_buf.offset = 0;
            }
        }
    }

    template<class _OutIt>
    void encode(int ch, _OutIt& out)
    {
        using namespace std;
        unsigned char val = static_cast<unsigned char>(ch);
        size_t offset = (__m_flags & ios::uppercase ? 16 : 0);
        __m_buf.res[num_hex_digits - 1] = alphabet::value[(val & 0x0F) + offset]; val >>= 4;
        __m_buf.res[num_hex_digits - 2] = alphabet::value[(val & 0x0F) + offset]; val >>= 4;
        out = std::copy_n(__m_buf.res, num_hex_digits, out);
    }


    template<class _OutIt>
    void flush(_OutIt&) {
        traits_type::assign(reinterpret_cast<char*>(&__m_buf), sizeof(__m_buf), 0);
    }

private:
    union {
        struct {
            unsigned char buf;
            unsigned char offset;
        };
        struct {
            char res[num_hex_digits];
        };
    } __m_buf;
    std::ios::fmtflags __m_flags;
};






template<>
class base16_codec<base16_alphabet>
{
    static const size_t num_hex_digits = (2 * sizeof(char));
    static_assert(base16_alphabet::length == 16 * num_hex_digits, "incorrect alphbet length");

public:
    typedef std::char_traits<char> traits_type;
    typedef base16_alphabet alphabet;

    base16_codec() :
        __m_flags(std::ios::fmtflags(0)) {
        std::memset(&__m_buf, 0, sizeof(__m_buf));
    }

    base16_codec(std::ios::fmtflags flags) :
        __m_flags(flags) {
        std::memset(&__m_buf, 0, sizeof(__m_buf));
    }

    template<class _OutIt>
    void decode(int ch, _OutIt& out)
    {
        unsigned val = 0;
        char c = static_cast<char> (ch);
        if (c >= '0' && c <= '9') val = c - '0';
        else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') val = c - 'a' + 10;
        else return;

        __m_buf.buf = (__m_buf.buf << 4) + val;
        ++__m_buf.offset;
        if (__m_buf.offset == num_hex_digits) {
            *out = __m_buf.buf; ++out;
            __m_buf.offset = 0;
        }
    }

    template<class _OutIt>
    void encode(int ch, _OutIt& out)
    {
        using namespace std;
        unsigned char val = static_cast<unsigned char>(ch);
        size_t offset = (__m_flags & ios::uppercase ? 16 : 0);
        __m_buf.res[num_hex_digits - 1] = alphabet::value[(val & 0x0F) + offset]; val >>= 4;
        __m_buf.res[num_hex_digits - 2] = alphabet::value[(val & 0x0F) + offset]; val >>= 4;
        out = std::copy_n(__m_buf.res, num_hex_digits, out);
    }


    template<class _OutIt>
    void flush(_OutIt&) {
        traits_type::assign(reinterpret_cast<char*>(&__m_buf), sizeof(__m_buf), 0);
    }

private:
    union {
        struct {
            unsigned char buf;
            unsigned char offset;
        };
        struct {
            char res[num_hex_digits];
        };
    } __m_buf;
    std::ios::fmtflags __m_flags;
};


typedef base16_codec<base16_alphabet> base16;


_STDX_END

