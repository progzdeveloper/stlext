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

#include "../../platform/common.h"
#include <iterator>     // for std::iterator_traits
#include <algorithm>    // for std::equal
#include <functional>   // for std::equal_to

_STDX_BEGIN

namespace detail {

	template <class T1, class T2>
	struct eq : public std::binary_function<T1, T2, bool> {
		bool operator () (const T1& v1, const T2& v2) const { 
			return (v1 == v2);
		}
	};

	template <class RandomAccessIterator1, class RandomAccessIterator2, class BinaryPredicate>
	bool equal(RandomAccessIterator1 first1, RandomAccessIterator1 last1,
			   RandomAccessIterator2 first2, RandomAccessIterator2 last2, BinaryPredicate pred,
			   std::random_access_iterator_tag, std::random_access_iterator_tag)
	{
		//  Random-access iterators let is check the sizes in constant time
		if (std::distance(first1, last1) != std::distance(first2, last2))
			return false;
		// If we know that the sequences are the same size, the original version is fine
		return std::equal(first1, last1, first2, pred);
	}

	template <class InputIterator1, class InputIterator2, class BinaryPredicate>
	bool equal(InputIterator1 first1, InputIterator1 last1,
			   InputIterator2 first2, InputIterator2 last2, BinaryPredicate pred,
			   std::input_iterator_tag, std::input_iterator_tag)
	{
		for (; first1 != last1 && first2 != last2; ++first1, ++first2)
			if (!pred(*first1, *first2))
				return false;

		return first1 == last1 && first2 == last2;
	}
}

/*! \fn equal ( InputIterator1 first1, InputIterator1 last1, 
 *             InputIterator2 first2, InputIterator2 last2,
 *             BinaryPredicate pred )
 * \return true if all elements in the two ranges are equal
 * 
 * \param first1    The start of the first range.
 * \param last1     One past the end of the first range.
 * \param first2    The start of the second range.
 * \param last2     One past the end of the second range.
 * \param pred      A predicate for comparing the elements of the ranges
 */
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1,
		   InputIterator2 first2, InputIterator2 last2, BinaryPredicate pred)
{
	return stdx::detail::equal(
		first1, last1, first2, last2, pred,
		typename std::iterator_traits<InputIterator1>::iterator_category(),
		typename std::iterator_traits<InputIterator2>::iterator_category()
	);
}

/*!
 * \fn equal ( InputIterator1 first1, InputIterator1 last1, 
 *             InputIterator2 first2, InputIterator2 last2 )
 * \return true if all elements in the two ranges are equal
 * 
 * \param first1    The start of the first range.
 * \param last1     One past the end of the first range.
 * \param first2    The start of the second range.
 * \param last2     One past the end of the second range.
 */
template <class InputIterator1, class InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1,
		   InputIterator2 first2, InputIterator2 last2)
{
	typedef typename stdx::detail::eq<
		typename std::iterator_traits<InputIterator1>::value_type,
		typename std::iterator_traits<InputIterator2>::value_type
	> _eq_type;
	return stdx::detail::equal(
		first1, last1, first2, last2, _eq_type(),
		typename std::iterator_traits<InputIterator1>::iterator_category(),
		typename std::iterator_traits<InputIterator2>::iterator_category());
}


_STDX_END


