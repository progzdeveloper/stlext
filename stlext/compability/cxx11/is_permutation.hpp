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

#include <utility>      // for std::make_pair
#include <functional>   // for std::equal_to
#include <iterator>

#include "../../platform/common.h"

_STDX_BEGIN

/// \cond DOXYGEN_HIDE
namespace detail {
	template <typename Predicate, typename Iterator>
	struct value_predicate {
		value_predicate(Predicate p, Iterator it) : p_(p), it_(it) {}

		template <typename T1>
		bool operator () (const T1 &t1) const { return p_(*it_, t1); }
	private:
		Predicate p_;
		Iterator it_;
	};

	//  Preconditions:
	//  1. The sequences are the same length
	//  2. Any common elements on the front have been removed (not necessary for correctness, just for performance)
	template< class ForwardIterator1, class ForwardIterator2, class BinaryPredicate >
	bool is_permutation_inner(ForwardIterator1 first1, ForwardIterator1 last1,
							  ForwardIterator2 first2, ForwardIterator2 last2,
							  BinaryPredicate p) {
		//  for each unique value in the sequence [first1,last1), count how many times
		//  it occurs, and make sure it occurs the same number of times in [first2, last2)
		for (ForwardIterator1 iter = first1; iter != last1; ++iter) {
			value_predicate<BinaryPredicate, ForwardIterator1> pred(p, iter);

			/*  For each value we haven't seen yet... */
			if (std::find_if(first1, iter, pred) == iter) {
				size_t dest_count = std::count_if(first2, last2, pred);
				if (dest_count == 0 || dest_count != (size_t) std::count_if(iter, last1, pred))
					return false;
			}
		}

		return true;
	}

	template< class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
	bool is_permutation_tag(ForwardIterator1 first1, ForwardIterator1 last1,
							ForwardIterator2 first2, ForwardIterator2 last2,
							BinaryPredicate p,
							std::forward_iterator_tag, std::forward_iterator_tag) {

		//  Skip the common prefix (if any)
		while (first1 != last1 && first2 != last2 && p(*first1, *first2)) {
			++first1;
			++first2;
		}
		if (first1 != last1 && first2 != last2)
			return detail::is_permutation_inner(first1, last1, first2, last2,
												std::equal_to<typename std::iterator_traits<ForwardIterator1>::value_type>());
		return first1 == last1 && first2 == last2;
	}

	template <class RandomAccessIterator1, class RandomAccessIterator2, class BinaryPredicate>
	bool is_permutation_tag(RandomAccessIterator1 first1, RandomAccessIterator1 last1,
							RandomAccessIterator2 first2, RandomAccessIterator2 last2,
							BinaryPredicate p,
							std::random_access_iterator_tag, std::random_access_iterator_tag) {
		//  Cheap check
		if (std::distance(first1, last1) != std::distance(first2, last2))
			return false;
		//  Skip the common prefix (if any)
		while (first1 != last1 && first2 != last2 && p(*first1, *first2)) {
			++first1;
			++first2;
		}

		if (first1 != last1 && first2 != last2)
			return is_permutation_inner(first1, last1, first2, last2, p);
		return first1 == last1 && first2 == last2;
	}

}
/// \endcond

/*!
 * \fn is_permutation ( _FwdIt1 first, _FwdIt1 last, _FwdIt2 first2, _BinPred p )
 * \brief Tests to see if the sequence [first,last) is a permutation of the sequence starting at first2
 * \tparam _FwdIt1  models forward only iterator
 * \tparam _FwdIt1  models forward only iterator
 * \tparam _BinPred models binary predicate
 * \param first1   The start of the input sequence
 * \param last1    One past the end of the input sequence
 * \param first2   The start of the second sequence
 * \param p        The predicate to compare elements with
 *
 * \note           This function is part of the C++2011 standard library.
 */
template< class _FwdIt1, class _FwdIt2, class _BinPred >
bool is_permutation (_FwdIt1 first1, _FwdIt1 last1,
					 _FwdIt2 first2, _BinPred p )
{
	//  Skip the common prefix (if any)
	std::pair<_FwdIt1, _FwdIt2> eq = std::mismatch(first1, last1, first2, p);
	first1 = eq.first;
	first2 = eq.second;
	if (first1 != last1) {
		//  Create last2
		_FwdIt2 last2 = first2;
		std::advance(last2, std::distance(first1, last1));
		return stdx::detail::is_permutation_inner(first1, last1, first2, last2, p);
	}

	return true;
}
/*!
 * \fn is_permutation ( ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 first2 )
 * \brief Tests to see if the sequence [first,last) is a permutation of the sequence starting at first2
 * \tparam _FwdIt1  models forward only iterator
 * \tparam _FwdIt1  models forward only iterator
 * \param first1   The start of the input sequence
 * \param last2    One past the end of the input sequence
 * \param first2   The start of the second sequence
 * 
 * \note           This function is part of the C++2011 standard library.
 */
template< class _FwdIt1, class _FwdIt2 >
bool is_permutation (_FwdIt1 first1, _FwdIt2 last1, _FwdIt2 first2 )
{
	//  How should I deal with the idea that ForwardIterator1::value_type
	//  and ForwardIterator2::value_type could be different? Define my own comparison predicate?
	//  Skip the common prefix (if any)
	std::pair<_FwdIt1, _FwdIt2> eq = std::mismatch(first1, last1, first2);
	first1 = eq.first;
	first2 = eq.second;
	if (first1 != last1) {
		//  Create last2
		_FwdIt2 last2 = first2;
		std::advance(last2, std::distance(first1, last1));
		return stdx::detail::is_permutation_inner(first1, last1, first2, last2,
											std::equal_to<typename std::iterator_traits<_FwdIt1>::value_type>());
	}
	return true;
}

_STDX_END

