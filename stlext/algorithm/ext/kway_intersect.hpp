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
#include <stack>

#ifdef _HAVE_BOOST_LIBS
#include <boost/heap/pairing_heap.hpp>
#endif

#include "../../platform/common.h"

#include "kway_utility.hpp"

_STDX_BEGIN



// optimized for small number of input arrays
template<class _Container, class _OutIt, class _Comp>
_OutIt __kway_intersect_small(const std::vector< _Container >& inputs, _OutIt out, _Comp comp)
{
    using namespace utility;

    typedef typename container_traits<_Container>::const_iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;

    typedef entry<iterator, _Comp> entry_t;

    // create min/max heap
    std::vector<entry_t> queue;
    queue.reserve(inputs.size());

    // push all first elements of each range
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        auto first = utility::begin(*it);
        auto last  = utility::end(*it);
        if (first != last) { // escape empty ranges
            queue.emplace_back(first, last);
        }
    }
    utility::__small_sort(queue.begin(), queue.end());

    size_t n = queue.size();

    // flag that is set when
    // none of elements was written
    // into output sequence
    bool empty = true;

    // memorized value
    value_type memorized;

    auto top = queue.end() - 1;
    auto bottom = queue.begin() - 1;
    while (top != bottom)
    {
        entry_t e = *top; // take the topmost range from min/max heap
        --top;            // pop out range from queue

        // count can't be zero, since at this point
        // at least two ranges must be present
        size_t count = 1;

        while(top != bottom) // take heads of ranges one-by-one
        {
            entry_t& current = *top;
            --top; // pop out range

            count += (*current == *e); // update counter
            for (; current; ++current) { // traverse through current array
                if (comp(*e, *current))   // until we have equal elements
                    break;
            }
        }

        // check counter along with uniqueness of found element
        if (count == n && (empty || comp(memorized, *e))) {
            // cache current element
            memorized = *e;
            // write cached element into output sequence
            *out = memorized; ++out;
            // turn off empty flag
            empty = false;
        }

        ++queue.back(); // increase the first pointer
        for (auto it = queue.begin(); it != queue.end(); ++it) {
            if (!*it) { // one of the sequences was exhaused
                return out; // bailing out
            }
        }
        // sort to restore heap properties
        utility::__small_sort(queue.begin(), queue.end());
        top = queue.end() - 1;
    }
    return out;
}

#ifdef _HAVE_BOOST_LIBS
// optimized for large number of input arrays
template<class _Container, class _OutIt, class _Comp>
_OutIt __kway_intersect_large(const std::vector< _Container >& inputs, _OutIt out, _Comp comp)
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
        if (first != last) { // escape empty ranges
            queue.emplace(first, last);
        }
    }

    size_t n = queue.size();

    // flag that is set when
    // none of elements was written
    // into output sequence
    bool empty = true;

    // memorized value
    value_type memorized;

    // cache of queue  elements
    // TODO: escape needless memory allocations
    // by using extended boost API
    std::vector<entry_t> cache;
    cache.reserve(queue.size());

    while (!queue.empty())
    {
        entry_t e = queue.top(); // take the topmost range from min/max heap
        cache.push_back(e);      // push it to cache
        queue.pop();             // pop out from queue

        // count can't be zero, since at this point
        // at least two ranges must be present
        size_t count = 1;

        while(!queue.empty()) // take heads of ranges one-by-one
        {
            entry_t current = queue.top();
            queue.pop();

            count += (*current == *e); // update counter
            for (; current; ++current) { // traverse through current array
                if (comp(*e, *current))   // until we have equal elements
                    break;
            }
            cache.push_back(current); // push current range to cache
        }

        // check counter along with uniqueness of found element
        if (count == n && (empty || comp(memorized, *e))) {
            // cache current element
            memorized = *e;
            // write cached element into output sequence
            *out = memorized; ++out;
            // turn off empty flag
            empty = false;
        }

        ++cache.front(); // increase the first pointer
        for (auto it = cache.begin(); it != cache.end(); ++it) {
            if (!*it) { // one of the sequences was exhaused
                return out; // bailing out
            }
            queue.push(*it); // put ranges back to queue
        }
        cache.clear(); // clear cache
    }
    return out;

}

#endif // _HAVE_BOOST_LIBS


// k-way intersect

template<class _Container, class _OutIt, class _Comp>
_OutIt kway_intersect(const std::vector< _Container >& inputs, _OutIt out, _Comp comp)
{
    using namespace utility;

    typedef typename container_traits<_Container>::const_iterator iterator;
    typedef typename std::iterator_traits<iterator>::value_type value_type;

    typedef entry<iterator, _Comp> entry_t;

    size_t n = 0; // count non-empty ranges
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        auto first = utility::begin(*it);
        auto last  = utility::end(*it);
        n += (first != last); // escape empty ranges
    }

    if (n < 2) { // empty set or singletone set can't be intersected
        return out;
    }

    if (n < 10) { // number of ranges are small - apply optimized version
         return __kway_intersect_small(inputs, out, comp);
    }

#if _HAVE_BOOST_LIBS
     if (n > 1000) { // number of ranges are huge - apply optimized version
         return __kway_intersect_large(inputs, out, comp);
     }
#endif

    // number of ranges between 10 and 1000 -
    // make use of basic version


    // create min/max heap
    std::priority_queue< entry_t, std::vector<entry_t> > queue;

    // push all first elements of each range
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        auto first = utility::begin(*it);
        auto last  = utility::end(*it);
        if (first != last) { // escape empty ranges
            queue.emplace(first, last);
        }
    }


    // flag that is set when
    // none of elements was written
    // into output sequence
    bool empty = true;

    // memorized value
    value_type memorized;

    // cache of queue  elements
    std::vector<entry_t> cache;
    cache.reserve(queue.size());

    while (!queue.empty())
    {
        entry_t e = queue.top(); // take the topmost range from min/max heap
        cache.push_back(e);      // push it to cache
        queue.pop();             // pop out from queue

        // count can't be zero, since at this point
        // at least two ranges must be present
        size_t count = 1;

        while(!queue.empty()) // take heads of ranges one-by-one
        {
            entry_t current = queue.top();
            queue.pop();

            count += (*current == *e); // update counter
            for (; current; ++current) { // traverse through current array
                if (comp(*e, *current))   // until we have equal elements
                    break;
            }
            cache.push_back(current); // push current range to cache
        }

        // check counter along with uniqueness of found element
        if (count == n && (empty || comp(memorized, *e))) {
            // cache current element
            memorized = *e;
            // write cached element into output sequence
            *out = memorized; ++out;
            // turn off empty flag
            empty = false;
        }

        ++cache.front(); // increase the first pointer
        for (auto it = cache.begin(); it != cache.end(); ++it) {
            if (!*it) { // one of the sequences was exhaused
                return out; // bailing out
            }
            queue.push(*it); // put ranges back to queue
        }
        cache.clear(); // clear cache
    }
    return out;
}

template<class _Container, class _OutIt>
_OutIt kway_intersect(const std::vector< _Container >& inputs, _OutIt out) {
    return kway_intersect(inputs, out, std::less<>{});
}

_STDX_END

