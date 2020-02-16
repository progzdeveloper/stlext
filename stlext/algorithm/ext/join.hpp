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

/*!
 * \fn join(_InIt first, _InIt last, _OutIt out)
 * \brief join elements in [first, last) range into out
 *
 * \tparam _InIt  models input iterator
 * \tparam _OutIt models output iterator
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param out   An output iterator to write the elements
 * \return output iterator pointing to the one past last element
 */
template<class _InIt, class _OutIt>
_OutIt join(_InIt first, _InIt last, _OutIt out)
{
	using namespace std;
	for (; first != last; ++first) {
		out = copy(begin(*first), end(*first), out);
	}
	return out;
}

/*! 
 * \fn join(_InIt first, _InIt last, _OutIt out, const _Delim& delim)
 * \brief join elements in [first, last) range into out using delimiter \a delim
 * 
 * \tparam _InIt  models input iterator
 * \tparam _OutIt models output iterator
 * \tparam _Delim models delimiter type
 *
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param out   An output iterator to write the elements
 * \return output iterator pointing to the one past last element
 */
template<class _InIt, class _OutIt, class _Delim>
_OutIt join(_InIt first, _InIt last, _OutIt out, const _Delim& delim)
{
	using namespace std;
	while (first != last)
	{
		out = copy(begin(*first), end(*first), out);
		if (++first != last) {
			*out = delim;
			++out;
		}
	}
	return out;
}

_STDX_END

