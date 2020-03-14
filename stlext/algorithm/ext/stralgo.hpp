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

#include <string>
#include <algorithm>
#include <iterator>

#include "../../platform/common.h"

_STDX_BEGIN

template<class _InIt,  class _OutIt>
inline _OutIt to_upper(_InIt first, _InIt last, _OutIt out) {
    return std::transform(first, last, out, [](auto c) {
        return static_cast<decltype(c)>(toupper(c));
    });
}

template<class _Char, class _Alloc, class _Traits, class _OutIt>
inline _OutIt to_upper(const std::basic_string<_Char, _Alloc, _Traits>& s, _OutIt out)
{
    return to_upper(s.begin(), s.end(), out);
}

template<class _Char, class _Alloc, class _Traits>
inline std::basic_string<_Char, _Alloc, _Traits> to_upper(const std::basic_string<_Char, _Alloc, _Traits>& s)
{
    std::basic_string<_Char, _Alloc, _Traits> result;
    result.reserve(s.size());
    to_upper(s, std::back_inserter(result));
    return result;
}




template<class _InIt,  class _OutIt>
inline _OutIt to_lower(_InIt first, _InIt last, _OutIt out)
{
    return std::transform(first, last, out, [](auto c) {
        return static_cast<decltype(c)>(tolower(c));
    });
}

template<class _Char, class _Alloc, class _Traits, class _OutIt>
inline _OutIt to_lower(const std::basic_string<_Char, _Alloc, _Traits>& s, _OutIt out)
{
    return to_lower(s.begin(), s.end(), out);
}

template<class _Char, class _Alloc, class _Traits>
inline std::basic_string<_Char, _Alloc, _Traits> to_lower(const std::basic_string<_Char, _Alloc, _Traits>& s)
{
    std::basic_string<_Char, _Alloc, _Traits> result;
    result.reserve(s.size());
    to_lower(s, std::back_inserter(result));
    return result;
}


_STDX_END
