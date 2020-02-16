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
#include <string>
#include <stdexcept>

#include "../platform/common.h"

_STDX_BEGIN

template<
	class _Char = char, 
	class _Traits = std::char_traits<_Char>
>
struct stream_scanner
{
	typedef std::basic_istream<_Char, _Traits> stream_type;

	stream_type* stream;
	int mode;

	stream_scanner(stream_type& s, int m) :
		stream(std::addressof(s)), mode(m) {
	}

	template<class T>
	void operator()(T& x) 
	{
		if (mode & std::ios::binary) {
			if (!(stream->read(reinterpret_cast<char*>(std::addressof(x)), sizeof(T)))) {
				throw std::invalid_argument("invalid argument(s)");
			}
		}
		else {
			if (!((*stream) >> x)) {
				throw std::invalid_argument("invalid argument(s)");
			}
		}
	}

	inline void operator()(std::basic_string<_Char, _Traits>& x)
	{
		if (mode & std::ios::binary) 
		{
			size_t n = 0;
			if (!(stream->read(reinterpret_cast<char*>(&n), sizeof(size_t)))) 
				throw std::invalid_argument("invalid argument(s)");
			
			x.resize(n);
			if (!(stream->read(const_cast<char*>(x.data()), sizeof(_Char) * n))) 
				throw std::invalid_argument("invalid argument(s)");
			
		}
		else {
			if (!((*stream) >> x))
				throw std::invalid_argument("invalid argument(s)");
		}
	}

};



_STDX_END

