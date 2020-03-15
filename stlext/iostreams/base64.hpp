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
#include "../platform/common.h"
#include "codecs_alpha.hpp"

_STDX_BEGIN


template<class _Alphabet>
class base64_codec
{
    static_assert(64 == _Alphabet::length, "alphabet size mismatch");

public:
    typedef _Alphabet alphabet;
    typedef typename _Alphabet::char_type char_type;

    base64_codec() :
        buf(0), nbits(0) {
    }

    template<class _OutIt>
    void decode(int ch, _OutIt& out)
    {
        auto it = std::find(std::begin(alphabet::value), std::end(alphabet::value), (char_type)ch);
        if (it != std::end(alphabet::value))
        {
            int d = (int)(it - std::begin(alphabet::value));
            buf = (buf << 6) | d;
            nbits += 6;
            if (nbits >= 8) {
                nbits -= 8;
                *out = (buf >> nbits); ++out;
                buf &= (1 << nbits) - 1;
            }
        }
    }

    template<class _OutIt>
    void encode(int ch, _OutIt& out)
    {
        if (nbits == 2) {
            buf |= int((unsigned char)ch);
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = alphabet::value[(buf & 0x00000FC0) >> 6]; ++out;
            *out = alphabet::value[(buf & 0x0000003F)]; ++out;
            nbits = 0;
            buf = 0;
        }
        else {
            buf |= int((unsigned char)ch) << (1 << (4 - nbits));
            ++nbits;
        }
    }


    template<class _OutIt>
    void flush(_OutIt& out)
    {
        static constexpr char padding = '=';
        switch (nbits)
        {
        case 1:
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = padding; ++out;
            *out = padding; ++out;
            break;
        case 2:
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = alphabet::value[(buf & 0x00000FC0) >> 6]; ++out;
            *out = padding; ++out;
            break;
        }
        buf = 0;
        nbits = 0;
    }

private:
    unsigned int buf : 24;
    int nbits : 8;
};





typedef detail::alphabet<char,
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
> base64_rfc4648_alphabet;

template<>
class base64_codec<base64_rfc4648_alphabet>
{
    typedef base64_rfc4648_alphabet alphabet;
public:
    base64_codec() :
        buf(0), nbits(0) {
    }

    template<class _OutIt>
    void decode(int ch, _OutIt& out)
    {
        int d;

        if (ch >= 'A' && ch <= 'Z')
            d = ch - 'A';
        else if (ch >= 'a' && ch <= 'z')
            d = ch - 'a' + 26;
        else if (ch >= '0' && ch <= '9')
            d = ch - '0' + 52;
        else if (ch == '+')
            d = 62;
        else if (ch == '/')
            d = 63;
        else
            d = -1;

        if (d != -1)
        {
            buf = (buf << 6) | d;
            nbits += 6;
            if (nbits >= 8) {
                nbits -= 8;
                *out = (buf >> nbits); ++out;
                buf &= (1 << nbits) - 1;
            }
        }
    }

    template<class _OutIt>
    void encode(int ch, _OutIt& out)
    {
        if (nbits == 2) {
            buf |= int((unsigned char)ch);
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = alphabet::value[(buf & 0x00000FC0) >> 6]; ++out;
            *out = alphabet::value[(buf & 0x0000003F)]; ++out;
            nbits = 0;
            buf = 0;
        }
        else {
            buf |= int((unsigned char)ch) << (1 << (4 - nbits));
            ++nbits;
        }
    }


    template<class _OutIt>
    void flush(_OutIt& out)
    {
        static constexpr char padding = '=';
        switch (nbits)
        {
        case 1:
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = padding; ++out;
            *out = padding; ++out;
            break;
        case 2:
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = alphabet::value[(buf & 0x00000FC0) >> 6]; ++out;
            *out = padding; ++out;
            break;
        }
        buf = 0;
        nbits = 0;
    }

private:
    unsigned int buf : 24;
    int nbits : 8;
};

typedef base64_codec<base64_rfc4648_alphabet> base64_rfc4648;
typedef base64_rfc4648 base64;







typedef detail::alphabet<char,
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '-', '_'
> base64_url_alphabet;

template<>
class base64_codec<base64_url_alphabet>
{
    typedef base64_url_alphabet alphabet;
public:
    base64_codec() :
        buf(0), nbits(0) {
    }

    template<class _OutIt>
    void decode(int ch, _OutIt& out)
    {
        int d;

        if (ch >= 'A' && ch <= 'Z')
            d = ch - 'A';
        else if (ch >= 'a' && ch <= 'z')
            d = ch - 'a' + 26;
        else if (ch >= '0' && ch <= '9')
            d = ch - '0' + 52;
        else if (ch == '-')
            d = 62;
        else if (ch == '_')
            d = 63;
        else
            d = -1;

        if (d != -1)
        {
            buf = (buf << 6) | d;
            nbits += 6;
            if (nbits >= 8) {
                nbits -= 8;
                *out = (buf >> nbits); ++out;
                buf &= (1 << nbits) - 1;
            }
        }
    }

    template<class _OutIt>
    void encode(int ch, _OutIt& out)
    {
        if (nbits == 2) {
            buf |= int((unsigned char)ch);
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = alphabet::value[(buf & 0x00000FC0) >> 6]; ++out;
            *out = alphabet::value[(buf & 0x0000003F)]; ++out;
            nbits = 0;
            buf = 0;
        }
        else {
            buf |= int((unsigned char)ch) << (1 << (4 - nbits));
            ++nbits;
        }
    }


    template<class _OutIt>
    void flush(_OutIt& out)
    {
        static constexpr char padding = '=';
        switch (nbits)
        {
        case 1:
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = padding; ++out;
            *out = padding; ++out;
            break;
        case 2:
            *out = alphabet::value[(buf & 0x00FC0000) >> 18]; ++out;
            *out = alphabet::value[(buf & 0x0003F000) >> 12]; ++out;
            *out = alphabet::value[(buf & 0x00000FC0) >> 6]; ++out;
            *out = padding; ++out;
            break;
        }
        buf = 0;
        nbits = 0;
    }

private:
    unsigned int buf : 24;
    int nbits : 8;
};

typedef base64_codec<base64_url_alphabet> base64_url;


_STDX_END
