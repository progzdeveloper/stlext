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
#include "intrusive_ptr.hpp"
#include "../platform/common.h"

_STDX_BEGIN

class arena_base
{
protected:
	template<typename _Arena>
	static inline void __init_from(const _Arena& __a, _Arena& __target) {
		__target = __a;
	}

	template<typename _Arena>
	static inline void __init_from(const _Arena& __a, intrusive_ptr<_Arena>& __target) {
		__target = const_cast<_Arena*>(&__a);
	}

};


template<
	typename _Mutex = void, 
	typename _Destroyer = empty_delete<>,
	bool _Stateless = false
>
class basic_arena : 
	public arena_base,
	public intrusive_ref_counter<
		basic_arena<_Mutex, _Destroyer, _Stateless>,
		typename std::conditional<
			std::is_same<void, _Mutex>::value, 
			thread_unsafe_counter, 
			thread_safe_counter
		>::type,
		_Destroyer
	>
{
public:
	typedef basic_arena<_Mutex, _Destroyer, _Stateless> base_type;
	typedef _Mutex mutex_type;
	typedef _Destroyer deleter_type;
	static const bool stateless = _Stateless;

};



template<
	typename _Mutex, 
	typename _Destroyer
>
class basic_arena<_Mutex, _Destroyer, true> : public arena_base
{
public:
	typedef basic_arena<_Mutex, _Destroyer, true> base_type;
	typedef _Mutex mutex_type;
	typedef _Destroyer deleter_type;
	static const bool stateless = true;

};



template<typename _Mutex>
class basic_arena<_Mutex, void, true> : public arena_base
{
public:
	typedef basic_arena<_Mutex, void, true> base_type;
	typedef _Mutex mutex_type;
	typedef void deleter_type;
	static const bool stateless = true;
};

_STDX_END

