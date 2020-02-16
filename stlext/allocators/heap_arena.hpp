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
	typename _Mutex = void
>
class newdel_arena : 
	public basic_arena<_Mutex, void, true>
{
public:
	void* allocate(size_t __nbytes) {
		return ::operator new(__nbytes, std::nothrow);
	}

	void* reallocate(size_t __nbytes, void*) {
		return ::operator new(__nbytes, std::nothrow);
	}

	void* deallocate(void* __addr, size_t) {
		::operator delete(__addr, std::nothrow);
		return nullptr;
	}

	template<typename T>
	size_t max_size() const
	{	// estimate maximum array size
		return ((size_t)(-1) / sizeof(T));
	}

};



template<
	typename _Mutex = void
>
class malloc_arena :
	public basic_arena<_Mutex, void, true>
{
public:
	void* allocate(size_t __nbytes) {
		return ::malloc(__nbytes);
	}

	void* reallocate(size_t __nbytes, void* __hint) {
		return ::realloc(__hint, __nbytes);
	}

	void* deallocate(void* __addr, size_t) {
		::free(__addr);
		return nullptr;
	}

	template<typename T>
	size_t max_size() const
	{	// estimate maximum array size
		return ((size_t)(-1) / sizeof(T));
	}

};




template<
	size_t _Align,
	typename _Mutex = void
>
class aligned_arena :
	public basic_arena<_Mutex, void, true>
{
protected:
	void* allocate(size_t __nbytes) {
#ifdef STDX_CMPLR_MSVC
		return ::_aligned_malloc(__nbytes, _Align);
#else
		return ::memalign(__nbytes, _Align);
#endif
	}

	void* reallocate(size_t __nbytes, void* __hint) {
#ifdef STDX_CMPLR_MSVC
		return ::_aligned_realloc(__hint, __nbytes, _Align);
#else
		return ::realloc(__hint, __nbytes);
#endif
	}

	void* deallocate(void* __addr, size_t) {
#ifdef STDX_CMPLR_MSVC
		return ::_aligned_free(__addr, _Align);
#else
		return ::free(__addr);
#endif
		return nullptr;
	}

	template<typename T>
	size_t max_size() const
	{	// estimate maximum array size
		return ((size_t)(-1) / sizeof(T));
	}

};


_STDX_END
