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

#include <iosfwd>
#include <iterator>

#include "../platform/common.h"

_STDX_BEGIN

// Output iterator that inserts a delimiter between elements
// C++14 proposal
template<
	typename _Delim, 
	typename _Elem = char, 
	typename _Traits = std::char_traits<_Elem>
>
class ostream_joiner
{
public:
	typedef _Elem    char_type;
	typedef _Traits  traits_type;

	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;

	typedef std::basic_ostream<_Elem, _Traits> ostream_type;
	typedef std::output_iterator_tag           iterator_category;
	
	ostream_joiner(ostream_type& stream, const _Delim& delim) :
		__m_stream(std::addressof(stream)),
		__m_delim(delim),
		__m_first(true) {
	}

	ostream_joiner(ostream_type& stream, _Delim&& delim) :
		__m_stream(std::addressof(stream)),
		__m_delim(std::move(delim)),
		__m_first(true) {
	}

	template<typename T>
	ostream_joiner& operator= (const T& x) 
	{
		if (!__m_first)
			(*__m_stream) << __m_delim;
		__m_first = false;
		(*__m_stream) << x;
		return (*this);
	}

	ostream_joiner& operator*() { return (*this); }
	ostream_joiner& operator++() { return (*this); }
	ostream_joiner& operator++(int) { return (*this); }

private:
	ostream_type* __m_stream;
	_Delim __m_delim;
	bool  __m_first;
};

// Object generator for ostream_joiner
template<typename _Elem, typename _Traits, typename _Delim>
inline ostream_joiner<std::decay_t<_Delim>, _Elem, _Traits>
	make_ostream_joiner(std::basic_ostream<_Elem, _Traits>& stream, _Delim&& delim) {
	return{ stream, std::forward<_Delim>(delim) };
}

_STDX_END

