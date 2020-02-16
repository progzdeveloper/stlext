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
#include <iterator>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_set>
#include <bitset>

#include "../../platform/common.h"

_STDX_BEGIN


/*!
* \fn is_unique (_InIt first, _InIt last)
* \brief check whatever all elements in range [first, last) are unique using specified predicate
* \tparam _InIt models input iterator
* \tparam _Pred models binary predicate
* \param first The start of the sequence to be tested.
* \param last  One past the end of the sequence.
* \param p     A binary predicate that returns true if two elements are equal.
* \return true whatever all elements in range [first, last) are unique, otherwise return false
*/
template<class _InIt, class _Hash, class _Pred>
bool is_unique(_InIt first, _InIt last, _Hash hs, _Pred p)
{
	typedef typename std::iterator_traits<_InIt>::value_type value_type;
	std::unordered_set<value_type, _Hash, _Pred> lookup(16, hs, p);
	for (; first != last; ++first) {
		auto it = lookup.find(*first);
		if (it != lookup.end())
			return false;
		lookup.emplace(*first);
	}
	return true;
}


/*!
* \fn is_unique (_InIt first, _InIt last)
* \brief check whatever all elements in range [first, last) are unique using specified predicate
* \tparam _InIt models input iterator
* \tparam _Pred models binary predicate
* \param first The start of the sequence to be tested.
* \param last  One past the end of the sequence.
* \param p     A binary predicate that returns true if two elements are equal.
* \return true whatever all elements in range [first, last) are unique, otherwise return false
*/
template<class _InIt,class _Pred>
inline bool is_unique(_InIt first, _InIt last, _Pred p)
{
	typedef typename std::iterator_traits<_InIt>::value_type value_type;
	typedef typename std::hash<value_type> hasher_type;
	return is_unique(first, last, hasher_type(), p);
}

/*!
 * \fn is_unique (_InIt first, _InIt last)
 * \brief check whatever all elements in range [first, last) are unique
 * \tparam _InIt models input iterator
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence.
 * \return true whatever all elements in range [first, last) are unique, otherwise return false
 */
template<class _InIt>
inline bool is_unique(_InIt first, _InIt last)
{
	typedef typename std::iterator_traits<_InIt>::value_type value_type;
	typedef typename std::hash<value_type> hasher_type;
	typedef typename std::equal_to<value_type> comp_type;
	return is_unique(first, last, hasher_type(), comp_type());
}



///
/// --- OVERLOADS
///
/*!
* \fn is_unique (_InIt first, _InIt last)
* \brief check whatever all chars in range [first, last) are unique using specified predicate
* \tparam _InIt models input iterator
* \tparam _Pred models binary predicate
* \param first The start of the sequence to be tested.
* \param last  One past the end of the sequence.
* \return true whatever all elements in range [first, last) are unique, otherwise return false
*/
template<class _Traits, class _Pred>
inline bool is_unique(std::istreambuf_iterator<char, _Traits> first,
			   std::istreambuf_iterator<char, _Traits> last)
{
	static constexpr size_t CHARSET_SIZE = (1 << (sizeof(char) * CHAR_BIT));

	std::bitset<CHARSET_SIZE> bits;
	for (; first != last; ++first) {
		uint8_t c = static_cast<uint8_t>(*first);
		if (bits.test(c))
			return false;
		bits.set(c, 1);
	}
	return true;
}

/*!
* \fn is_unique (_InIt first, _InIt last)
* \brief check whatever all chars in range [first, last) are unique using specified predicate
* \tparam _InIt models input iterator
* \tparam _Pred models binary predicate
* \param first The start of the sequence to be tested.
* \param last  One past the end of the sequence.
* \return true whatever all elements in range [first, last) are unique, otherwise return false
*/
template<class _Traits, class _Pred>
inline bool is_unique(std::istreambuf_iterator<char16_t, _Traits> first,
			   std::istreambuf_iterator<char16_t, _Traits> last)
{
	static constexpr size_t CHARSET_SIZE = (1 << (sizeof(char16_t) * CHAR_BIT));

	std::bitset<CHARSET_SIZE> bits;
	for (; first != last; ++first) {
        uint16_t c = static_cast<uint16_t>(*first);
		if (bits.test(c))
			return false;
		bits.set(c, 1);
	}
	return true;
}


// On windows wchar_t is 16-bit 
#ifdef STDX_OS_WIN
template<class _Traits, class _Pred>
inline bool is_unique(std::istreambuf_iterator<wchar_t, _Traits> first,
			   std::istreambuf_iterator<wchar_t, _Traits> last)
{
	static constexpr size_t CHARSET_SIZE = (1 << (sizeof(wchar_t) * CHAR_BIT));

	std::bitset<CHARSET_SIZE> bits;
	for (; first != last; ++first) {
		uint16_t c = static_cast<uint16_t>(*first)
		if (bits.test(c))
			return false;
		bits.set(c, 1);
	}
	return true;
}
#endif



///
/// --- FULL SPECIALIZATIONS
///

/*!
 * \fn is_unique (const char* first, const char* last)
 * \brief check whatever all chars in range [first, last) are unique 
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence.
 * \return true whatever all chars in range [first, last) are unique, otherwise return false
 * \note full specialization for const char*
 */
template<>
inline bool is_unique<const char*>(const char* first, const char* last)
{ // check whatever all chars in string are unique chars
	static constexpr size_t CHARSET_SIZE = (1 << (sizeof(char) * CHAR_BIT));

	std::bitset<CHARSET_SIZE> bits;
	for (; first != last; ++first) {
		uint8_t c = static_cast<uint8_t>(*first);
		if (bits.test(c))
			return false;
		bits.set(c, 1);
	}
	return true;
}


/*!
 * \fn is_unique (const char16_t* first, const char16_t* last)
 * \brief check whatever all chars in range [first, last) are unique 
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence.
 * \return true whatever all chars in range [first, last) are unique, otherwise return false
 * \note full specialization for const char16_t*
 */
template<>
inline bool is_unique<const char16_t*>(const char16_t* first, const char16_t* last)
{ // check whatever all chars in string are unique chars
	static constexpr size_t CHARSET_SIZE = (1 << (sizeof(char16_t) * CHAR_BIT));

	std::bitset<CHARSET_SIZE> bits;
	for (; first != last; ++first) {
		uint16_t c = static_cast<uint16_t>(*first);
		if (bits.test(c))
			return false;
		bits.set(c, 1);
	}
	return true;
}

// On windows wchar_t is 16-bit 
#ifdef STDX_OS_WIN
/*!
 * \fn is_unique (const wchar_t* first, const wchar_t* last)
 * \brief check whatever all chars in range [first, last) are unique 
 * \param first The start of the sequence to be tested.
 * \param last  One past the end of the sequence.
 * \return true whatever all chars in range [first, last) are unique, otherwise return false
 * \note full specialization for const wchar_t* 
 * \note this specialization is supported on Microsoft Windows platforms only
 */
template<>
inline bool is_unique<const wchar_t*>(const wchar_t* first, const wchar_t* last)
{ // check whatever all chars in string are unique chars
	static constexpr size_t CHARSET_SIZE = (1 << (sizeof(wchar_t) * CHAR_BIT));

	std::bitset<CHARSET_SIZE> bits;
	for (; first != last; ++first) {
		uint16_t c = static_cast<uint16_t>(*first);
		if (bits.test(c))
			return false;
		bits.set(c, 1);
	}
	return true;
}
#endif

/*!
* \fn is_unique (const wchar_t* first, const wchar_t* last)
* \tparam _Elem   models character
* \tparam _Traits models character traits
* \tparam _Alloc  models allocator
* \param s The string to be tested.
* \return true whatever all elements of string are unique, otherwise return false
* \note overloaded version provided for convinience
*/
template<class _Elem, size_t _Size>
inline bool is_unique(const _Elem (&s)[_Size]) {
	return is_unique(s, s + _Size);
}


/*!
 * \fn is_unique (const wchar_t* first, const wchar_t* last)
 * \tparam _Elem   models character
 * \tparam _Traits models character traits
 * \tparam _Alloc  models allocator
 * \param s The string to be tested.
 * \return true whatever all elements of string are unique, otherwise return false
 * \note overloaded version provided for convinience
 */
template<class _Elem, class _Traits, class _Alloc>
inline bool is_unique(const std::basic_string<_Elem, _Traits, _Alloc>& s) {
	return is_unique(s.c_str(), s.c_str() + s.size());
}

_STDX_END
