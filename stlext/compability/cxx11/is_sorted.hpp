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

#include <algorithm>
#include <functional>
#include <iterator>

_STDX_BEGIN

/*! 
 * \fn is_sorted_until ( _FwdIt first, _FwdIt last, _Pred p )
 * \return the point in the sequence [first, last) where the elements are unordered
 *         (according to the comparison predicate 'p').
 * \tparam _FwdIt models forward iterator
 * \tparam _Pred models binary predicate
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 * \param p     A binary predicate that returns true if two elements are ordered.
 */
template <class _FwdIt, class _Pred>
_FwdIt is_sorted_until(_FwdIt first, _FwdIt last, _Pred p)
{
	if (first == last) return last;  // the empty sequence is ordered
	_FwdIt next = first;
	while (++next != last)
	{
		if (p(*next, *first))
			return next;
		first = next;
	}
	return last;
}

/*! 
 * \fn is_sorted_until ( _FwdIt first, _FwdIt last )
 * \return the point in the sequence [first, last) where the elements are unordered
 * \tparam _FwdIt models forward iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 */
template <class _FwdIt>
_FwdIt is_sorted_until(_FwdIt first, _FwdIt last)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	return stdx::is_sorted_until(first, last, std::less<value_type>());
}


/*! 
 * \fn is_sorted ( _FwdIt first, _FwdIt last, Pred p )
 * \return whether or not the entire sequence is sorted
 * \tparam _FwdIt models forward iterator
 * \tparam _Pred models binary predicate
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 * \param p     A binary predicate that returns true if two elements are ordered.
 */
template <class _FwdIt, class Pred>
bool is_sorted(_FwdIt first, _FwdIt last, Pred p)
{
	return stdx::is_sorted_until(first, last, p) == last;
}

/*! 
 * \fn is_sorted ( _FwdIt first, _FwdIt last )
 * \return whether or not the entire sequence is sorted
 * \tparam _FwdIt models forward iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 */
template <class _FwdIt>
bool is_sorted(_FwdIt first, _FwdIt last)
{
	return stdx::is_sorted_until(first, last) == last;
}


///
/// -- Range based versions of the C++11 functions
///

/*! 
 * \fn is_increasing ( _FwdIt first, _FwdIt last )
 * \return true if the entire sequence is increasing; i.e, each item is greater than or  
 *     equal to the previous one.
 * \tparam _FwdIt models forward iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 *
 * \note This function will return true for sequences that contain items that compare
 *     equal. If that is not what you intended, you should use is_strictly_increasing instead.
 */
template <class _FwdIt>
bool is_increasing(_FwdIt first, _FwdIt last)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	return stdx::is_sorted(first, last, std::less<value_type>());
}


/*! 
 * \fn is_decreasing ( _FwdIt first, _FwdIt last )
 * \return true if the entire sequence is decreasing; i.e, each item is less than 
 *     or equal to the previous one.
 * \tparam _FwdIt models forward iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 *
 * \note This function will return true for sequences that contain items that compare
 *     equal. If that is not what you intended, you should use is_strictly_decreasing instead.
 */
template <class _FwdIt>
bool is_decreasing(_FwdIt first, _FwdIt last)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	return stdx::is_sorted(first, last, std::greater<value_type>());
}


/*! 
 * \fn is_strictly_increasing ( _FwdIt first, _FwdIt last )
 * \return true if the entire sequence is strictly increasing; i.e, each item is greater
 *     than the previous one
 * \tparam _FwdIt models forward iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 * 
 * \note This function will return false for sequences that contain items that compare
 *     equal. If that is not what you intended, you should use is_increasing instead.
 */
template <class _FwdIt>
bool is_strictly_increasing(_FwdIt first, _FwdIt last)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	return stdx::is_sorted(first, last, std::less_equal<value_type>());
}


/*! 
 * \fn is_strictly_decreasing ( _FwdIt first, _FwdIt last )
 * \return true if the entire sequence is strictly decreasing; i.e, each item is less than
 *     the previous one
 * \tparam _FwdIt models forward iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence
 *
 * \note This function will return false for sequences that contain items that compare
 *     equal. If that is not what you intended, you should use is_decreasing instead.
 */
template <class _FwdIt>
bool is_strictly_decreasing(_FwdIt first, _FwdIt last)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	return stdx::is_sorted(first, last, std::greater_equal<value_type>());
}

_STDX_END
