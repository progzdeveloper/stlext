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
#include <functional> // for equal_to
#include <iterator>   // for iterator_traits

#include "../../platform/common.h"

/*!
 *
 * A one-pass, linear time algorithm that returns the majority element of a
 * range of data.  The majority element is an element that appears strictly
 * more than half the time.  For example, in the sequence
 *
 *                                  0 1 0 0 2 0 3
 *
 * The number 0 is a majority element, since it appears 4/7 times.  However,
 * in the sequence
 *
 *                                 0 1 0 0 2 0 3 3
 *
 * There is no majority element, since even though 0 occurs 4/8 times, this
 * isn't strictly greater than half the elements.
 *
 * The algorithm for finding the majority element is remarkably simple, but its
 * correctness is not immediately obvious.  The algorithm works as follows.  At
 * each step, we maintain our "guess" of what the majority element will be, and
 * also a counter.  We then scan across the array.  At each point, if the new
 * element matches our current guess we increment the counter, and otherwise
 * we decrement it.  If the counter is ever zero, then on the next element we
 * change the counter to 1 and pick the next element as our guess.  Finally, we
 * output the guessed element.  For example, here is the algorithm running on
 * the earlier input.  The topmost row shows the input, below that our guess,
 * and below that the counter:
 *
 *  INPUT    0 1 0 0 2 0 3
 *  GUESS   ? 0 ? 0 0 0 0 0
 *  COUNTER 0 1 0 1 2 1 2 1
 *
 * Since our guess at the end is zero, we output zero.  This algorithm is
 * due to Boyer and Moore and is described in their paper "MJRTY - A Fast
 * Majority Vote Algorithm."
 *
 * There are many ways to think about why this algorithm works.  One good
 * intuition is to think of the algorithm as breaking the input down into lots
 * of stretches of consecutive copies of particular values.  Incrementing the
 * counter then corresponds to marking that multiple copies of the same value
 * were found, while decrementing it corresponds to some other sequences of
 * values "canceling out" the accumulation of values of a particular type.
 *
 * A formal proof of correctness of this algorithm (based on the proof in
 * Boyer and Moore's paper) relies on a key lemma.  In this section, we'll
 * let C be the number that is currently a candidate for the majority element,
 * K be its count after some number of steps, and N be the number of total
 * elements.
 *
 * Lemma 1: For any i, 1 <= i <= N, after i steps of the algorithm, the
 * elements in the range [1, i] can rearranged into two groups A and B such
 * that A is K copies of C, and B is a collection of elements with at most
 * i / 2 copies of any one element.
 *
 * Let's hold off of the proof of this lemma for now, and show that if it
 * holds and there is a majority element, the algorithm must be correct.  Using
 * the above lemma, note that when the algorithm terminates, there must be some
 * element C that was chosen with some count K.  Assume for the sake of
 * contradiction that C is not the majority element; then there is some other
 * element C' that must be the majority element.  Consequently, there are at
 * least n / 2 elements of the range equal to C'.  Let's consider where they
 * are.  By the above lemma, all the elements of the input can be broken up
 * into groups A and B, where everything in group A has value C and at most
 * |B| / 2 elements of |B| have value C'.  Since |A| = K and |A| + |B| = N,
 * this means that there are at most (N - K) / 2 copies of C', contradicting
 * the fact that C' is the actual majority element.  We have reached a
 * contradiction, and so C must be the majority element at the end of the
 * algorithm's run.
 *
 * We can now prove the claim of the lemma by induction on i.  As a base case,
 * if i = 1, then K = 1 and C is the first element of the range.  Then we can
 * let A be the singleton element and B be the empty set, which trivially obeys
 * the criteria of the lemma.  For the inductive step, assume that for some i
 * the claim holds and consider the execution of the algorithm on step i + 1.
 * Let A and B be the sets A and B from the ith step.  Then we consider three
 * possible cases:
 *
 * 1. On entry to this step, K = 0.  Then after this step finishes, K = 1
 *    and C is the newest element.  This means that on entry to this step,
 *    A was the empty set and B was some set where no element appeared more
 *    than i/2 times in B.  If we then let A' be the singleton set containing
 *    the new element and B' = B, then these sets satisfy the requirements of
 *    the lemma and the claim holds.
 * 2. On entry to this step, K > 0 and the new element matches the current
 *    majority element.  Then we can add this element to A to get a new set
 *    A' meeting the lemma's requirements, so the claim holds.
 * 3. On entry to this step, K > 0, and the new element does not match the
 *    current majority element.  This means that the new K is one minus the
 *    previous K, but the candidate majority element does not change.  If
 *    we then move one element from A into B, then place the new element into
 *    the set B, then the updated A and B will satisfy the lemma's claims.
 *    This is tedious but simple to check, so I'll leave it as an exercise
 *    to the reader. :-)
 *
 * In the case where there is no majority element, the element produced by the
 * algorithm will be arbitrary. We can then check whether we have the majority
 * element by performing a linear scan over the input range and counting the
 * frequency of the element.
 *
 */

_STDX_BEGIN

/*!
 * \brief majority_element() find a majority element in a [begin, end) range
 *
 * Given a range of values [begin, end) where strictly more than half the
 * elements have the same value according to the binary comparator comp,
 * returns the value of this most-common element.  If no element is a majority
 * element, end is returned as a sentinel.
 * 
 * \tparam _FwdIt type of iterator that meet requipments of forward-only iterator
 * \tparam _Comp type of user-supplied equality comparator functor
 * \param begin iterator pointing to begin of range
 * \param end iterator pointing to one-past-end element of range
 * \param comp user-supplied comparator functor
 */
template <class _FwdIt, class _Comp>
_FwdIt majority_element(_FwdIt begin, _FwdIt end, _Comp comp) 
{
	/* Initially, we have no guess and our count is zero.  However, to avoid
	 * edge cases with the empty range, we initialize the candidate to end.
	 */
	_FwdIt candidate = end;
	size_t confidence = 0;

	/* Scan over the input using the Boyer-Moore update rules. */
	for (_FwdIt it = begin; it != end; ++it) {
		/* If we have no confidence in our previous guess, update it to this new
		 * element.
		 */
		if (confidence == 0) {
			candidate = it;
			++confidence;
		}
		/* Otherwise, increment or decrement the confidence based on whether the
		 * next element matches.
		 */
		else if (comp(*candidate, *it))
			++confidence;
		else
			--confidence;
	}

	/* Finally, do one more pass to confirm that we have a majority element. */
	size_t nmatches = 0, ntotal = 0;
	for (_FwdIt it = begin; it != end; ++it) {
		/* Check whether this is a match and update appropriately. */
		if (comp(*candidate, *it))
			++nmatches;

		/* Either way, increment the total number of elements. */
		++ntotal;
	}

	/* This is a majority element if it accounts for at least half the number
	 * of elements.
	 */
	return (((ntotal / 2) < nmatches) ? candidate : end);
}

/*!
 * \brief majority_element() find a majority element in a [begin, end) range
 *
 * Given a range of values [begin, end) where strictly more than half the
 * elements have the same value according to the binary comparator std::equal_to<>,
 * returns the value of this most-common element.  If no element is a majority
 * element, end is returned as a sentinel.
 *
 * \tparam _FwdIt type of iterator that meet requipments of forward-only iterator
 * \param begin iterator pointing to begin of range
 * \param end iterator pointing to one-past-end element of range
 * \note std::equal_to<> is used to compare values.
 * \note Non-comparator version implemented in terms of comparator version.
 */
template <class _FwdIt>
inline _FwdIt majority_element(_FwdIt begin, _FwdIt end) {
	typedef typename std::iterator_traits<_FwdIt>::value_type val_t;
	return majority_element(begin, end, std::equal_to<val_t>());
}

/*!
 * \brief is_majority_element() check if a given value is actually a majority element.
 *
 * Given a sorted range of values [begin, end) check whatever supplied element x 
 * is actually a majority element.
 *
 * \tparam _FwdIt type of iterator that meet requipments of forward-only iterator
 * \tparam _Comp type of user-supplied comparator functor
 * \param begin iterator pointing to begin of range
 * \param end iterator pointing to one-past-end element of range
 * \param x value of majority candidate 
 * \param comp comp user-supplied comparator functor
 * \return true if x is a majority element, otherwise return false
 * \note range [begin, end) must be sorted
 */
template <class _FwdIt, class _Value, class _Comp>
inline bool is_majority_element(_FwdIt begin, _FwdIt end, const _Value& x, _Comp comp)
{
	/* Find a range of first and last occurences of the element x */
	auto range = std::equal_range(begin, end, x, comp);

	/* This is a majority element if it accounts for at 
	 * least half the number of elements.
	 */
	return (std::distance(range.first, range.second) > (std::distance(begin, end) / 2));
}

/*!
 * \brief is_majority_element() check if a given value is actually a majority element.
 *
 * Given a sorted range of values [begin, end) check whatever supplied element x
 * is actually a majority element.
 *
 * \tparam _FwdIt type of iterator that meet requipments of forward-only iterator
 * \param begin iterator pointing to begin of range
 * \param end iterator pointing to one-past-end element of range
 * \param x value of majority candidate
 * \return true if x is a majority element, otherwise return false
 *
 * \note range [begin, end) must be sorted
 * \note std::less<> is used to compare values.
 * \note Non-comparator version implemented in terms of comparator version.
 */
template <class _FwdIt, class _Value>
inline bool is_majority_element(_FwdIt begin, _FwdIt end, const _Value& x)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type val_t;
	return is_majority_element(begin, end, x, std::less<val_t>());
}


_STDX_END

