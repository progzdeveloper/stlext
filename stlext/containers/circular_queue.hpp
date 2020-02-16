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
#include <deque>
#include <iterator>

#include "../platform/common.h"

#include <queue>

_STDX_BEGIN

/*!
 * \brief circular_queue circular overwritable queue
 * 
 * circular_queue class implements overwritable queue of 
 * elements. The maximum number of elements that circular_queue
 * instance can hold can be setup in the constructor, or using 
 * resize() method.
 * 
 * When insert take place if queue is full then front element 
 * (i.e. last recently inserted) will be evicted to free space 
 * for newly insertable element
 */
template<
	typename _ValueType,
	typename _Container = std::deque<_ValueType>
>
class circular_queue
{
public:
	typedef _Container container_type;
	typedef typename _Container::value_type value_type;
	typedef typename _Container::size_type size_type;

	typedef typename _Container::reference reference;
	typedef typename _Container::const_reference const_reference;
	typedef typename _Container::pointer pointer;
	typedef typename _Container::const_pointer const_pointer;

	typedef typename _Container::iterator iterator;
	typedef typename _Container::const_iterator const_iterator;
	typedef typename _Container::reverse_iterator reverse_iterator;
	typedef typename _Container::const_reverse_iterator const_reverse_iterator;

	typedef typename _Container::allocator_type allocator_type;

	// construct by copying specified container
	explicit circular_queue(const _Container& c)
		: __m_buffer(c), __m_max_size(c.size()) {
	}

	// construct with specified size
	circular_queue(size_type size) :
		__m_max_size(size) {
	}

	// push element into back of queue
	// side-effect: front element is evicted on overflow
	void push(const_reference val) {
		if (full()) // evict front element on overflow
			pop();
		__m_buffer.push_back(val);
	}

	// emplace element into back of queue
	template<typename... _Args>
	void emplace(_Args&&... args) {
		if (full()) // evict last element on overflow
			pop(); 
		__m_buffer.emplace(std::forward<_Args>(args)...);
	}

	// pop front element from queue
	void pop()                     { __m_buffer.pop_front(); }

	// return first element of mutable queue
	reference front()              { return (__m_buffer.front()); }

	// return first element of nonmutable queue
	const_reference front() const  { return (__m_buffer.front()); }

	// return last element of mutable queue
	reference back()               { return (__m_buffer.back());}

	// return last element of nonmutable queue
	const_reference back() const   { return (__m_buffer.back()); }

	// return iterator pointing to first element of queue
	iterator begin()               { return (__m_buffer.begin()); }
	const_iterator begin() const   { return (__m_buffer.begin()); }
	const_iterator cbegin() const  { return (__m_buffer.cbegin()); }

	// return iterator pointing to one past last element of queue
	iterator end()                 { return (__m_buffer.end()); }
	const_iterator end() const     { return (__m_buffer.end()); }
	const_iterator cend() const    { return (__m_buffer.cend()); }

	reverse_iterator rbegin()             { return (__m_buffer.rbegin()); }
	const_reverse_iterator rbegin() const { return (__m_buffer.rbegin()); }

	reverse_iterator rend()               { return (__m_buffer.rend()); }
	const_reverse_iterator rend() const   { return (__m_buffer.rend()); }

	// remove all elements from queue
	void clear()        { __m_buffer.clear(); }

	// test if queue is empty
	bool empty() const  { return (__m_buffer.empty()); }
	// test if queue is full
	bool full() const   { return (__m_buffer.size() >= __m_max_size); }
	
	// return current capacity 
	size_type capacity() const { return __m_max_size; }
	// return current capacity (alias for max_size())
	size_type max_size() const { return __m_max_size; }
	// return current size
	size_type size() const     { return (__m_buffer.size()); }
	
	// resize internal capacity
	/*void resize(size_type n) {
		__m_max_size = n;
		__m_buffer.resize(n);
	}*/

	// return currently used memory allocator
	allocator_type get_allocator() const { return (__m_buffer.get_allocator()); }

private:
	container_type __m_buffer;
	size_type __m_max_size;
};


_STDX_END

