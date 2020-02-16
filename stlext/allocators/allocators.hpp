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
#include "memory_storage.hpp"
#include "basic_allocator.hpp"
#include "heap_arena.hpp"
#include "ordered_arena.hpp"
#include "fallback_arena.hpp"
#include "binexp_arena.hpp"
#include "bitmap_arena.hpp"

_STDX_BEGIN

/*! Stateless new/delete allocator */
template<
	typename T,
	typename _Mutex = void
>
using newdel_allocator = basic_allocator < T, newdel_arena< _Mutex > > ;

/*! Stateless malloc/free allocator */
template<
	typename T,
	typename _Mutex = void
>
using malloc_allocator = basic_allocator < T, malloc_arena < _Mutex > > ;

/*! Stateless malloc/free aligned allocator */
template<
	typename T, 
	size_t _Align = std::alignment_of<T>::value, 
	typename _Mutex = void
>
using aligned_allocator = basic_allocator < T, aligned_arena< _Align, _Mutex > > ;


/*! Statefull tack-ordered allocator with bounded size storage */
template<
	typename T,
	size_t _MaxSize,
	typename _Mutex = void,
	typename _Deleter = empty_delete<>
>
using ordered_allocator = basic_allocator < T, ordered_arena< storage<sizeof(T)*_MaxSize>, _Mutex, _Deleter > >;


/*! Statefull bitmap allocator */
template<
	typename T,
	size_t _MaxBlocks,
	typename _Mutex = void,
	typename _Arena = newdel_arena<_Mutex>,
	typename _Deleter = empty_delete<>
>
using bitmap_allocator = basic_allocator < T, bitmap_arena < _Arena, sizeof(T), _MaxBlocks, _Mutex, _Deleter> > ;

/*! Fallback allocator defaulted with bitmap arena with fixed stack-allocated storage and heap-based fallback */
template<
	typename T,
	size_t _MaxSize,
	typename _Mutex = void,
	typename _Arena = bitmap_arena< 
		ordered_arena < storage<sizeof(T)*_MaxSize>, _Mutex >, 
		sizeof(T),
		_MaxSize,
		_Mutex 
	>,
	typename _Fallback = newdel_arena<_Mutex>
>
using fallback_allocator = basic_allocator < T, fallback_arena< _Arena, _Fallback, _Mutex > > ;


/* SLAB Allocator*/
template<
	typename T,
	size_t _Capacity = 0,
	typename _Mutex = void,
	typename _Arena = newdel_arena<_Mutex>,
	typename _Deleter = empty_delete<>
>
using slab_allocator = basic_allocator < T, binexp_arena<_Arena, _Capacity, _Mutex, _Deleter> >;



_STDX_END

