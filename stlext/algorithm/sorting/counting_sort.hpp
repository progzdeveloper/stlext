// Copyright (c) 2018, Vladislav Liseev, Michael Polukarov (Russia).
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
#include <iterator>
#include <numeric>
#include <type_traits>


#include "../../functional/function_traits.hpp"
#include "../../functional/identity.hpp"

_STDX_BEGIN

#ifndef _MAX_STACK_SIZE
#    define _MAX_STACK_SIZE 65536
#endif

/*!
 * \fn counting_sort(_RanIt first, _RanIt last, _Proj proj, const _Alloc& al)
 * \brief Perform a counting sort at range [first, last).
 *
 * \tparam _RanIt models random access iterator
 * \tparam _Proj  models projection unary function
 * \tparam _Alloc models memory allocator
 * 
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param proj  projection function object (i.e. an object that project
 * an input value into another value such that a new value is a subset of original).
 * \param al    Allocator used to allocate memory for internal needs
 * 
 */
template <class _RanIt, class _Proj, class _Alloc>
void counting_sort(_RanIt first, _RanIt last, _Proj proj, const _Alloc& al)
{
    using namespace std;
    typedef std::ptrdiff_t key_type; // key type
    typedef typename iterator_traits<_RanIt>::value_type value_type; // iterator value type
    //typedef typename std::result_of<_Proj(value_type)>::type result_type;
    typedef typename stdx::function_traits<_Proj>::result_type result_type;
    typedef typename std::decay<result_type>::type projected_type;
    
    typedef typename _Alloc::template rebind<key_type>::other key_allocator; // key allocator type

    static_assert(std::is_integral<projected_type>::value, "projected type is not an integral type");

    if (first == last) 
        return;  // nothing to sort

    // indirect comparision lambda
    auto pred = [&proj](const value_type& x, const value_type& y) {
        return proj(x) < proj(y);
    };

    // find min & max elements
    auto minmax = minmax_element(first, last, pred); 

    auto minimum = proj(*minmax.first);   // minimal value
    auto maximum = proj(*minmax.second);  // maximal value
    size_t n = distance(first, last);     // number of elements in [first, last) range

    // number of buckets in histogram +1 (partial sum starting from 0)
    size_t k = (maximum - minimum) + 2;

    if (k >= n) {                 // k is too large
        sort(first, last, pred);  // fallback to quick sort algorithm
        return;
    }

    key_allocator key_alloc(al);

    // get counter_buffer
    key_type* counter_buffer = nullptr;
    size_t kspace = k * sizeof(key_type); // space for counters
    if (kspace < _MAX_STACK_SIZE)  // use stack allocation 
        counter_buffer = static_cast<key_type*>(alloca(kspace));
    else
        counter_buffer = key_alloc.allocate(k);

    fill(counter_buffer, counter_buffer + k, key_type());

    // create histogram
    for (_RanIt it = first; it != last; ++it) {
        ++counter_buffer[proj(*it) - minimum + 1];
    }

    *counter_buffer = 0;

    // boundary calcualtion
    std::partial_sum(counter_buffer, counter_buffer + k, counter_buffer);

    // move elements
    for (ptrdiff_t i = n - 1; i >= 0; --i) 
    {
        _RanIt lIt = (first + i);
        auto val = proj(*lIt) - minimum;

        auto j = counter_buffer[val];
        _RanIt rIt = (first + j);

        if (j < i) {
            // process a fetch cycle. 
            // since the index `i` moves downward
            // and the counts move upward, it is 
            // guaranteed that a value is never moved twice
            do {
                ++counter_buffer[val];
                // swap
                std::iter_swap(lIt, rIt);
                val = proj(*lIt) - minimum;
                j = counter_buffer[val];
                rIt = first + j;
            } while (j < i);

            // move final value into place
            *lIt = *rIt;
        }
    }

    // release temporary buffers
    // if space was allocated using alloca() call
    // it will be automatically disposed after the
    // function returns
    if (kspace >= _MAX_STACK_SIZE)
        key_alloc.deallocate(counter_buffer, k);
}


template <class _FwdIt, class _Alloc>
void counting_sort(_FwdIt first, _FwdIt last,
				   stdx::identity<typename std::iterator_traits<_FwdIt>::value_type>,
				   const _Alloc& al)
{
	using namespace std;

	typedef std::ptrdiff_t key_type;
	typedef typename iterator_traits<_FwdIt>::value_type value_type;
	typedef typename _Alloc::template rebind<key_type>::other key_allocator;
	

	static_assert(is_integral<value_type>::value,
				  "iterator value type is not an integral type");

	if (first == last)
		return; // nothing to sort

	auto minmax = minmax_element(first, last); // find min & max elements 
	value_type minimum = *minmax.first; // minimal value
	value_type maximum = *minmax.second; // maximal value
	size_t n = distance(first, last); // size of [first, last) range
	size_t k = (maximum - minimum) + 1; // number of buckets in histogram

	if (k >= n) { // k is too large
		sort(first, last); // fallback to quick sort algorithm
		return;
	}

	key_allocator key_alloc(al);

	// get temporary buffer
	key_type* buffer = nullptr;
	size_t kspace = k * sizeof(key_type); // space for counters
	if (kspace < _MAX_STACK_SIZE) // use stack allocation
		buffer = static_cast<key_type*>(alloca(kspace));
	else
		buffer = key_alloc.allocate(n);

	fill(buffer, buffer + k, value_type());

	// create histogram
	for (_FwdIt it = first; it != last; ++it) {
		++buffer[*it - minimum];
	}

	// copy results in [first, last) according to histogram
	for (key_type* it = buffer, *end = buffer + k; it != end; ++it) {
		first = fill_n(first, *it, static_cast<value_type>(std::distance(buffer, it) + minimum));
	}

	// release temporary buffer
	// if space was allocated using alloca() call 
	// it will be automatically disposed after the 
	// function returns
	if (kspace >= _MAX_STACK_SIZE)
		key_alloc.deallocate(buffer, n);
}


/*!
 * \fn counting_sort(_RanIt first, _RanIt last, _Proj proj)
 * \brief Perform a counting sort at range [first, last).
 *
 * \tparam _RanIt models random access iterator
 * \tparam _Proj  models projection unary function
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param proj  projection function object (i.e. an object that project
 * an input value into another value such that a new value is a subset of original)
 *
 */
template <class _RanIt, class _Proj>
void counting_sort(_RanIt first, _RanIt last, _Proj proj)
{
	typedef std::allocator<char> _Alloc_type;
	counting_sort(first, last, proj, _Alloc_type());
}

/*!
 * \fn counting_sort(_RanIt first, _RanIt last, _Proj proj)
 * \brief Perform a counting sort at range [first, last).
 *
 * \tparam _FwdIt models forward iterator
 * \tparam _Proj  models projection unary function
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 *
 */
template <class _FwdIt>
void counting_sort(_FwdIt first, _FwdIt last)
{
	typedef std::allocator<char> _Alloc_type;
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	counting_sort(first, last, stdx::identity<value_type>(), _Alloc_type());
}

_STDX_END

