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
#include <algorithm>

#include "basic_arena.hpp"
#include "arena_traits.hpp"

_STDX_BEGIN

template<
	typename _Arena,
	typename _Fallback,
	typename _Mutex = void,
	typename _Destroyer = empty_delete<>
>
class fallback_arena :
	public basic_arena<_Mutex, _Destroyer, false>
{
	__disable_copy(fallback_arena)

public:
	fallback_arena() {
		__init_from(arena_traits<_Arena>::global(), __m_target);
		__init_from(arena_traits<_Fallback>::global(), __m_fallback);
	}

	fallback_arena(const _Arena& __target, const _Fallback& __fallback) {
		__init_from(__target, __m_target);
		__init_from(__fallback, __m_fallback);
	}


	inline void* allocate(size_t __nbytes) {
		void* addr = arena_traits<_Arena>::allocate(__m_target, __nbytes);
		if (addr == nullptr)
			return arena_traits<_Fallback>::allocate(__m_fallback, __nbytes);
		return addr;
	}

	inline void* reallocate(size_t __nbytes, void* __hint) {
		void* addr = arena_traits<_Arena>::reallocate(__m_target, __nbytes, __hint);
		if (addr == nullptr)
			return arena_traits<_Fallback>::reallocate(__m_fallback, __nbytes, __hint);
		return addr;

	}

	inline void* deallocate(void* __addr, size_t __nbytes) {
		void* res = arena_traits<_Arena>::deallocate(__m_target, __addr, __nbytes);
		if (res != nullptr)
			return arena_traits<_Fallback>::deallocate(__m_fallback, __addr, __nbytes);
		return res;

	}

	template<typename T>
	inline size_t max_size() const {
		return (std::max)(arena_traits<_Arena>::template max_size<T>(__m_target), 
						  arena_traits<_Fallback>::template max_size<T>(__m_fallback));
	}

private:
	typename arena_traits<_Arena>::member_type __m_target;
	typename arena_traits<_Fallback>::member_type __m_fallback;
};


_STDX_END
