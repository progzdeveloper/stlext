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
#include <chrono>

#include "../platform/common.h"
#include "../iostreams/ratio.hpp"

#include "invoker.hpp"

_STDX_BEGIN

#ifdef _STDX_DISABLE_BECHMARKING

#define _BENCHMARK(_Text, _Target, ...) _Target(__VA_ARGS__)

template<
	typename _Clock,
	typename _Ratio,
	typename _Fn,
	typename... _Args
>
inline auto benchmark(std::ostream& stream, _Fn&& target, _Args&&... args)
-> typename std::result_of<_Fn(_Args...)>::type {
	return target(args...);
}

#else

#define _BENCHMARK(_Text, _Target, ...) stdx::benchmark(std::cout << _Text, _Target, __VA_ARGS__)

template<
	typename _Clock,
	typename _Ratio,
	typename _Fn,
	typename... _Args
>
auto benchmark(std::ostream& stream, _Fn&& target, _Args&&... args)
	-> typename std::result_of<_Fn(_Args...)>::type
{
	using namespace std;
	using std::chrono::duration_cast;

	typedef std::chrono::duration<double, _Ratio> duration_t;
	typedef typename result_of<_Fn(_Args...)>::type result_type;

	static invoker<result_type> _invoker;

	auto timestamp = _Clock::now();
	_invoker(target, forward<_Args>(args)...);
	duration_t diff = (_Clock::now() - timestamp);
	stream << diff << endl;
	return _invoker.get();
}

#endif

_STDX_END

