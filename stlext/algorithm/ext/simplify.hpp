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
#include <iterator>


/*!
 * \brief simplify simplifies input range by removing consequentive elements that satisfy the
 * predicated and replace them by a single replacement element
 * \tparam _BiIt models bidirectional iterator
 * \tparam _OutIt models output iterator
 * \tparam _Pred models unary predicate
 * \tparam _Elem models single element
 * \param first iterator pointing to first element of input sequence
 * \param last iterator pointing to one-past-last element of input sequence
 * \param out iterator pointing to the begining of output
 * \param pr predicate
 * \param e replacement element
 * \return output iterator
 */
template<
    class _BiIt,
    class _OutIt,
    class _Pred,
    class _Elem
>
_OutIt simplify(_BiIt first, _BiIt last, _OutIt out, _Pred pr, _Elem e)
{
    using namespace std;

    // the most portable way to negate predicate
    auto not_pr = [&pr](char c) { return !pr(c); };

    // find first non-escaped element in sequence
    first = find_if(first, last, not_pr);
    if (first == last) {
        return (out);
    }

    // find last non-escaped element in sequence
    last = find_if(make_reverse_iterator(last),
                   make_reverse_iterator(first), not_pr).base();

    size_t count = 0; // counter of escaped elements
    for (; first != last; ++first)
    {
        // check if we got non-escaped element
        if (!pr(*first)) {
            count = 0; // reset counter
            *out = *first; ++out; // copy element
            continue; // continue the scan loop
        }

        // otherwise we got unwanted element
        if (count < 1) { // there is multiply occuriences of escaped element(s)
            *out = e; ++out; // copy only one replacement elemnt
        }
        ++count; // increase counter of escaped elements
    }
    return (out);
}


