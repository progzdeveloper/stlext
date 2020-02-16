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
#include <string>
#include <regex>
#include <iterator>

#include "../../platform/common.h"

_STDX_BEGIN

template<
	class _Regex,
	class _BiIt,
	class _OutIt
>
_OutIt regex_split(const _Regex& rx, _BiIt first, _BiIt last, _OutIt out)
{
    //typedef typename std::iterator_traits<_BiIt>::value_type elem_type;
    //typedef std::basic_regex<elem_type> regex_type;
	typedef std::regex_iterator<_BiIt> iter_type;


	iter_type rxlast;
	iter_type rxfirst(first, last, rx);
	for (; rxfirst != rxlast; ++rxfirst, ++out) {
		*out = rxfirst->str();
	}
	return out;
}



template<
	class _Elem,
	class _Traits,
	class _Alloc,
	class _BiIt,
	class _OutIt
>
_OutIt regex_split(const std::basic_string<_Elem, _Traits, _Alloc>& expr,
				   _BiIt first, _BiIt last, _OutIt out)
{
	typedef typename std::iterator_traits<_BiIt>::value_type elem_type;
	return regex_split(std::basic_regex<elem_type>(expr), first, last, out);
}


template<
	class _Regex,
	class _Elem,
	class _Traits,
	class _Alloc,
	class _OutIt
>
_OutIt regex_split(const _Regex& rx,
				   const std::basic_string<_Elem, _Traits, _Alloc>& line,
				   _OutIt out)
{
	return regex_split(rx, line.begin(), line.end(), out);
}


template<
	class _Regex,
	class _Elem,
	class _OutIt
>
_OutIt regex_split(const _Regex& rx, const _Elem* line, _OutIt out)
{
	return regex_split(rx, line, line + std::char_traits<_Elem>::length(line), out);
}


template<
	class _Elem,
	class _Traits,
	class _Alloc,
	class _OutIt
>
_OutIt regex_split(const _Elem* expr,
				   const std::basic_string<_Elem, _Traits, _Alloc>& line,
				   _OutIt out)
{
	return regex_split(std::basic_regex<_Elem>(expr), line.begin(), line.end(), out);
}


template<
	class _Elem,
	class _OutIt
>
_OutIt regex_split(const _Elem* expr, const _Elem* line, _OutIt out)
{
	return regex_split(std::basic_regex<_Elem>(expr), line, line + std::char_traits<_Elem>::length(line), out);
}


template<
	class _Elem,
	class _Traits,
	class _Alloc,
	class _OutIt
>
_OutIt regex_split(const std::basic_string<_Elem, _Traits, _Alloc>& expr,
				   const std::basic_string<_Elem, _Traits, _Alloc>& line,
				   _OutIt out)
{
	return regex_split(std::basic_regex<_Elem>(expr), line.begin(), line.end(), out);
}


_STDX_END


