// Copyright (c) 2016-2020, Michael Polukarov (Russia).
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

#include "../platform/common.h"


_STDX_BEGIN

// stack with O(1) access to min/max element according to _Comp

template<
    class T,
    class _Comp,
    class _Sequence = std::vector<T>
>
class ordered_stack : _Comp
{
public:
    typedef typename _Sequence::value_type                value_type;
    typedef typename _Sequence::reference                 reference;
    typedef typename _Sequence::const_reference           const_reference;
    typedef typename _Sequence::size_type                 size_type;
    typedef          _Sequence                            container_type;

    explicit ordered_stack(const _Sequence& c)
    {
        for (auto it = std::begin(c); it != std::end(c); ++it) {
            push(*it);
        }
    }

    explicit ordered_stack(_Sequence&& c = _Sequence())
    {
        for (auto it = std::begin(c); it != std::end(c); ++it) {
            push(*it);
        }
    }

    void push(const_reference x)
    {
        // if main stack is empty - push onto both stacks
        if (__m_stack.empty()) {
            __m_stack.emplace_back(x);
            __m_track.emplace_back(x);
        } else {
            // push the minimal into tracking stack
            __m_track.emplace_back((std::min)(__m_track.back(), x, static_cast<_Comp&>(*this)));
            __m_stack.emplace_back(x); // insert it into main stack.
        }
    }

    template<class... _Args>
    void emplace(_Args&&... args) {
        // if main stack is empty - emplace onto both stacks
        if (__m_stack.empty()) {
            __m_stack.emplace_back(std::forward<_Args>(args)...);
            __m_track.emplace_back(__m_stack.back());
        } else {
            // emplace element into main stack.
           __m_stack.emplace_back(std::forward<_Args>(args)...);
            // push the minimal into tracking stack
            __m_track.emplace_back((std::min)(__m_track.back(), __m_stack.back(), static_cast<_Comp&>(*this)));
        }

    }

    void pop()
    {
        // pop the top elements
        if (!__m_stack.empty()) {
            __m_track.pop_back();
            __m_stack.pop_back();
        }
    }

    const_reference top() const {
        return __m_stack.back();
    }

    const_reference order_element() const {
        return __m_track.back();
    }

    bool empty() const {
        return __m_stack.empty();
    }

    size_type size() const {
        return __m_stack.size();
    }

protected:
    _Sequence __m_stack;
    _Sequence __m_track;
};

template<class T>
using max_stack = ordered_stack< T, std::greater<T> >;


template<class T>
using min_stack = ordered_stack< T, std::less<T> >;

_STDX_END

