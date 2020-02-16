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
#include "../platform/common.h"
#include "../compability/integer_sequence"

_STDX_BEGIN

namespace detail {
	/// TRANSFORM IMPLEMENTATION DETAILS

	struct element_assigner {
		template<class T, class R, class _Act>
		inline void operator()(const T& x, R&& r, _Act&& op) {
			r = op(x);
		}

		template<class T, class R, class _Act>
		inline void operator()(const T& x, const T& y, R&& r, _Act&& op) {
			r = op(x, y);
		}
	};


	template<class _Action, class... _Src, class... _Dst, size_t... _Idx>
	inline void tuple_transform1(_Action&& act,
								 const std::tuple<_Src...>& src,
								 std::tuple<_Dst...>& dst,
								 std::index_sequence<_Idx...>)
	{
		using std::get;
		int unused[] = { 0, ((void)element_assigner{}(get<_Idx>(src), get<_Idx>(dst), act), 0)... };
		(void)unused;
	}


	template<class _Action, class... _Src, class... _Dst, size_t... _Idx>
	inline void tuple_transform2(_Action&& act,
								 const std::tuple<_Src...>& lhs, const std::tuple<_Src...>& rhs,
								 std::tuple<_Dst...>& dst, std::index_sequence<_Idx...>)
	{
		using std::get;
		int unused[] = { 0, ((void)element_assigner{}(get<_Idx>(lhs), get<_Idx>(rhs), get<_Idx>(dst), act), 0)... };
		(void)unused;
	}


} // end namespace detail


/// TRANSFORM TUPLE

template<class _Action, class... _Src, class... _Dst>
void tuple_transform(const std::tuple<_Src...>& src, std::tuple<_Dst...>& dst, _Action&& act)
{
	static_assert(sizeof...(_Src) == sizeof...(_Dst), "tuple size mismatch");
	detail::tuple_transform1(act, src, dst, std::make_index_sequence < sizeof...(_Src) > {});
}


template<class _Act, class... _Src, class... _Dst>
void tuple_transform(const std::tuple<_Src...>& __lhs, const std::tuple<_Dst...>& __rhs, std::tuple<_Dst...>& __dst, _Act&& __act)
{
	static_assert(sizeof...(_Src) == sizeof...(_Dst), "tuple size mismatch");
	detail::tuple_transform2(__act, __lhs, __rhs, __dst, std::make_index_sequence < sizeof...(_Src) > {});
}


_STDX_END


