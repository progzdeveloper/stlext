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
 * \fn iota ( _FwdIt first, _FwdIt last, T value )
 * \brief Generates an increasing sequence of values, and stores them in [first, last)
 * \tparam _FwdIt  models forward iterator
 * \tparam T       models value type
 * \param first    The start of the input sequence
 * \param last     One past the end of the input sequence
 * \param value    The initial value of the sequence to be generated
 * \note           This function is part of the C++2011 standard library.
 */
template <class _FwdIt, class T>
void iota (_FwdIt first, _FwdIt last, T value )
{
    for ( ; first != last; ++first, ++value )
        *first = value;
}

/*!
 * \fn iota_n ( _OutIt out, T value, std::size_t n )
 * \brief Generates an increasing sequence of values, and stores them in the input Range.
 * \tparam _OutIt  models output iterator
 * \tparam T       models value type
 * \param out      An output iterator to write the results into
 * \param value    The initial value of the sequence to be generated
 * \param n        The number of items to write
 */
template <class _OutIt, class T>
_OutIt iota_n (_OutIt out, T value, std::size_t n )
{
    for ( ; n > 0; --n, ++value )
        *out++ = value;

    return out;
}

_STDX_END
