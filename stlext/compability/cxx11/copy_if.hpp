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
#include <utility>
#include "../../platform/common.h"

_STDX_BEGIN

/*!
 * \fn copy_if ( InputIterator first, InputIterator last, OutputIterator result, Predicate p )
 * \brief Copies all the elements from the input range that satisfy the
 * predicate to the output range.
 * \return The updated output iterator
 * \tparam _InIt  models input iterator
 * \tparam _OutIt models output iterator
 * \tparam _Pred  models unary predicate
 * \param first    The start of the input sequence
 * \param last     One past the end of the input sequence
 * \param result   An output iterator to write the results into
 * \param p        A predicate for testing the elements of the range
 * \note           This function is part of the C++2011 standard library.
 *  We will use the standard one if it is available, 
 *  otherwise we have our own implementation.
 */
template<class _InIt, class _OutIt, class _Pred>
_OutIt copy_if (_InIt first, _InIt last, _OutIt result, _Pred p )
{
    for ( ; first != last; ++first )
        if (p(*first))
            *result++ = *first;
    return result;
}


/*! 
 * \fn copy_while ( InputIterator first, InputIterator last, OutputIterator result, Predicate p )
 * \brief Copies all the elements at the start of the input range that
 *     satisfy the predicate to the output range.
 * \return The updated input and output iterators packed into std::pair<>
 * \tparam _InIt  models input iterator
 * \tparam _OutIt models output iterator
 * \tparam _Pred  models unary predicate
 * \param first    The start of the input sequence
 * \param last     One past the end of the input sequence
 * \param result   An output iterator to write the results into
 * \param p        A predicate for testing the elements of the range
 */
template<class _InIt, class _OutIt, class _Pred>
std::pair<_InIt, _OutIt>
copy_while (_InIt first, _InIt last, _OutIt result, _Pred p )
{
    for ( ; first != last && p(*first); ++first )
        *result++ = *first;
    return std::make_pair(first, result);
}


/*!
 * \fn copy_until ( InputIterator first, InputIterator last, OutputIterator result, Predicate p )
 * \brief Copies all the elements at the start of the input range that do not
 *     satisfy the predicate to the output range.
 * \return The updated input and output iterators packed into std::pair<>
 * \tparam _InIt  models input iterator
 * \tparam _OutIt models output iterator
 * \tparam _Pred  models unary predicate
 * \param first    The start of the input sequence
 * \param last     One past the end of the input sequence
 * \param result   An output iterator to write the results into
 * \param p        A predicate for testing the elements of the range
 */
template<class _InIt, class _OutIt, class _Pred>
std::pair<_InIt, _OutIt>
copy_until (_InIt first, _InIt last, _OutIt result, _Pred p )
{
    for ( ; first != last && !p(*first); ++first )
        *result++ = *first;
    return std::make_pair(first, result);
}

_STDX_END

