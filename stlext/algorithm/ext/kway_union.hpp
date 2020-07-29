// Copyright (c) 2020, Michael Polukarov (Russia).
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

#include <vector>
#include <iterator>
#include <queue>

#ifdef _HAVE_BOOST_LIBS
#include <boost/heap/pairing_heap.hpp>
#endif

#include "../../platform/common.h"

#include "kway_utility.hpp"


_STDX_BEGIN

#ifdef _HAVE_BOOST_LIBS

template<class _Container, class _OutIt, class _Comp>
_OutIt __kway_union_large(const std::vector< _Container >& inputs, _OutIt out, _Comp comp)
{
    using namespace utility;

    typedef typename container_traits<_Container>::const_iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;

    typedef entry<iterator, _Comp> entry_t;

    // create min/max heap
    boost::heap::pairing_heap< entry_t > queue;

    // push all first elements of each range
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        auto first = utility::begin(*it);
        auto last  = utility::end(*it);
        if (first != last)
            queue.emplace(first, last);
    }

    // flag that is set when
    // none of elements was written
    // into output sequence
    bool empty = true;

    // cached value
    value_type cached;

    // now one by one get the minimum/maximum element
    // from min heap and replace it with next
    // element of its array
    while (!queue.empty())
    {
        entry_t e = queue.top();
        queue.pop();

        // here is a tricky part:
        // check if we already put something in
        // output sequence or last written element
        // is less/greater than current
        if (empty || comp(cached, *e)) {
            // cache current element
            cached = *e;
            // write cached element into output sequence
            *out = cached; ++out;
            // turn off empty flag
            empty = false;
        }

        ++e;
        if (e) {
            queue.push(e);
        }
    }
    return out;
}
#endif


template<class _Container, class _OutIt, class _Comp>
_OutIt __kway_union_small(const std::vector< _Container >& inputs, _OutIt out, _Comp comp)
{
    using namespace utility;

    typedef typename container_traits<_Container>::const_iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;

    typedef entry<iterator, _Comp> entry_t;

    switch(inputs.size()) {
    case 0:
        return out;
    case 1:
        return copy(utility::begin(*inputs.begin()), utility::end(*inputs.begin()), out);
    }

    // create min/max heap
    std::vector<entry_t> queue;
    queue.reserve(inputs.size());

    // push all first elements of each range
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        auto first = utility::begin(*it);
        auto last  = utility::end(*it);
        if (first != last)
            queue.emplace_back(first, last);
    }
    utility::__small_sort(queue.begin(), queue.end());

    // flag that is set when
    // none of elements was written
    // into output sequence
    bool empty = true;

    // cached value
    value_type cached;

    // now one by one get the minimum/maximum element
    // from min heap and replace it with next
    // element of its array
    while (!queue.empty())
    {
        entry_t e = queue.back();
        queue.pop_back();

        // here is a tricky part:
        // check if we already put something in
        // output sequence or last written element
        // is less/greater than current
        if (empty || comp(cached, *e)) {
            // cache current element
            cached = *e;
            // write cached element into output sequence
            *out = cached; ++out;
            // turn off empty flag
            empty = false;
        }

        ++e;
        if (e) {
            queue.push_back(e);
        }
        utility::__small_sort(queue.begin(), queue.end());
    }
    return out;
}

// k-way union

template<class _Container, class _OutIt, class _Comp>
_OutIt kway_union(const std::vector< _Container >& inputs, _OutIt out, _Comp comp)
{
    using namespace utility;

    typedef typename container_traits<_Container>::const_iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;

    typedef entry<iterator, _Comp> entry_t;

    if (inputs.size() < 10) {
        return __kway_union_small(inputs, out, comp);
    }
#if _HAVE_BOOST_LIBS
    if (inputs.size() > 1000) {
        return __kway_union_large(inputs, out, comp);
    }
#endif
    // create min/max heap
    std::priority_queue< entry_t, std::vector<entry_t> > queue;

    // push all first elements of each range
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        auto first = utility::begin(*it);
        auto last  = utility::end(*it);
        if (first != last)
            queue.emplace(first, last);
    }

    // flag that is set when
    // none of elements was written
    // into output sequence
    bool empty = true;

    // cached value
    value_type cached;

    // now one by one get the minimum/maximum element
    // from min heap and replace it with next
    // element of its array
    while (!queue.empty())
    {
        entry_t e = queue.top();
        queue.pop();

        // here is a tricky part:
        // check if we already put something in
        // output sequence or last written element
        // is less/greater than current
        if (empty || comp(cached, *e)) {
            // cache current element
            cached = *e;
            // write cached element into output sequence
            *out = cached; ++out;
            // turn off empty flag
            empty = false;
        }

        ++e;
        if (e) {
            queue.push(e);
        }
    }
    return out;
}

template<class _Container, class _OutIt>
_OutIt kway_union(const std::vector< _Container >& inputs, _OutIt out) {
    return kway_union(inputs, out, std::less<>{});
}

_STDX_END
