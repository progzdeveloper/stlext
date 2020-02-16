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

_STDX_BEGIN

/*!
 * \fn partition_point ( ForwardIterator first, ForwardIterator last, Predicate p )
 * \brief Given a partitioned range, returns the partition point, i.e, the first element 
 *     that does not satisfy p
 * \tparam _FwdIt  models forward iterator
 * \tparam _Pred   models unary predicate
 * \param first    The start of the input sequence
 * \param last     One past the end of the input sequence
 * \param p        The predicate to test the values with
 * \note           This function is part of the C++2011 standard library.
 */
template <class _FwdIt, class _Pred>
_FwdIt partition_point (_FwdIt first, _FwdIt last, _Pred p )
{
    std::size_t dist = std::distance ( first, last );
    while ( first != last ) {
        std::size_t d2 = dist >> 1; // dist / 2
        _FwdIt ret_val = first;
        std::advance (ret_val, d2);
        if (p (*ret_val)) {
            first = ++ret_val;
            dist -= d2 + 1;
        }
        else {
            last = ret_val;
            dist = d2;
        }
    }
    return first;
}

_STDX_END
