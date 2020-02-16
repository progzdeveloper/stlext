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
#include <utility>  // for std::pair

#include "../../platform/common.h"

_STDX_BEGIN

/*!
 * \fn partition_copy ( InputIterator first, InputIterator last,
 *     OutputIterator1 out_true, OutputIterator2 out_false, UnaryPredicate p )
 * \brief Copies the elements that satisfy the predicate p from the range [first, last) 
 *     to the range beginning at d_first_true, and
 *     copies the elements that do not satisfy p to the range beginning at d_first_false.
 *
 * \tparam _InIt   models input iterator
 * \tparam _OutIt1 models output iterator
 * \tparam _OutIt2 models output iterator
 * \tparam _Pred   models unary predicate
 * \param first     The start of the input sequence
 * \param last      One past the end of the input sequence
 * \param out_true  An output iterator to write the elements that satisfy the predicate into
 * \param out_false An output iterator to write the elements that do not satisfy the predicate into
 * \param p         A predicate for dividing the elements of the input sequence.
 *
 * \note            This function is part of the C++2011 standard library.
 */
template <
	class _InIt,
	class _OutIt1,
	class _OutIt2,
	class _Pred
>
std::pair<_OutIt1, _OutIt2>
		partition_copy(_InIt first, _InIt last,
					   _OutIt1 out_true, _OutIt2 out_false,
					   _Pred p)
{
	for (; first != last; ++first) {
		if (p(*first))
			*out_true++ = *first;
		else
			*out_false++ = *first;
	}
	return std::pair<_OutIt1, _OutIt2>(out_true, out_false);
}

_STDX_END
