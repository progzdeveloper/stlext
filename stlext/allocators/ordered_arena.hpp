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
#include "basic_arena.hpp"

_STDX_BEGIN

template<
	typename _Storage,
	typename _Mutex = void,
	typename _Destroyer = empty_delete<>
>
class ordered_arena :
	public basic_arena<_Mutex, _Destroyer, false>
{
	__disable_copy(ordered_arena)

public:
	typedef _Mutex mutex_type;

	ordered_arena() {
		__m_free_block = __m_cache.begin();
	}

	ordered_arena(size_t msize) : __m_cache(msize) {
		__m_free_block = __m_cache.begin();
	}

	~ordered_arena() {
	}

	void* allocate(size_t __nbytes)
	{
		if (__nbytes > size_t(__m_cache.end() - __m_free_block))
			return nullptr;

		uint8_t* ptr = __m_free_block;
		__m_free_block += __nbytes;
		return ptr;
	}

	void* reallocate(size_t __nbytes, void*) {
		return allocate(__nbytes);
	}

	void* deallocate(void* __addr, size_t __nbytes)
	{
		if (__m_cache.contains(__addr)) {
			__m_free_block -= __nbytes;
			return nullptr;
		}
		return __addr;
	}

	template<typename T>
	size_t max_size() const
	{	// estimate maximum array size
		return __m_cache.template max_size<T>();
	}

private:
	_Storage __m_cache;
	uint8_t* __m_free_block;
};


_STDX_END

