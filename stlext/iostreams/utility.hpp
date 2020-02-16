// Copyright (c) 2013-2016, Michael Polukarov (Russia).
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
#include <clocale>
#include <locale>
#include <string>
#include <system_error>

#include "../platform/common.h"

_STDX_BEGIN

inline std::string error_message(const std::string& s,
								 int val = errno,
								 const std::error_category& c = std::generic_category()) {
	std::string text(s);
	text += c.message(val);
	return text;
}

inline std::string error_message(int val = errno,
								 const std::error_category& c = std::generic_category()) {
	return c.message(val);
}

inline std::string error_message(const std::error_code& code) {
	return code.message();
}


static inline std::locale system_locale() {
	return std::locale(::setlocale(LC_ALL, ""));
}


template<class T>
inline std::size_t count_digits(T x, size_t radix = 10) {
    std::size_t n = 1;
	while (x /= static_cast<T>(radix)) { 
		++n; 
	}
	return n;
}

template<std::size_t _Radix, class T>
inline std::size_t count_digits(T x) {
    std::size_t n = 1;
	while (x /= static_cast<T>(_Radix)) {
		++n;
	}
	return n;
}


_STDX_END
