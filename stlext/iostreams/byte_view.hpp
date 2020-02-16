// Copyright (c) 2018, Michael Polukarov (Russia).
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
#include <iostream>
#include <iomanip>

#include "../platform/common.h"

_STDX_BEGIN



template<std::ios_base::fmtflags _Radix>
struct byte_viewer_traits;

template<>
struct byte_viewer_traits<std::ios_base::hex>
{
    static constexpr size_t width = 2;
};

template<>
struct byte_viewer_traits<std::ios_base::oct>
{
    static constexpr size_t width = 3;
};

template<>
struct byte_viewer_traits<std::ios_base::dec>
{
    static constexpr size_t width = 3;
};



template<
    std::ios_base::fmtflags _Radix,
    class _InIt
>
class byte_viewer : public byte_viewer_traits<_Radix>
{
    template<std::ios_base::fmtflags R, class I>
    friend byte_viewer<R, I> make_view(I, I, std::ios_base::fmtflags, size_t, bool);

private:
    typedef byte_viewer_traits<_Radix> traits_type;

    byte_viewer(_InIt first, _InIt last, std::ios_base::fmtflags iofmt, size_t width, bool offsets) :
        __m_first(first),
        __m_last(last),
        __m_width(width),
        __m_ioflags(iofmt),
        __m_flag(offsets)
    {
    }

    byte_viewer() {
    }

    template<class _OStream>
    friend _OStream& operator<< (_OStream& stream, const byte_viewer& v) {
        return (v(stream));
    }

private:
    template<class _OStream>
    _OStream& operator()(_OStream& stream) const
    {
        size_t __m_offset = 0;
        size_t __m_curr = 0;
        stream.setf(_Radix, std::ios::basefield);
        stream.setf(__m_ioflags);
        for (; __m_first != __m_last; ++__m_first, ++__m_curr)
        {
            if (__m_flag && (__m_curr % __m_width) == 0) {
                offset(stream, __m_curr) << ": ";
            }
            stream << std::setw(traits_type::width) << std::setfill('0') << (uint16_t)(*__m_first) << ' ';
            if (__m_offset > 0 && (__m_offset % (__m_width - 1) == 0)) {
                stream << '\n';
                __m_offset = 0;
            }
            else {
                ++__m_offset;
            }
        }

        if (__m_offset != 0)
            stream << '\n';

        stream.unsetf(__m_ioflags);
        stream.unsetf(_Radix);
        stream.flush();
        return (stream);
    }


    template<class _OStream>
    static _OStream& offset(_OStream& stream, size_t off) {
        return (stream << std::setw(8) << std::setfill('0') << off);
    }


private:
    mutable _InIt __m_first;
    const _InIt __m_last;
    size_t __m_width;
    std::ios_base::fmtflags __m_ioflags;
    bool __m_flag;

};











template<std::ios_base::fmtflags _Radix, class _It>
byte_viewer<_Radix, _It> make_view(_It first, _It last, std::ios_base::fmtflags flags = (std::ios_base::fmtflags)0, size_t w = 16, bool offsets = false) {
    return byte_viewer<_Radix, _It>(first, last, flags, w, offsets);
}

template<std::ios_base::fmtflags _Radix, class _Container>
auto make_view(const _Container& c, std::ios_base::fmtflags flags = (std::ios_base::fmtflags)0, size_t w = 16, bool offsets = false) {
    return make_view<_Radix>(std::cbegin(c), std::cend(c), flags, w, offsets);
}


template<class _It>
auto hex_view(_It first, _It last, std::ios_base::fmtflags flags = (std::ios_base::fmtflags)0, size_t w = 16, bool offsets = false) {
    return make_view<std::ios::hex, _It>(first, last, flags, w, offsets);
}

template<class _Container>
auto hex_view(const _Container& c, std::ios_base::fmtflags flags = (std::ios_base::fmtflags)0, size_t w = 16, bool offsets = false) {
    return make_view<std::ios::hex>(std::cbegin(c), std::cend(c), flags, w, offsets);
}

template<class _It>
auto oct_view(_It first, _It last, std::ios_base::fmtflags flags = (std::ios_base::fmtflags)0, size_t w = 16, bool offsets = false) {
    return make_view<std::ios::oct, _It>(first, last, flags, w, offsets);
}

template<class _Container>
auto oct_view(const _Container& c, std::ios_base::fmtflags flags = (std::ios_base::fmtflags)0, size_t w = 16, bool offsets = false) {
    return make_view<std::ios::oct>(std::cbegin(c), std::cend(c), flags, w, offsets);
}

_STDX_END

