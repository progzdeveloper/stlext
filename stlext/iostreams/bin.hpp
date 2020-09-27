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
#include <iostream>
#include <limits>
#include <type_traits>
#include <bitset>

#include "../platform/common.h"
#include "../bitvector/bitvector.hpp"

_STDX_BEGIN

namespace detail
{

    // generic implemetation
    template<typename _Num>
    class binrep
    {
        typedef typename std::make_unsigned<_Num>::type _Unsigned;
    public:
        typedef _Num argument_type;

        static const size_t bpw = std::numeric_limits<_Unsigned>::digits;

        binrep(const argument_type& x, size_t digits, const char* delim) :
            __m_val(x), __m_ndgts(digits), __m_sep(delim) {
        }

        template<typename _Elem, typename _Traits>
        friend std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& stream, const binrep& rep)
        {
            if (stream.flags() & std::ios::showbase) {
                stream.put((_Elem)'b');
            }

            uint8_t n = (bpw - 1);
            for (int8_t pos = n; pos >= 0; --pos) {
                if (rep.__m_ndgts && (pos != static_cast<std::ptrdiff_t>(n)) && !((pos + 1) % rep.__m_ndgts))
                    stream << rep.__m_sep;
                stream << _Elem('0' + (bool)((_Unsigned)rep.__m_val & (_Unsigned(1) << pos)));
            }
            return (stream << std::flush);
        }

        const argument_type& __m_val;
        size_t __m_ndgts;
        const char* __m_sep;
    };


    // bitvector<> specialization
    template<
        class _Word,
        class _Alloc,
        size_t _Opt
    >
    class binrep< stdx::bitvector<_Word, _Alloc, _Opt> >
    {
    public:
        typedef stdx::bitvector<_Word, _Alloc, _Opt> argument_type;

        binrep(const argument_type& x, size_t digits, const char* delim) :
            __m_val(x), __m_ndgts(digits), __m_sep(delim) {
        }

        template<typename _Elem, typename _Traits>
        friend std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& stream,
                                                              const binrep<argument_type>& rep)
        {
            if (stream.flags() & std::ios::showbase) {
                stream.put((_Elem)'b');
            }

            size_t n = rep.__m_val.size() - 1;
            for (ptrdiff_t pos = n; pos >= 0; --pos) {
                if (rep.__m_ndgts && (pos != static_cast<std::ptrdiff_t>(n)) && !((pos + 1) % rep.__m_ndgts))
                    stream << rep.__m_sep;
                stream << _Elem('0' + rep.__m_val.test(pos));
            }
            return (stream);
        }

        const argument_type& __m_val;
        size_t __m_ndgts;
        const char* __m_sep;
    };


    // bitset<> specialization
    template<size_t _Size>
    class binrep < std::bitset<_Size> >
    {
    public:
        typedef std::bitset<_Size> argument_type;

        binrep(const argument_type& x, size_t digits, const char* delim) :
            __m_val(x), __m_ndgts(digits),
            __m_sep(delim), __m_sep_len(delim != nullptr ? std::char_traits<char>::length(delim) : 0) {
        }

        template<typename _Elem, typename _Traits>
        friend std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& stream,
                                                              const binrep< argument_type >& rep)
        {
            if (stream.flags() & std::ios::showbase) {
                stream.put((_Elem)'b');
            }

            size_t n = rep.__m_val.size() - 1;
            for (ptrdiff_t pos = n; pos >= 0; --pos) {
                if (rep.__m_ndgts && (pos != static_cast<std::ptrdiff_t>(n)) && !((pos + 1) % rep.__m_ndgts))
                    stream.write((const _Elem*)rep.__m_sep, rep.__m_sep_len);
                stream << _Elem('0' + rep.__m_val.test(pos));
            }
            return (stream);
        }

        const argument_type& __m_val;
        size_t __m_ndgts;
        const char* __m_sep;
        size_t __m_sep_len;
    };

} // end namespace detail


template<class T>
detail::binrep<T> bin(const T& x, size_t digits = 0, const char* delim = " ") {
    static_assert(std::is_integral<T>::value, "T is not an integral type");
    return detail::binrep<T>(x, digits, delim);
}

template<class _Word, class _Alloc, size_t _Opt>
detail::binrep< stdx::bitvector<_Word, _Alloc, _Opt> > bin(const stdx::bitvector<_Word, _Alloc, _Opt>& x,
                                                           size_t digits = 0, const char* delim = " ") {
    typedef stdx::bitvector<_Word, _Alloc, _Opt> value_type;
    return detail::binrep<value_type>(x, digits, delim);
}

template<size_t _Size>
detail::binrep< std::bitset<_Size> > bin(const std::bitset<_Size>& x,
                                         size_t digits = 0, const char* delim = " ") {
    typedef std::bitset<_Size> value_type;
    return detail::binrep<value_type>(x, digits, delim);
}

_STDX_END

