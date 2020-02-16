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
#include <tuple>

#include "../platform/common.h"

#ifndef _HEAP_STORAGE_DEFAULT_SIZE
#define _HEAP_STORAGE_DEFAULT_SIZE (1 << 16)
#endif

_STDX_BEGIN

template<typename _Impl>
class memory_storage_base
{
	__disable_copy(memory_storage_base)
public:
	typedef memory_storage_base<_Impl> base_type;

	memory_storage_base() {}
	~memory_storage_base() {}

	inline uint8_t* begin() const { return cast()->block_begin(); }
	inline uint8_t* end() const { return cast()->block_end(); }
	inline size_t size() const { return cast()->block_size(); }

	template<typename T>
	inline size_t max_size() const
	{	// estimate maximum array size
		return (size() / sizeof(T));
	}

	inline bool contains(const void* __addr) const {
		return ((__addr >= begin()) && __addr < end());
	}


private:
	inline _Impl* cast() { return static_cast<_Impl*>(this); }
	inline const _Impl* cast() const { return static_cast<const _Impl*>(this); }
};





// Compile-Time Size(CTS) Storage 

template<
	size_t _Size = 0, 
	size_t _Align = std::alignment_of<std::max_align_t>::value
>
class storage :
	public memory_storage_base< storage<_Size> >
{
	friend class base_type;

public:
	static const size_t SIZE = _Size;
	static const size_t ALIGN = _Align;

private:
	uint8_t* block_begin() const { return const_cast<uint8_t*>(__m_block); }
	uint8_t* block_end() const { return const_cast<uint8_t*>(__m_block + SIZE); }
	size_t block_size() const { return SIZE; }

	uint8_t __m_block[SIZE];
};

template<size_t _Size>
using stack_storage = storage < _Size > ;



// Run-Time Size(RTS) Storage 

template<size_t _Align>
class storage<0, _Align> :
	public memory_storage_base< storage<0> >
{
	friend class base_type;

public:
	static const size_t SIZE = 0;
	static const size_t ALIGN = _Align;

	storage(size_t size = _HEAP_STORAGE_DEFAULT_SIZE) : __m_size(size) {
#ifdef STDX_CMPLR_MSVC
		_block = (uint8_t*)::_aligned_malloc(__m_size, ALIGN);
#else
		_block = (uint8_t*)::memalign(ALIGN, __m_size);
#endif
	}

	~storage() {
#ifdef STDX_CMPLR_MSVC
		::_aligned_free(__m_block);
#endif
		::free(__m_block);
#endif
	}

private:
	uint8_t* block_begin() const { return __m_block; }
	uint8_t* block_end() const { return (__m_block + __m_size); }
	size_t block_size() const { return __m_size; }

private:
	uint8_t* __m_block;
	size_t __m_size;
};
typedef storage<0> heap_storage;




class temporary_storage :
	public memory_storage_base<temporary_storage>
{
	friend class base_type;

public:
	temporary_storage(size_t size)  {
		std::tie(__m_block, __m_size) = std::get_temporary_buffer<uint8_t>(size);
	}

	~temporary_storage() {
		std::return_temporary_buffer(__m_block);
	}

private:
	uint8_t* block_begin() const { return __m_block; }
	uint8_t* block_end() const { return (__m_block + __m_size); }
	size_t block_size() const { return __m_size; }

private:
	uint8_t* __m_block;
	size_t __m_size;
};


_STDX_END


