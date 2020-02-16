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
#include <iostream>
#include <string>
#include <type_traits>

#include "../platform/common.h"

_STDX_BEGIN

template<typename T>
static inline std::istream& prompt_for(const char* message, T& result) 
{
	if (message)
		std::cout << message;
	return (std::cin >> result);
}

template<typename T>
static inline std::wistream& prompt_for(const wchar_t* message, T& result)
{
	if (message)
		std::wcout << message;
	return (std::wcin >> result);
}


static inline  std::istream& prompt_for(const char* message, std::string& result)
{
	if (message)
		std::cout << message;
	int c = std::cin.get();
	if (c != std::cin.widen('\n'))
		std::cin.putback(c);
	return std::getline(std::cin, result);
}


static inline std::wistream& prompt_for(const wchar_t* message, std::wstring& result)
{
	if (message)
		std::wcout << message;
	int c = std::wcin.get();
	if (c != std::wcin.widen('\n'))
		std::wcin.putback(c);
	return std::getline(std::wcin, result);
}


template<
	typename T,
	typename _Elem,
	typename _Traits,
	typename _Allocator
>
static inline std::basic_istream<_Elem, _Traits>&
	prompt_for(const std::basic_string<_Elem, _Traits, _Allocator>& message, T& result)
{
	static_assert(std::is_same<_Elem, char>::value || std::is_same<_Elem, wchar_t>::value,
		"char type is not supported");
	return prompt_for(message.c_str(), result);
}



_STDX_END
