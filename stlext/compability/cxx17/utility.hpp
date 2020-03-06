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
#include <iosfwd>
#include <iterator>
#include <type_traits>
#include <initializer_list>

#include "../../platform/common.h"

#include "../integer_sequence"

#if (defined(STDX_CMPLR_MSVC) && STDX_CMPLR_MSVC <= 1800) || \
    (defined(STDX_CMPLR_GNU) && (STDX_CMPLR_GNU < 600)) || \
    (defined(STDX_CMPLR_INTEL) && STDX_CMPLR_INTEL <= 1800)

_STDX_BEGIN

template<class _Container>
__CONSTEXPR auto inline data(_Container& c)
-> decltype(c.data())
{	// get data() for container
    return (c.data());
}

template<class _Container>
__CONSTEXPR auto inline data(const _Container& c)
-> decltype(c.data())
{	// get pointer to data of const container
    return (c.data());
}

template<class _Ty, size_t _Size>
inline __CONSTEXPR _Ty *data(_Ty(&arr)[_Size])
{	// get pointer to data of array
    return (arr);
}

template<class _Elem> inline
__CONSTEXPR const _Elem *data(std::initializer_list<_Elem> ilist)
{	// get pointer to data of initializer_list
    return (ilist.begin());
}

/// C++17 proposal
template<class C>
__CONSTEXPR auto size(const C& c) -> decltype(c.size()) {
    return c.size();
}

/// C++17 proposal
template<typename T, size_t N>
__CONSTEXPR size_t size(const T(&)[N]) {
	return N;
}


/// Non-standard
template<class _Ty, _Ty... _Vals>
__CONSTEXPR size_t size(const std::integer_sequence<_Ty, _Vals...>&& __seq) {
	return __seq.size();
}



/// C++17 proposal
template<class _Container>
__CONSTEXPR auto empty(const _Container& __c) -> decltype(__c.empty()) {
	return __c.empty();
}

/// C++17 proposal
template<typename T, size_t N>
__CONSTEXPR bool empty(const T(&)[N]) {
	return false;
}

/// C++17 proposal
template<typename T>
__CONSTEXPR bool empty(const std::initializer_list<T>& __il) {
	return (__il.size() == 0);
}

/// Non-standard
template<class _Ty, _Ty... _Vals>
__CONSTEXPR size_t empty(const std::integer_sequence<_Ty, _Vals...>&& __seq) {
	return (__seq.size() == 0);
}


/// Non-standard
template<class _Tx, class _Elem, class _Traits>
std::istream_iterator<_Tx, _Elem, _Traits> begin(std::basic_istream<_Elem, _Traits>& __stream) {
	return std::istream_iterator<_Tx, _Elem, _Traits>(__stream);
}

/// Non-standard
template<class _Tx, class _Elem, class _Traits>
std::istream_iterator<_Tx, _Elem, _Traits> end(std::basic_istream<_Elem, _Traits>&) {
	return std::istream_iterator<_Tx, _Elem, _Traits>();
}

_STDX_END


// expose to std namespace
namespace std {
	using stdx::data;
	using stdx::size;
	using stdx::empty;
}


#elif defined(STDX_CMPLR_MSVC) && STDX_CMPLR_MSVC < 1800 
_STDX_BEGIN

using std::begin;
using std::cbegin;

using std::rbegin;
using std::crbegin;

using std::end;
using std::cend;

using std::rend;
using std::crend;

using std::data;
using std::size;
using std::empty;

/// EXPERIMENTAL: non-standard
template<class _Ty, _Ty... _Vals>
__CONSTEXPR size_t empty(const std::integer_sequence<_Ty, _Vals...>&& __seq) {
	return (__seq.size() == 0);
}

/// EXPERIMENTAL: non-standard
template<class _Ty, _Ty... _Vals>
__CONSTEXPR size_t size(const std::integer_sequence<_Ty, _Vals...>&& __seq) {
	return __seq.size();
}


/// EXPERIMENTAL: non-standard
template<class _Tx, class _Elem, class _Traits>
std::istream_iterator<_Tx, _Elem, _Traits> begin(std::basic_istream<_Elem, _Traits>& __stream) {
	return std::istream_iterator<_Tx, _Elem, _Traits>(__stream);
}

/// EXPERIMENTAL: non-standard
template<class _Tx, class _Elem, class _Traits>
std::istream_iterator<_Tx, _Elem, _Traits> end(std::basic_istream<_Elem, _Traits>&) {
	return std::istream_iterator<_Tx, _Elem, _Traits>();
}

_STDX_END
#else

#endif



