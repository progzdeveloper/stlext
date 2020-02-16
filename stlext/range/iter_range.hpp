// Copyright (c) 2013-2016, Michael Polukarov (Russia).
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
#include <cassert>
#include <utility>
#include <iterator>
#include <algorithm>

#include "../platform/common.h"

_STDX_BEGIN

template<typename _Iter>
struct iter_range 
{
	typedef iter_range<_Iter> this_type;
	typedef _Iter iterator_type;
	typedef typename std::iterator_traits<_Iter>::iterator_category iterator_category; 
	typedef typename std::iterator_traits<_Iter>::distance_type     distance_type; 
	typedef typename std::iterator_traits<_Iter>::difference_type   difference_type; 
	typedef typename std::iterator_traits<_Iter>::value_type        value_type;
	typedef typename std::iterator_traits<_Iter>::reference         reference;

	/// ctors

	// ctor
	iter_range(const _Iter& f, const _Iter& l) : 
		first(f), last(l) {
	}

	// ctor
	template<typename _OtherIter>
	iter_range(const std::pair<_OtherIter, _OtherIter>& iterp) : 
		first(iterp.first), last(iterp.second) {
	}

	// copy ctor
	template<typename _OtherIter>
	iter_range(const iter_range<_OtherIter>& other) : 
		first(other.first), last(other.last) {
	}


	/// methods

	// range size
	inline distance_type size() const {
		return std::distance(first, last);
	}

	inline bool empty() const {
		return (first == last);
	}

	// extract section from range producing new range
	inline this_type section(size_t from, size_t to) const
	{
		__stdx_assert((distance_type)from < size(), std::invalid_argument, "invalid _From value");
		__stdx_assert((distance_type)to < size(), std::invalid_argument, "invalid _To value");
		__stdx_assert(from < to, std::invalid_argument, "invalid range");
		return this_type(first + from, first + to);
	}

	// extract section from range producing new range (syntax sugar)
	inline this_type operator()(size_t from, size_t to) const {
		return section(from, to);
	}


	// exchange contents with _Right
	void swap(this_type& other)
	{	
		if (this != std::addressof(other)) { // different, worth swapping
			std::swap(first, other.first);
			std::swap(last, other.last);
		}
	}

	/// operator overloading

	// assign operator
	template<typename _OtherIter>
	this_type& operator= (const iter_range<_OtherIter>& other)
	{
		if (this != std::addressof(other)) {
			first = other.first;
			last = other.last;
		}
		return (*this);
	}


	// compare operators
	template<typename _OtherIter>
	inline bool operator== (const iter_range<_OtherIter>& other) {
		return ((first == other.first) && (last == other.last));
	}

	template<typename _OtherIter>
	inline bool operator!= (const iter_range<_OtherIter>& other) {
		return (!(other == (*this)));
	}

	// subscript operators
	inline reference operator[] ( difference_type i ) {
		return first[i];
	}

	inline reference operator[] ( difference_type i ) const {
		return first[i];
	}

	/// members
	_Iter first, last;
};



// producing functions

template<typename _It>
inline iter_range< _It > 
	make_range(const _It& first, const _It& last) 
{
	return{ first, last };
}

template<typename _Container>
inline auto make_range(_Container& __c) ->
	iter_range< decltype(std::begin(c)) >
{
	return make_range(std::begin(c), std::end(c));
}

template<typename _Container>
inline auto make_range(const _Container& __c) -> 
	iter_range< decltype(std::begin(c)) >
{
	return make_range(std::begin(c), std::end(c));
}

template<typename _Type, size_t _Size>
inline auto make_range(_Type(&arr)[_Size]) ->
	iter_range< decltype(std::begin(arr)) >
{
	return make_range(std::begin(arr), std::end(arr));
}


template<typename _Type, size_t _Size>
inline auto make_range(const _Type(&arr)[_Size]) ->
	iter_range< decltype(std::begin(arr)) >
{
	return make_range(std::begin(arr), std::end(arr));
}

#if 0 
// this has no need anymore, since we have 
// begin()/end() for streams implemented
// in extensions module
template<typename T>
inline iter_range< std::istream_iterator<T> >
	make_range(std::istream& _Stream)
{
	typedef std::istream_iterator<T> iterator;
	typedef iter_range< iterator  > _Range;
	return (_Range(iterator(_Stream), iterator()));
}
#endif

_STDX_END



// STL embedding: iterator begin()/end() adaptors
namespace std
{

	template<typename _Iter>
	inline _Iter begin(stdx::iter_range<_Iter>& range) {
		return range.first;
	}

	template<typename _Iter>
	inline _Iter begin(const stdx::iter_range<_Iter>& range) {
		return range.first;
	}

	template<typename _Iter>
	inline _Iter end(stdx::iter_range<_Iter>& range) {
		return range.last;
	}

	template<typename _Iter>
	inline _Iter end(const stdx::iter_range<_Iter>& range) {
		return range.last;
	}

	template<class _It>
	inline typename ::stdx::iter_range<_It>::distance_type 
		distance(const ::stdx::iter_range<_It>& range) {
			return range.size();
	}

}





