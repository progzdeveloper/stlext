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
#include "../platform/common.h"
#include "iomanipbase.hpp"

// for deprecated functions
#include "../tuple/tupleio.hpp"

_STDX_BEGIN


template<size_t _BufSize>
class cformatter :
        public omanip< cformatter<_BufSize> >
{
    friend class omanip< cformatter<_BufSize> >;
public:
    template<class... _Args>
    cformatter(char(&buf)[_BufSize], const char* format,  _Args&&... args) :
        buffer(buf), length(0)
    {
        length = std::snprintf(buf, _BufSize, format, std::forward<_Args>(args)...);
    }

    template<class... _Args>
    cformatter(char(&buf)[_BufSize], const wchar_t* format,  _Args&&... args) :
        buffer(buf), length(0)
    {
        length = std::swprintf((wchar_t*)buf, _BufSize, format, std::forward<_Args>(args)...);
    }

    template<class... _Args>
    cformatter(wchar_t(&buf)[_BufSize], const wchar_t* format,  _Args&&... args) :
        buffer(buf), length(0)
    {
        length = std::swprintf(buf, _BufSize, format, std::forward<_Args>(args)...);
    }

private:
    template<class _Char, class _Traits>
    void do_put(std::basic_ostream<_Char, _Traits>& out) const {
        out.write((const _Char*)buffer, length);
    }

private:
    const void* buffer;
    size_t length;
};



template<size_t _BufSize, class... _Args>
inline cformatter<_BufSize> cformat(char(&buf)[_BufSize], const char* fmt,  _Args&&... args) {
    return cformatter<_BufSize>(buf, fmt, std::forward<_Args>(args)...);
}

template<size_t _BufSize, class... _Args>
inline cformatter<_BufSize> cformat(char(&buf)[_BufSize], const wchar_t* fmt,  _Args&&... args) {
    static_assert(_BufSize % sizeof(wchar_t) == 0, "unaligned buffer size");
    return cformatter<_BufSize>(buf, fmt, std::forward<_Args>(args)...);
}

template<size_t _BufSize, class... _Args>
inline cformatter<_BufSize> cformat(wchar_t(&buf)[_BufSize], const wchar_t* fmt,  _Args&&... args) {
    return cformatter<_BufSize>(buf, fmt, std::forward<_Args>(args)...);
}




template<>
class cformatter<0> :
        public omanip< cformatter<0> >
{
    friend class omanip< cformatter<0> >;

public:
    template<class... _Args>
    cformatter(const char* format,  _Args&&... args)
    {
        __reserve(format, std::forward<_Args>(args)...);
        size_t length = std::snprintf((char*)buffer.data(), buffer.size(), format, std::forward<_Args>(args)...);
        buffer.resize(length);
    }

    template<class... _Args>
    cformatter(const wchar_t* format,  _Args&&... args)
    {
        __reserve(format, std::forward<_Args>(args)...);
        size_t length = std::swprintf((wchar_t*)buffer.data(), buffer.size(), format, std::forward<_Args>(args)...);
        buffer.resize(length * sizeof(wchar_t));
    }

    template<class _Char, class _Traits>
    void do_put(std::basic_ostream<_Char, _Traits>& out) const {
        out.write((const _Char*)buffer.data(), buffer.size());
    }

private:
    template<class... _Args>
    inline void __reserve(const char* format, _Args&&... args) {
        buffer.resize(std::snprintf(nullptr, 0, format, std::forward<_Args>(args)...) + 1, 0);
    }

    template<class... _Args>
    inline void __reserve(const wchar_t* format, _Args&&... args) {
        buffer.resize((std::swprintf(nullptr, 0, format, std::forward<_Args>(args)...) + 1) * sizeof(wchar_t), 0);
    }

private:
    std::string buffer;
};



template<class... _Args>
inline cformatter<0> cformat(const char* fmt,  _Args&&... args) {
    return cformatter<0>(fmt, std::forward<_Args>(args)...);
}

template<class... _Args>
inline cformatter<0> cformat(const wchar_t* fmt,  _Args&&... args) {
    return cformatter<0>(fmt, std::forward<_Args>(args)...);
}




namespace deprecated {

/*! \deprecated */
template<typename _Elem, typename... _Args>
inline std::basic_string<_Elem>& format(std::basic_string<_Elem>& __str, const _Elem* __fmt, _Args&&... __args) {
	__str = put_tuple(__fmt, std::forward_as_tuple(__args...));
	return str;
}

/*! \deprecated */
template<typename _Elem, typename... _Args>
inline std::basic_string<_Elem> format(const _Elem* __fmt, _Args&&... __args) {
	return put_tuple(__fmt, std::forward_as_tuple(__args...));
}

}

_STDX_END

