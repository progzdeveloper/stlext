// Copyright (c) 2010-2020, Michael Polukarov (Russia).
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

#include <iterator>
#include <deque>

#include "../platform/common.h"

_STDX_BEGIN

template<class _InIt>
class zigzag_iterator
{
public:
    typedef std::pair<_InIt, _InIt> iterator_range;

    typedef std::forward_iterator_tag iterator_category;
    typedef typename std::iterator_traits<_InIt>::difference_type difference_type;

    typedef typename std::iterator_traits<_InIt>::value_type value_type;

    typedef typename std::iterator_traits<_InIt>::reference reference;
    typedef typename std::iterator_traits<_InIt>::pointer pointer;

    typedef const reference const_reference;
    typedef const pointer const_pointer;


    zigzag_iterator() {}

    zigzag_iterator(std::initializer_list< iterator_range > ranges)
    {
        for (auto first = ranges.begin(); first != ranges.end(); ++first) {
            if (first->first != first->second)
                __m_iters.emplace_back(*first);
        }
    }

    zigzag_iterator& operator++() {
        next();
        return (*this);
    }

    const_reference operator*() const {
        return *(__m_iters.front().first);
    }

    reference operator*() {
        return *(__m_iters.front().first);
    }

    const_reference operator->() const {
        return __m_iters.front().first.operator->();
    }

    reference operator->() {
        return __m_iters.front().first.operator->();
    }

    _InIt base() { return __m_iters.front().first; }


    friend inline bool operator==(const zigzag_iterator& lhs, const zigzag_iterator& rhs)
    {
        size_t n = lhs.__m_iters.size();
        if (n != rhs.__m_iters.size())
            return false; // sizes of queues differ

        if (n == 0) // size of both queues is 0
            return true;

        if (lhs.__m_iters.front().first != rhs.__m_iters.front().first)
            return false; // front elements are not equal

        // make full scan
        return std::equal(lhs.__m_iters.begin(), lhs.__m_iters.end(), rhs.__m_iters.begin());
    }

    friend inline bool operator!=(const zigzag_iterator& lhs, const zigzag_iterator& rhs) {
        return !(lhs == rhs);
    }

private:
    void next()
    {
        if (__m_iters.empty()) {
            return;
        }
        auto r = __m_iters.front();
        __m_iters.pop_front(); // pop front itrator range from queue
        if (++r.first == r.second)
            return; // one of iterators is at end
        __m_iters.push_back(r); // put iterator range back into queue
    }
private:
    std::deque<iterator_range> __m_iters;
};

_STDX_END
