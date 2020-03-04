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
#include <algorithm>
#include <iterator>
#include <type_traits>

#include "../../platform/common.h"
#include "../../compability/functional"
#include "../../functional/function_traits.hpp"

_STDX_BEGIN

/*!
 * \fn interpolation_search(_RandIt first, _RandIt last, const _Key& key, _Proj proj)
 * \brief Find a lower bound equivalent to value appears within the range [first, last)
 * using interpolation search.
 *
 * Find an iterator pointing to the first element in the range [first, last)
 * that is not less than (i.e. greater or equal to) value.
 * The range [first, last) must be at least partially ordered, i.e. partitioned with
 * respect to the expression `element < value` or `comp(element, value)`.
 *
 * \tparam _RandIt models random access iterator
 * \tparam _Key    models element type
 * \tparam _Proj   models projection unary function
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param key   The value to compare the elements to
 * \param proj  projection function object (i.e. an object that project
 * an input value into another value such that a new value is a subset of original).
 *
 * \return Iterator pointing to the first element that is not less than value,
 * or last if no such element is found.
 */
template<class _It, class _Key, class _Proj>
_It interpolation_search(_It first, _It last, const _Key& what, _Proj proj)
{
	//typedef typename std::result_of<_Proj(_Key)>::type index_type;
	typedef typename stdx::function_traits<_Proj>::result_type index_type;
	typedef typename std::iterator_traits<_It>::iterator_category category;
	static_assert(std::is_same<category, std::random_access_iterator_tag>::value,
				  "interpolation_search() can be used with random access iterators only");

	if (first == last)
		return last; // nothing to search

#ifdef _STDX_DEBUG
    __stdx_assertx(std::is_sorted(first, last, [&proj](const _Key& x, const _Key& y) { return proj(x) < proj(y); }),
                   std::invalid_argument, "[first, last) range must be sorted");
#endif

	_It lowIt = first;
	_It highIt = std::prev(last);
	_It midIt;

	index_type mid;
	index_type low = proj(*lowIt);
	index_type high = proj(*highIt);
	index_type key = what;

	while (high != low && key >= low && key <= high)
	{
		midIt = lowIt + ((key - low) * (highIt - lowIt) / (high - low));
		mid = proj(*midIt);

		if (mid < key) {
			lowIt = midIt + 1;
			low = proj(*lowIt);
		}
		else if (mid > key) {
			highIt = midIt - 1;
			high = proj(*highIt);
		}
		else
			return midIt;
	}

	return ((key == low) ? lowIt : last);
}

/*!
* \fn interpolation_search(_RandIt first, _RandIt last, const _Key& key, _Proj proj)
* \brief Find a lower bound equivalent to value appears within the range [first, last)
* using interpolation search.
*
* Find an iterator pointing to the first element in the range [first, last)
* that is not less than (i.e. greater or equal to) value.
* The range [first, last) must be at least partially ordered, i.e. partitioned with
* respect to the expression `element < value` or `comp(element, value)`.
*
* \tparam _RandIt models random access iterator
* \tparam _Key    models element type
*
* \param first The start of the input sequence
* \param last  One past the end of the input sequence
* \param key   The value to compare the elements to
*
* \return Iterator pointing to the first element that is not less than value,
* or last if no such element is found.
*/
template<class _RandIt, class _Key, class _Proj>
_RandIt interpolation_search(_RandIt first, _RandIt last, const _Key& key)
{
	typedef typename std::iterator_traits<_RandIt>::value_type value_type;
	return interpolation_search(first, last, key, stdx::identity<_Key>());
}

_STDX_END
