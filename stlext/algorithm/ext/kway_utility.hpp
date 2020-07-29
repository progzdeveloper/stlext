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

#include <algorithm>
#include <utility>

#include "../../platform/common.h"
#include "../../functional/symmetric_operation.hpp"

_STDX_BEGIN

namespace utility
{
    using std::begin;
    using std::end;

    template<class _It>
    _It begin(std::pair<_It, _It>& p) { return p.first; }

    template<class _It>
    _It begin(const std::pair<_It, _It>& p) { return p.first; }

    template<class _It>
    _It end(std::pair<_It, _It>& p) { return p.second; }

    template<class _It>
    _It end(const std::pair<_It, _It>& p) { return p.second; }


    template<class _Container>
    struct container_traits {
        typedef typename _Container::iterator iterator;
        typedef typename _Container::const_iterator const_iterator;

    };

    template<class _It>
    struct container_traits< std::pair<_It, _It> > {
        typedef _It iterator;
        typedef _It const_iterator;
    };


    // sort

    // stable, 2-3 compares, 0-2 swaps

    template <class _Compare, class _ForwardIterator>
    inline unsigned __sort3(_ForwardIterator __x,
                            _ForwardIterator __y,
                            _ForwardIterator __z,
                            _Compare __c)
    {
        using std::swap;
        unsigned __r = 0;
        if (!__c(*__y, *__x))          // if x <= y
        {
            if (!__c(*__z, *__y))      // if y <= z
                return __r;            // x <= y && y <= z
                                       // x <= y && y > z
            swap(*__y, *__z);          // x <= z && y < z
            __r = 1;
            if (__c(*__y, *__x))       // if x > y
            {
                swap(*__x, *__y);      // x < y && y <= z
                __r = 2;
            }
            return __r;                // x <= y && y < z
        }
        if (__c(*__z, *__y))           // x > y, if y > z
        {
            swap(*__x, *__z);          // x < y && y < z
            __r = 1;
            return __r;
        }
        swap(*__x, *__y);              // x > y && y <= z
        __r = 1;                       // x < y && x <= z
        if (__c(*__z, *__y))           // if y > z
        {
            swap(*__y, *__z);          // x <= y && y < z
            __r = 2;
        }
        return __r;
    }                                  // x <= y && y <= z

    // stable, 3-6 compares, 0-5 swaps

    template <class _Compare, class _ForwardIterator>
    inline unsigned __sort4(_ForwardIterator __x1, _ForwardIterator __x2,
                            _ForwardIterator __x3, _ForwardIterator __x4,
                            _Compare __c)
    {
        using std::swap;
        unsigned __r = __sort3<_Compare>(__x1, __x2, __x3, __c);
        if (__c(*__x4, *__x3))
        {
            swap(*__x3, *__x4);
            ++__r;
            if (__c(*__x3, *__x2))
            {
                swap(*__x2, *__x3);
                ++__r;
                if (__c(*__x2, *__x1))
                {
                    swap(*__x1, *__x2);
                    ++__r;
                }
            }
        }
        return __r;
    }

    // stable, 4-10 compares, 0-9 swaps

    template <class _Compare, class _ForwardIterator>
    inline unsigned __sort5(_ForwardIterator __x1, _ForwardIterator __x2, _ForwardIterator __x3,
                            _ForwardIterator __x4, _ForwardIterator __x5, _Compare __c)
    {
        using std::swap;
        unsigned __r = __sort4<_Compare>(__x1, __x2, __x3, __x4, __c);
        if (__c(*__x5, *__x4))
        {
            swap(*__x4, *__x5);
            ++__r;
            if (__c(*__x4, *__x3))
            {
                swap(*__x3, *__x4);
                ++__r;
                if (__c(*__x3, *__x2))
                {
                    swap(*__x2, *__x3);
                    ++__r;
                    if (__c(*__x2, *__x1))
                    {
                        swap(*__x1, *__x2);
                        ++__r;
                    }
                }
            }
        }
        return __r;
    }

    template<class _RanIt, class _Compare>
    void __small_sort(_RanIt first, _RanIt last, _Compare c)
    {
        auto n = last - first;
        switch (n)
        {
        case 0:
        case 1:
            return;
        case 2:
            if (c(*--last, *first))
                std::swap(*first, *last);
            return;
        case 3:
            __sort3<_Compare>(first, first+1, --last, c);
            return;
        case 4:
            __sort4<_Compare>(first, first+1, first+2, --last, c);
            return;
        case 5:
            __sort5<_Compare>(first, first+1, first+2, first+3, --last, c);
            return;
        }
        std::sort(first, last, c);
    }

    template<class _RanIt>
    inline void __small_sort(_RanIt first, _RanIt last)
    {
        __small_sort(first, last, std::less<>{});
    }

}

template<
    class _Comp,
    bool _IsEmpty = std::is_empty<_Comp>::value
>
struct entry_ebo
{
    entry_ebo() {}
    entry_ebo(const _Comp& c) : comp(c) {}
    template<class _Lhs, class _Rhs>
    bool operator()(const _Lhs& lhs, const _Rhs& rhs) const {
        return (comp)(lhs, rhs);
    }
    _Comp comp;
};

template<class _Comp>
struct entry_ebo<_Comp, true> : public _Comp
{
    entry_ebo() {}
    entry_ebo(const _Comp& c) : _Comp(c) {}
};

// Helper struct to make it more convenient
template< class _InIt, class _Comp >
struct entry : public entry_ebo< typename symmetric_operation<_Comp>::type >
{
    typedef entry_ebo< _Comp > base_type;

    typedef _InIt iterator;
    typedef _InIt const_iterator;
    typedef typename std::iterator_traits<_InIt>::reference reference;

    entry(_InIt first, _InIt last) :
        it(first), eod(last) {}

    entry(_InIt first, _InIt last, const _Comp& c) :
        base_type(c), it(first), eod(last) {}

    entry(const std::pair<_InIt, _InIt>& range) :
        it(range.first), eod(range.second) {}

    entry(const std::pair<_InIt, _InIt>& range, const _Comp& c) :
        base_type(c), it(range.first), eod(range.second) {}

    const_iterator begin() const { return it; }
    const_iterator end() const { return eod; }

    reference operator*() { return (*it); }
    entry& operator++() { ++it; return (*this); }
    operator bool () const { return (it != eod); }
    bool operator< (entry const& rhs) const { return (bool)(*this)(*it, *rhs.it); }

private:
    _InIt it;  // current position
    _InIt eod; // end-of-data
};

_STDX_END
