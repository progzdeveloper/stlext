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

#include <utility>      // for std::pair

_STDX_BEGIN
/*!
 * \fn mismatch ( InputIterator1 first1, InputIterator1 last1, 
 *                InputIterator2 first2, InputIterator2 last2,
 *                BinaryPredicate pred )
 * \return a pair of iterators pointing to the first elements in the sequence that do not match
 * 
 * \param first1    The start of the first range.
 * \param last1     One past the end of the first range.
 * \param first2    The start of the second range.
 * \param last2     One past the end of the second range.
 * \param pred      A predicate for comparing the elements of the ranges
 */
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
std::pair<InputIterator1, InputIterator2> mismatch (
                    InputIterator1 first1, InputIterator1 last1,
                    InputIterator2 first2, InputIterator2 last2,
                    BinaryPredicate pred )
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        if ( !pred ( *first1, *first2 ))
            break;
    return std::pair<InputIterator1, InputIterator2>(first1, first2);
}

/*!
 * \fn mismatch ( InputIterator1 first1, InputIterator1 last1, 
 *                InputIterator2 first2, InputIterator2 last2 )
 * \return a pair of iterators pointing to the first elements in the sequence that do not match
 * 
 * \param first1    The start of the first range.
 * \param last1     One past the end of the first range.
 * \param first2    The start of the second range.
 * \param last2     One past the end of the second range.
 */
template <class InputIterator1, class InputIterator2>
std::pair<InputIterator1, InputIterator2> mismatch (
                    InputIterator1 first1, InputIterator1 last1,
                    InputIterator2 first2, InputIterator2 last2 )
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        if ( *first1 != *first2 )
            break;
    return std::pair<InputIterator1, InputIterator2>(first1, first2);
}

_STDX_END
