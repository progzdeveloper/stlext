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
#include <functional>
#include <iterator>
#include <type_traits>

#include "../../platform/common.h"

_STDX_BEGIN

/*!
 * \fn exponential_search(_RandIt first, _RandIt last, const _Key& key, _Comp comp)
 * \brief Find an element equivalent to value appears within the range [first, last)
 * using exponential search.
 *
 * Returns an iterator pointing to the first element in the range [first, last)
 * that is not less than (i.e. greater or equal to) value.
 * The range [first, last) must be partitioned with respect to the expression
 * `element < value` or `comp(element, value)` i.e., all elements for which the
 * expression is true must precede all elements for which the expression is false.
 * A fully-sorted range meets this criterion.
 *
 * Type requirements
 * - _RandIt must meet the requirements of LegacyRandomAccessIterator.
 * - _Comp   must meet the requirements of BinaryPredicate. It is not required to
 * satisfy Compare
 *
 * \tparam _FwdIt  models random access iterator
 * \tparam _Key    models element type
 * \tparam _Comp   models binary comparator
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param key   The value to compare the elements to
 * \param comp  comparison function object (i.e. an object that satisfies the
 * requirements of Compare which returns true if the first argument is less than
 * (i.e. is ordered before) the second.
 *
 * \return Iterator pointing to the first element that is not less than value,
 * or last if no such element is found.
 *
 * \note The algorithm behaves the same as std::lower_bound() from STL library.
 *
 * \note Complexity
 * Best case: O(1)
 * Worst case: O(log i) time, where i is the index where the search key would be
 * in the sequence.
 */
template<class _RandIt, class _Key, class _Comp>
_RandIt exponential_search(_RandIt first, _RandIt last, const _Key& key, _Comp comp)
{
    typedef typename std::iterator_traits<_RandIt>::iterator_category category;
    static_assert(!std::is_same<category, std::input_iterator_tag>::value,
                  "exponential_search() cannot be used with input iterators");

    if (first == last)
        return last; // nothing to search

    size_t size = std::distance(first, last);
    size_t bound = 1; // as 2^0 = 1
    _RandIt curr = std::next(first);
    for (; bound < size; ) {
        if(comp(*curr, key)) {
            first = curr;
            std::advance(curr, (bound + 1) >> 1);
            bound <<= 1; // bound will increase as power of 2
        }
        else
            break; // when array[i] corsses the key element
    }

    // correct upper bound if we out of range
    return std::lower_bound(first, (bound >= size ? last : curr), key, comp);
}

/*!
 * \fn exponential_search(_RandIt first, _RandIt last, const _Key& key)
 * \brief Find a lower bound of key in a input sequence [first, last) using exponential search.
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
template<class _FwdIt, class _Key>
inline _FwdIt exponential_search(_FwdIt first, _FwdIt last, const _Key& key)
{
    return exponential_search(first, last, key, std::less<_Key>());
}

_STDX_END

