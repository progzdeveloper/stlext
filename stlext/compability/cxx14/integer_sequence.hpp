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

#include <type_traits>
#include "../../platform/common.h"

_STDX_BEGIN

/// C++17 proposal

// TEMPLATE STRUCT integer_sequence
template<class _Ty, _Ty... _Vals>
struct integer_sequence
{	// sequence of integer parameters
	static_assert(std::is_integral<_Ty>::value,
		"integer_sequence<T, I...> requires T to be an integral type.");

	typedef integer_sequence<_Ty, _Vals...> type;
	typedef _Ty value_type;

	static constexpr size_t size()
	{	// get length of parameter list
		return (sizeof...(_Vals));
	}
};

namespace detail {

	// ALIAS TEMPLATE make_integer_sequence
	template<
		bool _Negative,
		bool _Zero,
	class _Int_con,
	class _Int_seq
	>
	struct __make_seq
	{	// explodes gracefully below 0
		static_assert(!_Negative,
			"make_integer_sequence<T, N> requires N to be non-negative.");
	};

	template<class _Ty, _Ty... _Vals>
	struct __make_seq<
		false, true,
		std::integral_constant<_Ty, 0>,
		integer_sequence<_Ty, _Vals...>
	> : integer_sequence<_Ty, _Vals...>
	{	// ends recursion at 0
	};

	template<
		class _Ty,
			_Ty _Ix,
			_Ty... _Vals
	>
	struct __make_seq<
		false, false,
		std::integral_constant<_Ty, _Ix>,
		integer_sequence<_Ty, _Vals...>
	> : __make_seq<
			false, _Ix == 1,
			std::integral_constant<_Ty, _Ix - 1>,
			integer_sequence<_Ty, _Ix - 1, _Vals...>
	>
	{	// counts down to 0
	};

}


template<class _Ty, _Ty _Size>
using make_integer_sequence =
	typename detail::__make_seq<
		_Size < 0, _Size == 0,
		std::integral_constant<_Ty, _Size>,
		integer_sequence<_Ty>
	>::type;

template<size_t... _Vals>
using index_sequence = integer_sequence<size_t, _Vals...>;

template<size_t _Size>
using make_index_sequence = make_integer_sequence<size_t, _Size>;

template<typename... _Types>
using index_sequence_for = make_index_sequence< std::tuple_size< std::tuple<_Types...> >::value >;


_STDX_END


