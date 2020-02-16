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

template<typename _Arena>
struct arena_traits
{
	typedef typename _Arena arena_type;
	typedef typename _Arena::base_type base_type;
	typedef typename _Arena::mutex_type mutex_type;
	typedef typename _Arena::deleter_type deleter_type;

	typedef typename std::conditional<
		std::is_empty<_Arena>::value,
		_Arena,
		intrusive_ptr<_Arena>
	>::type member_type;

	static const bool stateless = _Arena::stateless;

	static inline _Arena& global() {
		static _Arena __instance;
		return __instance;
	}

	static inline void* allocate(_Arena& __a, size_t __nbytes) {
		return __a.allocate(__nbytes);
	}

	static inline void* reallocate(_Arena& __a, size_t __nbytes, void* __hint) {
		return __a.reallocate(__nbytes, __hint);
	}

	static inline void* deallocate(_Arena& __a, void* __addr, size_t __nbytes) {
		return __a.deallocate(__addr, __nbytes);
	}

	template<typename T>
	static inline size_t max_size(const _Arena& __a) {
		return __a.template max_size<T>();
	}



	static inline void* allocate(intrusive_ptr<_Arena>& __a, size_t __nbytes) {
		return __a->allocate(__nbytes);
	}

	static inline void* reallocate(intrusive_ptr<_Arena>& __a, size_t __nbytes, void* __hint) {
		return __a->reallocate(a, __nbytes, __hint);
	}

	static inline void* deallocate(intrusive_ptr<_Arena>& __a, void* __addr, size_t __nbytes) {
		return __a->deallocate(__addr, __nbytes);
	}

	template<typename T>
	static inline size_t max_size(const intrusive_ptr<_Arena>& __a) {
		return __a->template max_size<T>();
	}

};

_STDX_END