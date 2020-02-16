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
#include <memory>
#include "intrusive_ptr.hpp"
#include "arena_traits.hpp"

_STDX_BEGIN


template<
	typename _Type,
	typename _Arena, 
	typename _Mutex = void
>
class alloc_base 
{

public:
	typedef _Arena arena_type;
	typedef arena_traits<_Arena> traits_type;
	typedef alloc_base<_Type, _Arena, _Mutex> base_type;

	alloc_base() {
		__init_from(arena_traits<_Arena>::global(), arena);
	}

	alloc_base(const arena_type& __arena) {
		__init_from(__arena, arena);
	}

	alloc_base(const typename traits_type::member_type& __other) :
		arena(__other) {
	}

	inline size_t max_size() const
	{	// estimate maximum array size
		//return (arena ? arena->template max_size<_Type>() : 0);
		return traits_type::template max_size<_Type>(arena);
	}

	// ### FIX ME: find a way to make arena protected member
	typename traits_type::member_type arena;

protected:
	static inline void __init_from(const _Arena& a, _Arena& target) {
		target = a;
	}

	static inline void __init_from(const _Arena& a, intrusive_ptr<_Arena>& target) {
		target = const_cast<_Arena*>(&a);
	}

	inline void* do_alloc(size_t __nbytes) 
	{ 
		/*if (!arena)
			throw std::bad_alloc();*/

		if (__nbytes == 0)
			throw std::bad_alloc(); // "unable to allocate zero-size array"

		std::lock_guard<_Mutex> locker(__s_mtx);
		void* addr = traits_type::allocate(arena, __nbytes);
		if (addr == nullptr)
			throw std::bad_alloc();

		return addr;
	}

	inline void do_dealloc(void* __addr, size_t __nbytes) 
	{ 
		std::lock_guard<_Mutex> locker(__s_mtx);
		traits_type::deallocate(arena, __addr, __nbytes);
	}
	
	static _Mutex __s_mtx;
};


template<
	typename _Type,
	typename _Arena,
	typename _Mutex
>
_Mutex alloc_base<_Type, _Arena, _Mutex>::__s_mtx;






template<
	typename _Type,
	typename _Arena
>
class alloc_base<_Type, _Arena, void>
{
public:
	typedef _Arena arena_type;
	typedef arena_traits<_Arena> traits_type;
	typedef alloc_base<_Type, _Arena, void> base_type;

	alloc_base() {
		__init_from(arena_traits<_Arena>::global(), arena);
	}

	alloc_base(const arena_type& __arena) {
		__init_from(__arena, arena);
	}

	alloc_base(const typename traits_type::member_type& __other) :
		arena(__other) {
	}

	inline size_t max_size() const
	{	// estimate maximum array size
		//return (arena ? arena->template max_size<_Type>() : 0);
		return traits_type::template max_size<_Type>(arena);
	}

	// ### FIX ME: find a way to make arena protected member
	typename traits_type::member_type arena;

protected:
	static inline void __init_from(const _Arena& a, _Arena& target) {
		target = a;
	}

	static inline void __init_from(const _Arena& a, intrusive_ptr<_Arena>& target) {
		target = const_cast<_Arena*>(&a);
	}

	inline void* do_alloc(size_t __nbytes)
	{
		/*if (!arena)
		throw std::bad_alloc();*/

		if (__nbytes == 0)
			throw std::bad_alloc(); // "unable to allocate zero-size array"

		void* addr = traits_type::allocate(arena, __nbytes);
		if (addr == nullptr)
			throw std::bad_alloc();

		return addr;
	}

	inline void do_dealloc(void* __addr, size_t __nbytes)
	{
		traits_type::deallocate(arena, __addr, __nbytes);
	}
};






template<
	typename _Type,
	typename _Arena
>
class basic_allocator : 
	public alloc_base<
		_Type, 
		_Arena,
		typename _Arena::mutex_type
	>
{

public:
	typedef _Arena arena_type;

	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;

	typedef _Type value_type;

	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef value_type* pointer;
	typedef const value_type* const_pointer;

	typedef void* void_pointer;
	typedef const void* const_void_pointer;

	typedef std::false_type propagate_on_container_copy_assignment;
	typedef std::false_type propagate_on_container_move_assignment;
	typedef std::false_type propagate_on_container_swap;


	basic_allocator<_Type, _Arena> select_on_container_copy_construction() const {
		// return this allocator
		return (*this);
	}

	template<class _Other>
	struct rebind
	{	// convert this type to allocator<_Other>
		typedef basic_allocator<_Other, _Arena> other;
	};


	basic_allocator() :
		base_type() {
	}

	/*template<typename... _Args>
	basic_allocator(std::piecewise_construct_t __tag, _Args&&... __args) :
		base_type(std::forward<_Args>(__args)...) {
	}*/

	basic_allocator(const arena_type& __arena) :
		base_type(__arena) {
	}

	basic_allocator(const basic_allocator<_Type, _Arena>& __other) throw() :
		base_type(__other.arena) {
		// construct by copying
	}

	template<typename _Other>
	basic_allocator(const basic_allocator<_Other, _Arena>& __other) throw() :
		base_type(__other.arena) {
		// construct from a related allocator
	}

	~basic_allocator() {
	}

	// assign from a related allocator
	template<class _Other>
	basic_allocator& operator=(const basic_allocator<_Other, _Arena>& __other)
	{	
		if (__other != (*this)) // escape arena self assignment
			arena = __other.arena; // construct by copying
		return (*this);
	}

	inline pointer allocate(size_type __count, const_void_pointer __hint = const_void_pointer()) {
		return static_cast<pointer>(do_alloc(__count * sizeof(value_type)));
	}

	inline void deallocate(pointer __addr, size_type __count) {
		do_dealloc(__addr, __count * sizeof(value_type));
	}

	pointer address(reference __x) const
	{	// return address of mutable __x
		return (std::addressof(__x));
	}

	const_pointer address(const_reference __x) const
	{	// return address of nonmutable __x
		return (std::addressof(__x));
	}

	void construct(pointer __addr)
	{	// default construct object at __addr
		::new ((void *)__addr) value_type();
	}

	void construct(pointer __addr, const_reference __x)
	{	// construct object at __addr with value __x
		::new ((void *)__addr) value_type(__x);
	}

	template<class _Obj, class... _Args>
	void construct(_Obj *__addr, _Args&&... __args)
	{	// construct _Obj(_Args...) at __addr
		::new ((void *)__addr) _Obj(std::forward<_Args>(__args)...);
	}


	template<class _Uty>
	void destroy(_Uty *__addr)
	{	// destroy object at __addr
		__addr->~_Uty();
	}

};


template<typename _Arena>
class basic_allocator<void, _Arena> :
	public alloc_base<
		void, 
		_Arena, 
		typename _Arena::mutex_type
	>
{	// generic allocator for type void
public:
	typedef _Arena arena_type;

	//typedef basic_allocator<void, _Arena> other;

	typedef void value_type;

	typedef void *pointer;
	typedef const void *const_pointer;
	typedef void *void_pointer;
	typedef const void *const_void_pointer;

	template<class _Other>
	struct rebind
	{	// convert this type to an allocator<_Other>
		typedef basic_allocator<_Other, _Arena> other;
	};

	basic_allocator<void, _Arena> select_on_container_copy_construction() const {
		// return this allocator
		return (*this);
	}

	basic_allocator() throw()
	{	// construct default allocator (do nothing)
	}

	basic_allocator(arena_type& __arena) : base_type(__arena) {
	}

	basic_allocator(const basic_allocator<void, _Arena>&) throw()
	{	// construct by copying (do nothing)
	}

	template<class _Other>
	basic_allocator(const basic_allocator<_Other, _Arena>&) throw()
	{	// construct from related allocator (do nothing)
	}

	template<class _Other>
	basic_allocator<void, _Arena>& operator=(const basic_allocator<_Other, _Arena>&)
	{	// assign from a related allocator (do nothing)
		return (*this);
	}

	~basic_allocator() {
	}
};


template<class _Tx, class _Ax,
class _Ty, class _Ay> inline
	bool operator==(const basic_allocator<_Tx, _Ax>& __left,
					const basic_allocator<_Ty, _Ay>& __right) throw()
{	// test for allocator inequality
	return (std::addressof(__left.arena) == std::addressof(__right.arena));
}

template<class _Tx, class _Ax,
class _Ty, class _Ay> inline
	bool operator!=(const basic_allocator<_Tx, _Ax>& __left,
					const basic_allocator<_Ty, _Ay>& __right) throw()
{	// test for allocator inequality
	return (!(__left == __right));
}


_STDX_END