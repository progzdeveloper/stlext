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

struct binary_scanner
{
	const unsigned char* buffer;
	size_t size;

	template<
		class _Char, 
		class _Traits, 
		class _Alloc
	>
	binary_scanner(std::basic_string<_Char, _Traits, _Alloc>& buf) :
		buffer(reinterpret_cast<const unsigned char*>(buf.data())), 
		size(buf.size() * sizeof(_Char)) {
	}

	binary_scanner(const char* buf, size_t n) :
		buffer(reinterpret_cast<const unsigned char*>(buf)), size(n) {
	}

	binary_scanner(const unsigned char* buf, size_t n) :
		buffer(buf), size(n) {
	}

	template<class T>
	void operator()(T& x) 
	{
        __stdx_assertx(sizeof(T) <= size, std::underflow_error, "buffer underflow");
		memmove(reinterpret_cast<char*>(std::addressof(x)), buffer, sizeof(T));
		buffer += sizeof(T);
		size -= sizeof(T);
	}
};

_STDX_END
