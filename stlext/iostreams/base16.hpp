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

_STDX_BEGIN

namespace {
	static constexpr const char* b16alpha = 
		"01234567" "89abcdef" 
		"01234567" "89ABCDEF";
}

class base16
{
	static const size_t num_hex_digits = (2 * sizeof(unsigned char));
	typedef std::char_traits<char> traits_type;

public:
	base16() :
		__m_flags(std::ios::fmtflags(0)) {
        std::memset(&__m_buf, 0, sizeof(__m_buf));
	}

	base16(std::ios::fmtflags flags) : 
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
		__m_buf.res[num_hex_digits - 1] = b16alpha[(val & 0x0F) + offset]; val >>= 4;
		__m_buf.res[num_hex_digits - 2] = b16alpha[(val & 0x0F) + offset]; val >>= 4;
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

_STDX_END

