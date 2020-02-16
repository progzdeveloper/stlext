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
	/// FOR_EACH ELEMENT IMPLEMENTATION DETAILS

	template<class _Fn, class... _Args, size_t... _Idx>
	inline void foreach_element(_Fn&& __act, const std::tuple<_Args...>& __args, stdx::index_sequence<_Idx...>) {
		using namespace std;
		int unused[] = { 0, ((void)forward<_Fn>(__act)(get<_Idx>(__args)), 0)... };
		(void)unused;
	}

	template<class _Fn, class... _Args, size_t... _Idx>
	inline void foreach_element(_Fn&& __act, std::tuple<_Args...>& __args, stdx::index_sequence<_Idx...>) {
		using namespace std;
		int unused[] = { 0, ((void)forward<_Fn>(__act)(get<_Idx>(__args)), 0)... };
		(void)unused;
	}

} // end namespace detail

/// FOR_EACH ELEMENT IN TUPLE

template<class _Fn, class... _Args>
inline void foreach_element(_Fn&& act, const std::tuple<_Args...>& args) {
	detail::foreach_element(std::forward<_Fn>(act), args, stdx::make_index_sequence < sizeof...(_Args) > {});
}

template<class _Fn, class... _Args>
inline void foreach_element(_Fn&& act, std::tuple<_Args...>& args) {
	detail::foreach_element(std::forward<_Fn>(act), args, stdx::make_index_sequence < sizeof...(_Args) > {});
}


/// FOR_EACH ARGUMENT IN ARGUMENT PACK 

template<class _Fn, class... _Args>
inline void foreach_arg(_Fn&& act, _Args&&... args) {
	foreach_element(std::forward<_Fn>(act), std::forward_as_tuple(args...));
}




_STDX_END

