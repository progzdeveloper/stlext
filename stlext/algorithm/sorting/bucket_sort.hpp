// Copyright (c) 2018, Michael Polukarov (Russia).
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
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include <iterator>

#include "../../platform/common.h"



_STDX_BEGIN

template<class T>
class bucketizer;


template<>
struct bucketizer<int>
{
	template<class _It>
	bucketizer(_It first, _It last) {
		n = std::distance(first, last);
		auto minmax = std::minmax_element(first, last);
		minimum = *minmax.first;
		maximum = *minmax.second;
	}

	inline size_t operator()(int x) const {
		return ((x - minimum) * n) / (maximum - minimum);
	}

	size_t size() const { return n + 1; }

	size_t n;
	int minimum;
	int maximum;
};


template<>
struct bucketizer<char>
{
	template<class _It>
	bucketizer(_It first, _It last) {
	}

	inline size_t operator()(char x) const {
		return x;
	}

	size_t size() const {
		return (1 << (sizeof(char)*CHAR_BIT));
	}
};


template<>
struct bucketizer<wchar_t>
{
	template<class _It>
	bucketizer(_It first, _It last) {
	}

	inline size_t operator()(wchar_t x) const {
		return x;
	}

	size_t size() const {
        return (1ULL << (sizeof(wchar_t)*CHAR_BIT));
	}
};


template<class E, class T, class A>
struct bucketizer< std::basic_string<E, T, A> >
{
	typedef std::basic_string<E, T, A> string_type;

	template<class _It>
	bucketizer(_It first, _It last) {
	}

	inline size_t operator()(const string_type& x) const {
		return (x.empty() ? 0 : x[0]);
	}

	size_t size() const {
		return (1 << (sizeof(E)*CHAR_BIT));
	}
};




template <class _It, class _Bucketizer, class _Alloc>
void bucket_sort(_It first, _It last, _Bucketizer b, const _Alloc& al)
{
	using namespace std;
	typedef typename iterator_traits<_It>::value_type value_type;
    typedef priority_queue<value_type, std::vector<value_type>, std::greater<> > bucket_map;
	typedef typename _Alloc::template rebind<bucket_map>::other allocator_type;

	if (first == last)
		return; // nothing to sort

	size_t nbins = b.size();
	bucket_map* bins = nullptr;

	allocator_type alloc(al);
	try {
		bins = alloc.allocate(nbins);
		for (bucket_map* bucket = bins, *end = bins + nbins; bucket != end; ++bucket) {
			alloc.construct(bucket);
		}
	}
	catch (...) {
		throw;
	}

	bucket_map* bucket;
	for (_It it = first; it != last; ++it) {
		bucket = bins + b(*it);
		bucket->emplace(*it);
	}

	for (bucket_map* bucket = bins, *end = bins + nbins; bucket != end; ++bucket) {
		for (; !bucket->empty(); bucket->pop(), ++first)
			*first = bucket->top();
		alloc.destroy(bucket);
	}
	alloc.deallocate(bins, nbins);
}



template <class _It, class _Bucketizer>
void bucket_sort(_It first, _It last, _Bucketizer b)
{
	using namespace std;
	typedef typename iterator_traits<_It>::value_type value_type;
	if (first == last)
		return; // nothing to sort
	bucket_sort(first, last, b, std::allocator<value_type>());
}

template <class _It>
void bucket_sort(_It first, _It last)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	if (first == last)
		return; // nothing to sort
	bucket_sort(first, last, bucketizer<value_type>(first, last));
}

_STDX_END

