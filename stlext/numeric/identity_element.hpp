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
#include <functional>

#include "../platform/common.h"

_STDX_BEGIN


// Non-standard
/*!
\brief identity_element
\tparam _Op type of operation

Template argument of struct identity_element is
an operation, and it's return value is identity
element for that operation.

It's specialized for addition and multiplication
and you can specialize it for your own nefarious operations.

*/
template<typename _Op>
struct identity_element;

/*!
\brief identity_element< std::plus< T > > specialization for addition operation
\see \ref identity_element
*/
template<typename T>
struct identity_element < std::plus<T> >
{
	typedef T result_type;

	static constexpr T value = T(0);

	result_type operator()() const { return get(); }

	static constexpr result_type get() { return T(0); }
};

/*!
\brief identity_element< std::multiplies< T > > specialization for multiplication operation
\see \ref identity_element
*/
template<typename T>
struct identity_element < std::multiplies<T> >
{
	typedef T result_type;

	static constexpr T value = T(1);

	result_type operator()() const { return get(); }

	static constexpr result_type get() { return T(1); }
};

_STDX_END
