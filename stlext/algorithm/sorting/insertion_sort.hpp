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
#include <iterator>
#include <functional>
#include "../../platform/common.h"

_STDX_BEGIN

/// Non-standard
/*!
 * \fn insertion_sort(_FwdIt first, _FwdIt last)
 * \brief Perform a insertion sort at range [first, last).
 *
 * \tparam _FwdIt models forward iterator
 * \tparam _Pr    models projection unary function
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param pred  A binary predicate that returns true if two elements are ordered.
 */
template<typename _FwdIt, typename _Pr>
inline void insertion_sort(_FwdIt first, _FwdIt last, _Pr pred)
{
	using namespace std;
	for (_FwdIt it = first; it != last; ++it)
		rotate(upper_bound(first, it, *it, pred), it, next(it));
}


/// Non-standard
/*!
 * \fn insertion_sort(_FwdIt first, _FwdIt last)
 * \brief Perform a insertion sort at range [first, last).
 *
 * \tparam _FwdIt models forward iterator
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 */
template<typename _FwdIt>
inline void insertion_sort(_FwdIt first, _FwdIt last)
{
	typedef typename std::iterator_traits<_FwdIt>::value_type value_type;
	insertion_sort(first, last, std::less<value_type>());
}


_STDX_END
