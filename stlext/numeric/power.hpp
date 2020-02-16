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

#include "../platform/common.h"
#include "identity_element.hpp"

_STDX_BEGIN


// Non-standard
/*!
 * \brief power() function
 * \tparam _Tp type of value
 * \tparam _Integer  type of integer-like type
 * \tparam _MonoidOp type of monoid operation
 * \param x value
 * \param n power
 * \param op monoid operation
 * \return x ** n, where n >= 0.
 * \note _MonoidOp models monoid operation.
 * \note Note that "multiplication" is requered to be associative,
 * but not nessesarily commutative.
 * \note This function is C++ SGI extension and not part of the standard.
 * \remark Taken from Knuth, The Art of Computer Programming, Volume 2:
 * Seminumerical Algorithms, Section 4.6.3
 */
template<typename _Tp, typename _Integer, typename _MonoidOp>
_Tp power(_Tp x, _Integer n, _MonoidOp op)
{
	if (n == 0)
		return typename identity_element<_MonoidOp>::get();

	while ((n & 1) == 0)
	{
		n >>= 1;
		x = op(x, x);
	}

	_Tp result = x;
	n >>= 1;
	while (n != 0)
	{
		x = op(x, x);
		if ((n & 1) != 0) {
			result = op(result, x);
			n >>= 1;
		}
	}
	return result;
}


// Non-standard
/*!
 * \brief power() function with multiplies<T> monoid operation
 * \tparam _Tp type of value
 * \tparam _Integer  type of integer-like type
 * \param x value
 * \param n power
 * \return x ** n, where n >= 0.
 * \note Note that "multiplication" is requered to be associative,
 * but not nessesarily commutative.
 * \note This function is C++ SGI extension and not part of the standard.
*/
template<typename _Tp, typename _Integer>
inline _Tp power(_Tp x, _Integer n) {
	return power(x, n, std::multiplies<_Tp>());
}

_STDX_END

