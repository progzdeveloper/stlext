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
#include <cstdint>
#include <limits>
#include "basic_arena.hpp"
#include "arena_traits.hpp"
#include "freelist.hpp"

#include "../platform/bits.h"

_STDX_BEGIN

template<
	typename _Arena, 
	size_t _MaxSize = 0, 
	typename _Mutex = void,
	typename _Destroyer = empty_delete<>
>
class binexp_arena :
	public basic_arena<_Mutex, _Destroyer, false>
{
	__disable_copy(binexp_arena)
	typedef arena_traits<_Arena> traits_type;
	typedef typename traits_type::member_type arena_t;

	static const size_t BITS_PER_SIZE_T = (sizeof(size_t) * CHAR_BIT);
public:

	static const size_t BINCOUNT = (sizeof(void*) * CHAR_BIT - 1);
	static const size_t MAX_FREELIST_SIZE = _MaxSize;

	typedef freelist<_MaxSize> freelist_type;

	binexp_arena() {
		__init_from(arena_traits<_Arena>::global(), __m_target);
	}

	binexp_arena(const _Arena& __a) : 
		__init_from(__a, __m_target) {
	}

	~binexp_arena() {
		__dispose();
	}

	void* allocate(size_t __nbytes)
	{
		if (__nbytes == 0)
			return nullptr;

		//size_t idx = nlz(__nbytes - 1);
		size_t idx = BITS_PER_SIZE_T - stdx::__clz(__nbytes);
		//size_t s = (1ULL << idx);
		if (idx >= BINCOUNT || __nbytes < sizeof(void*))
			return traits_type::allocate(__m_target, __nbytes);

		void* addr = __m_bins[idx].pop();
		if (addr != nullptr)
			return addr;

		//return traits_type::allocate(__m_target, s < sizeof(void*) ? sizeof(void*) : s);
		return traits_type::allocate(__m_target, (1ULL << idx));
	}

	void* reallocate(size_t __nbytes, void*) {
		return allocate(__nbytes);
	}


	void* deallocate(void* __addr, size_t __nbytes)
	{
		if (__nbytes == 0)
			return nullptr;

		//size_t idx = nlz(__nbytes - 1);
		size_t idx = BITS_PER_SIZE_T - stdx::__clz(__nbytes);
		if (idx >= BINCOUNT || __nbytes < sizeof(void*)) {
			return traits_type::deallocate(__m_target, __addr, __nbytes);
		}

		if (!__m_bins[idx].push(__addr))
			return traits_type::deallocate(__m_target, __addr, (1ULL << idx));

		return nullptr;
	}

	template<typename T>
	inline size_t max_size() const
	{	// estimate maximum array size
		return traits_type::template max_size<T>(__m_target);
	}

private:
	void __dispose()
	{
		void *addr = nullptr;
		uint64_t s = 1;
		for (size_t i = 0; i < BINCOUNT; i++, s <<= 1) { 
			// destroy all lists
			while ((addr = __m_bins[i].pop()) != nullptr) {
				traits_type::deallocate(__m_target, addr, s);
			}
		}
	}

private:
	arena_t __m_target;
	freelist_type __m_bins[BINCOUNT];
};


_STDX_END


