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

#include "../../platform/common.h"

_STDX_BEGIN

/*!
 * \brief simplify simplifies input range by removing consecutive elements that satisfy
 * the predicate and replace them by a single replacement element
 * \tparam _BiIt models input iterator
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
    class _InIt,
    class _OutIt,
    class _Pred,
    class _Elem
>
_OutIt simplify(_InIt first, _InIt last, _OutIt out, _Pred pr, _Elem e)
{
    using namespace std;

    // the most portable way to negate predicate
    auto not_pr = [&pr](char c) { return !pr(c); };

    bool flag = true; // is first element or not
    for (; first != last; )
    {
        if (!pr(*first)) { // if non-escaped element
            *out = *first; ++out; // copy element to output
            ++first; // move to next element
            flag = false; // clear flag
            continue;
        }

        if (flag) { // first escaped element must NOT be writed to output
            if (++first == last) { // move to next element
                break; // end-of-data: bailng out
            }
            flag = false; // clear flag
            first = find_if(first, last, not_pr); // find next non-escaped element
        } else { // i-th element
            first = find_if(++first, last, not_pr); // find next non-escaped element
            if (first == last) {
                break; // end-of-data: bailng out
            }
            *out = e; ++out; // write replaced element to output
        }
    }
    return (out);
}

_STDX_END

