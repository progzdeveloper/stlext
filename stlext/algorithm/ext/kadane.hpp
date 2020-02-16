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
#include <iterator>   // for std::iterator_traits
#include <algorithm>  // for std::max
#include <functional> // for std::less
#include <tuple>      // for std::tuple

#include "../../platform/common.h"

/**
 *
 * An implementation of Kadane's algorithm for solving the maximum subarray
 * problem.  In this problem, we are given an array A of real-valued entries
 * and want to find the contiguous subarray of elements with maximum total sum.
 * This problem can be solved very naively in O(n^3) time by computing all
 * O(n^2) possible start and end points, then checking the sum of each range.
 * However, using dynamic programming, it's possible to optimize this to run in
 * O(n) time by looking more closely at the structure of an optimal solution.
 *
 * The idea behind Kadane's algorithm is to treat the problem recursively.  In
 * particular, we will consider the following question.  If we think about any
 * subarray, that subarray is defined by its start and endpoint.  If we
 * consider the optimal solution to the problem, then it must still be optimal
 * if we were to truncate the array just after that subarray.  That is, there
 * must be some index in the array for which the maximum-value subarray is the
 * largest subarray that ends at that particular index.  If we can scan across
 * the array and check which location this ends up being, and if we can check
 * each possible location in O(1) time each, we'll have ourselves an O(n)
 * algorithm for the problem.
 *
 * Fortunately, if we compute these values in a particular order, we can indeed
 * compute all of these values in time O(1) each.  The idea is as follows.
 * Let's define a function F(i) to be the weight of the maximum-weight subarray
 * ending just before position i in the array.  This means, for example, that
 * F(0) would be 0, since the weight of the maximum-weight subarray ending just
 * before the first character is the weight of the empty subarray, which is
 * zero.
 *
 * Now, let's think about the relationship between F(i) and F(i + 1).  That is,
 * if we were to take the maximum-weight subarray ending just before character
 * i, could we use information about it to determine the size of the maximum-
 * weight subarray ending just before element i + 1?  Well, let's think about
 * what the possible maximum subarrays ending just before element i + 1 might
 * look like.  We can take any number of elements from the suffix of the first
 * i elements of the array, of which the one that has the maximum value will be
 * given by F(i), the weight of the maximum-weight subarray ending at position
 * i.  Moreover, we know that F(i) >= 0, since in the worst case the largest
 * subarray ending at position i could just be the empty subarray.  Thus if we
 * want to consider the subarray formed by extending the maximum-weight
 * subarray ending just before position i, we would do so by adding in the
 * array element at position i.  This means that one possible subarray we could
 * pick has value F(i) + A[i].  The other option is to take no elements at all,
 * which would happen if, for example, the value of A[i] is so enormously
 * negative that it would dwarf the result.  This means that we have the nice
 * recurrence that
 *
 *     F(0) = 0
 *     F(i + 1) = max { F(i) + A[i], 0 }
 *
 * This can be computed trivially by a forward scan over the array.  Once we
 * have all of the values of this function, the maximum one we encounter must
 * be the maximum-weight subarray for the overall array.  We can implement this
 * naively by actually storing all the values of F, but since all we care about
 * is the maximum value attained by F, we can do so in O(1) space by just
 * storing the maximum value encountered so far.  The result is an O(n)-time,
 * O(1)-space algorithm for finding the maximum-weight subarray of the array.
 * Moreover, the algorithm works perfectly fine on a stream of elements, since
 * we need just one pass over the data.
 *
 */


_STDX_BEGIN
/*!
 * \brief kadane_sum compute the weight of the maximum/minimum-weight subarray in the range [begin, end)
 *
 * Given a range of elements [begin, end), returns the weight of the maximum-
 * weight subarray in the range [begin, end).
 * \tparam _InIt iterator type that meet requipments of input iterator
 * \tparam _Comp user-supplied comparator functor
 * \param begin iterator pointing to first range element
 * \param end iterator pointing to one-past-last element
 * \return weight of the maximum-weight subarray
 */
template <class _InIt, class _Comp>
typename std::iterator_traits<_InIt>::value_type
	kadane_sum(_InIt begin, _InIt end, _Comp comp) 
{
	/* For sanity's sake, give the underlying type being iterated over a nicer name. */
	typedef typename std::iterator_traits<_InIt>::value_type elem_t;

	if (begin == end)
		return elem_t();

	/* Initially, assume that the maximum value found so far is equal 
	 * to first element, since in
	 * the worst-case we can just take nothing.
	 */
	elem_t result = *begin;

	/* The value of the maximum-weight subarray ending just before the current
	 * iterator, which corresponds to F(0) initially.
	 */
	elem_t endingValue = *begin;

	/* Scan across the elements, computing the maximum-weight subarray ending at
	* each point.  At every point, update our resulting maximum value based on
	* what we've found so far.
	*/
	for (; begin != end; ++begin) {
		/* Update the value of the subarray ending at this element according to the
		* above recurrence relation.
		*/
		endingValue = (std::max)(*begin, endingValue + *begin, comp);

		/* Update the global maximum value based on this candidate answer. */
		result = (std::max)(result, endingValue, comp);
	}

	return result;
}

/*!
 * \brief kadane_max compute the weight of the maximum-weight subarray in the range [begin, end)
 *
 * Given a range of elements [begin, end), returns the weight of the maximum-
 * weight subarray in the range [begin, end).
 * \tparam _InIt iterator type that meet requipments of input iterator
 * \tparam _Comp user-supplied comparator functor
 * \param begin iterator pointing to first range element
 * \param end iterator pointing to one-past-last element
 * \return weight of the maximum-weight subarray
 * \note Non-comparator version implemented in terms of comparator version.
 */
template <class _InIt>
inline typename std::iterator_traits<_InIt>::value_type
	kadane_max(_InIt begin, _InIt end)
{
	/* For sanity's sake, give the underlying type being iterated over a nicer name. */
	typedef typename std::iterator_traits<_InIt>::value_type elem_t;
	return kadane_sum(begin, end, std::less<elem_t>());
}


/*!
* \brief kadane_min compute the weight of the minimum-weight subarray in the range [begin, end)
*
* Given a range of elements [begin, end), returns the weight of the maximum-
* weight subarray in the range [begin, end).
* \tparam _InIt iterator type that meet requipments of input iterator
* \tparam _Comp user-supplied comparator functor
* \param begin iterator pointing to first range element
* \param end iterator pointing to one-past-last element
* \return weight of the maximum-weight subarray
* \note Non-comparator version implemented in terms of comparator version.
*/
template <class _InIt>
inline typename std::iterator_traits<_InIt>::value_type
	kadane_min(_InIt begin, _InIt end)
{
	/* For sanity's sake, give the underlying type being iterated over a nicer name. */
	typedef typename std::iterator_traits<_InIt>::value_type elem_t;
	return kadane_sum(begin, end, std::greater<elem_t>());
}



/*!
 * \brief kadane_range compute the weight of the maximum/minimum-weight subarray in the range [begin, end)
 *
 * Given a range of elements [begin, end), returns the weight of the maximum-
 * weight subarray and a subarray range [first, last) as a tuple.
 *
 * \tparam _FwdIt iterator type that meet requipments of forward-only iterator
 * \param begin iterator pointing to first range element
 * \param end iterator pointing to one-past-last element
 * \return on success return a three elements std::tuple<> witch contains:
 * - a weight value as first element
 * - iterator pointing to first element of subarray as second element
 * - iterator pointing to one-past-last element of subarray as third element
 * on failure return a three elements std::tuple<> witch contains:
 * - a zero value as first element
 * - iterator pointing to end of input range as second element
 * - iterator pointing to end of input range as third element
 */
template <class _FwdIt, class _Comp>
std::tuple<typename std::iterator_traits<_FwdIt>::value_type, _FwdIt, _FwdIt>
	kadane_range(_FwdIt begin, _FwdIt end, _Comp comp)
{
	/* For sanity's sake, give the underlying type being iterated over a nicer name. */
	typedef typename std::iterator_traits<_FwdIt>::value_type elem_t;

	if (begin == end)
		return std::make_tuple(elem_t(), end, end);

	/* Initially, assume that the maximum value found so far is zero, since in
	 * the worst-case we can just take nothing.
	 */
	elem_t result = *begin;

	/* The value of the maximum-weight subarray ending just before the current
	 * iterator, which corresponds to F(0) initially (with value zero).
	 */
	elem_t endingValue = *begin;

	/* Temporary value */
	elem_t tmpval;

	_FwdIt first = begin, last = end, mid = begin;
	/* Scan across the elements, computing the maximum-weight subarray ending at
	 * each point.  At every point, update our resulting maximum value based on
	 * what we've found so far.
	 */
	for (; begin != end; ++begin)
	{
		/* Update the value of the subarray ending at this element according to the
		 * above recurrence relation.
		 */
		tmpval = endingValue + *begin;
		if (comp(tmpval, *begin)) {
			endingValue = *begin;
			mid = begin;
		}
		else {
			endingValue = tmpval;
		}

		/* Update the global maximum value based on this candidate answer. */
		if (comp(result, endingValue)) {
			result = endingValue;
			last = begin;
			first = mid;
		}
	}
	return std::make_tuple(result, first, last);
}

/*!
* \brief kadane_range_max compute the weight of the maximum-weight subarray in the range [begin, end)
*
* Given a range of elements [begin, end), returns the weight of the maximum-
* weight subarray and a subarray range [first, last) as a tuple.
*
* \tparam _FwdIt iterator type that meet requipments of forward-only iterator
* \param begin iterator pointing to first range element
* \param end iterator pointing to one-past-last element
* \return on success return a three elements std::tuple<> witch contains:
* - a weight value as first element
* - iterator pointing to first element of subarray as second element
* - iterator pointing to one-past-last element of subarray as third element
* on failure return a three elements std::tuple<> witch contains:
* - a zero value as first element
* - iterator pointing to end of input range as second element
* - iterator pointing to end of input range as third element
*/
template <class _FwdIt>
inline std::tuple<typename std::iterator_traits<_FwdIt>::value_type, _FwdIt, _FwdIt>
	kadane_range_max(_FwdIt begin, _FwdIt end)
{
	/* For sanity's sake, give the underlying type being iterated over a nicer name. */
	typedef typename std::iterator_traits<_FwdIt>::value_type elem_t;
	return kadane_range(begin, end, std::less<elem_t>());
}

/*!
* \brief kadane_range_min compute the weight of the minimum-weight subarray in the range [begin, end)
*
* Given a range of elements [begin, end), returns the weight of the minimum-
* weight subarray and a subarray range [first, last) as a tuple.
*
* \tparam _FwdIt iterator type that meet requipments of forward-only iterator
* \param begin iterator pointing to first range element
* \param end iterator pointing to one-past-last element
* \return on success return a three elements std::tuple<> witch contains:
* - a weight value as first element
* - iterator pointing to first element of subarray as second element
* - iterator pointing to one-past-last element of subarray as third element
* on failure return a three elements std::tuple<> witch contains:
* - a zero value as first element
* - iterator pointing to end of input range as second element
* - iterator pointing to end of input range as third element
*/
template <class _FwdIt>
inline std::tuple<typename std::iterator_traits<_FwdIt>::value_type, _FwdIt, _FwdIt>
	kadane_range_min(_FwdIt begin, _FwdIt end)
{
	/* For sanity's sake, give the underlying type being iterated over a nicer name. */
	typedef typename std::iterator_traits<_FwdIt>::value_type elem_t;
	return kadane_range(begin, end, std::greater<elem_t>());
}

_STDX_END

