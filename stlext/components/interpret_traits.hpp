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
#include <tuple>
#include <string>
#include <sstream>
#include "stream_scanner.hpp"

#include "../tuple/tuple_foreach.hpp"
#include "../tuple/tuple_apply.hpp"

_STDX_BEGIN

/*!
 * \brief 
 */
template<class _Source>
struct interpret_traits;


template<
	class _Elem, 
	class _Traits,
	class _Alloc
>
struct interpret_traits<
	std::basic_string<_Elem, _Traits, _Alloc> 
>
{
	typedef std::basic_string<_Elem, _Traits, _Alloc> source_type;
	typedef std::basic_istringstream<_Elem, _Traits, _Alloc> stream_type;
	typedef stream_scanner<_Elem, _Traits> scanner_type;

	template<class... _Args>
	static void interpret(const source_type& source,
						  std::tuple<_Args...>& args) 
	{
		stream_type buffer(source);
		stdx::foreach_element(scanner_type(buffer, std::ios::in), args);
	}

};



template<
	class _Elem,
	class _Traits
>
struct interpret_traits<
	std::basic_iostream<_Elem, _Traits>
>
{
	typedef std::basic_iostream<_Elem, _Traits> source_type;
	typedef stream_scanner<_Elem, _Traits> scanner_type;

	template<class... _Args>
	static void interpret(const source_type& source,
						  std::tuple<_Args...>& args)
	{
		source_type& source_ref = const_cast<source_type&>(source);
		auto pos = source_ref.tellg();
		stdx::foreach_element(scanner_type(source_ref, std::ios::in), args);
		source_ref.seekg(pos);
		source_ref.clear();
	}
};

template<
	class _Elem,
	class _Traits
>
struct interpret_traits<
	std::basic_istream<_Elem, _Traits>
>
{
	typedef std::basic_istream<_Elem, _Traits> source_type;
	typedef stream_scanner<_Elem, _Traits> scanner_type;

	template<class... _Args>
	static void interpret(const source_type& source,
						  std::tuple<_Args...>& args)
	{
		source_type& source_ref = const_cast<source_type&>(source);
		auto pos = source_ref.tellg();
		stdx::foreach_element(scanner_type(source_ref, std::ios::in), args);
		source_ref.seekg(pos);
		source_ref.clear();
	}
};

_STDX_END
