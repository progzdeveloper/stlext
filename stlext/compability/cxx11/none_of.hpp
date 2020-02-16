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
 * \fn none_of ( InputIterator first, InputIterator last, Predicate p )
 * \return true if none of the elements in [first, last) satisfy the predicate 'p'
 * \note returns true on an empty range
 * \tparam _InIt models input iterator
 * \tparam _Pred models unary predicate
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param p     A predicate for testing the elements of the sequence
 */
template<class _InIt, class _Pred>
bool none_of (_InIt first, _InIt last, _Pred p )
{
for ( ; first != last; ++first )
    if ( p(*first)) 
        return false;
    return true;
} 

/*!
 * \fn none_of_equal ( InputIterator first, InputIterator last, const V &val )
 * \return true if none of the elements in [first, last) are equal to 'val'
 * \note returns true on an empty range
 * \tparam _InIt models input iterator
 * \tparam _Tx   models value type
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param val   A value to compare against
 */
template<class _InIt, class _Tx>
bool none_of_equal (_InIt first, _InIt last, const _Tx &val )
{
    for ( ; first != last; ++first )
        if ( val == *first )
            return false;
    return true; 
} 

_STDX_END

