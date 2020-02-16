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
#include "../platform/common.h"

//
// selection between overloaded functions
// Example:
// int process(int) {}
// int process(int, int) {}
//
// overload<int, int>::of(process) // --> select `int process(int, int)`
// overload<int>::of(&process) // --> select `int process(int)`
// 
// const and non-const class methods also supported
//

_STDX_BEGIN

template <typename... Args>
struct non_const_overload
{
	template <typename R, typename T>
	constexpr auto operator()(R(T::*ptr)(Args...)) const -> decltype(ptr) {
		return ptr;
	}

	template <typename R, typename T>
	static constexpr auto of(R(T::*ptr)(Args...)) -> decltype(ptr) {
		return ptr;
	}
};

template <typename... Args>
struct const_overload
{
	template <typename R, typename T>
	constexpr auto operator()(R(T::*ptr)(Args...) const) const -> decltype(ptr) {
		return ptr;
	}

	template <typename R, typename T>
	static constexpr auto of(R(T::*ptr)(Args...) const) -> decltype(ptr) {
		return ptr;
	}
};

template <typename... Args>
struct overload : const_overload<Args...>, non_const_overload<Args...>
{
	using const_overload<Args...>::of;
	using const_overload<Args...>::operator();
	using non_const_overload<Args...>::of;
	using non_const_overload<Args...>::operator();

	template <typename R>
	constexpr auto operator()(R(*ptr)(Args...)) const -> decltype(ptr) {
		return ptr;
	}

	template <typename R>
	static constexpr auto of(R(*ptr)(Args...)) -> decltype(ptr) {
		return ptr;
	}
};

// C++14
// template <class... Args> static constexpr stdx::overload<Args...> overloaded = {};
// template <class... Args> static constexpr stdx::const_overload<Args...> const_overloaded = {};
// template <class... Args> static constexpr stdx::non_const_overload<Args...> non_const_overloaded = {};

_STDX_END

