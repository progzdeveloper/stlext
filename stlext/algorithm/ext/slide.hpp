// Copyright (c) 2016, Michael Polukarov (Russia).
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


/// Non-standard
/*!
 * \fn slide_apply(_It first, _It last, size_t width, _Fn2 op, bool strict)
 * \brief slide_apply Apply target function to subrange of elements [first, last);
 * subrange is selected by sliding window with width \a width.
 * 
 * \tparam _It  models input iterator
 * \tparam _Fn2 models binary function
 * 
 * \param first   The start of the input sequence
 * \param last    One past the end of the input sequence
 * \param width   width of sliding window
 * \param op      binary function to be applied for pair of iterators
 * \param strict  flag that disable processing less elements than size of window
 * \return The updated output iterator
 */
template<class _It, class _Fn2>
inline _Fn2 slide_apply(_It first, _It last, size_t width, _Fn2 op, bool strict = true)
{
	using namespace std;

	if (distance(first, last) < ptrdiff_t(width)) {
		if (!strict) op(first, last);
		return move(op);
	}

	_It it = first;
	advance(it, width);
	for (; it != last;) {
		op(first, it);
		advance(first, 1);
		advance(it, 1);
	}
	op(first, it);
	return move(op);
}

/// Non-standard
/*!
 * \fn slide_move (_FwdIt first, _FwdIt last, _FwdIt pos)
 * \brief Rearrange elements in range [first, last) by partition point pos
 *
 * \tparam _FwdIt models forward iterator
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param pos   pivoting position
 * \return pair of iterators pointing to begin and one past end of resulting sequence
 */
template<class _FwdIt>
inline std::pair<_FwdIt, _FwdIt> slide_move(_FwdIt first, _FwdIt last, _FwdIt pos)
{
	using namespace std;
	if (pos < first) return{ pos, rotate(pos, first, last) };
	if (last < pos) return{ rotate(first, last, pos), pos };
	return{ first, last };
}

_STDX_END
