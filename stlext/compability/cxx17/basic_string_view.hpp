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

#include <string>
#include <algorithm>
#include <iosfwd>

#include "../../platform/common.h"

_STDX_BEGIN

// 21.4.2 template class basic_string_view
template<
	class _Elem,
	class _Traits = std::char_traits<_Elem>
>
class basic_string_view
{
public:
	using traits_type = _Traits;
	using value_type = _Elem;
	using pointer = _Elem*;
	using const_pointer = const _Elem*;
	using reference = _Elem&;
	using const_reference = const _Elem&;

	using const_iterator = const _Elem*;
	using iterator = const_iterator;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	using size_type = size_t;
	using difference_type = std::ptrdiff_t;

	static constexpr size_type npos = size_type(-1);

	// 21.4.2.1 construction and assignment
	constexpr basic_string_view() :
		__m_data(nullptr), __m_size(0) {
	}

	constexpr basic_string_view(const_pointer s) :
		__m_data(s), __m_size(s == nullptr ? 0 : _Traits::length(s)) {
	}

	constexpr basic_string_view(const_pointer s, size_type n) :
		__m_data(s), __m_size(n) {
	}

    template<size_t N>
    constexpr basic_string_view(const _Elem (&s)[N]) :
        __m_data(s), __m_size(N-1) {
    }

	template<class _Alloc>
	constexpr basic_string_view(const std::basic_string<_Elem, _Traits, _Alloc>& str) :
		__m_data(str.data()), __m_size(str.size()) {
	}

	basic_string_view(const basic_string_view&) noexcept = default;

	basic_string_view& operator=(const basic_string_view&) noexcept = default;

	template<class _Alloc>
	basic_string_view& operator=(const std::basic_string<_Elem, _Traits, _Alloc>& str) noexcept {
		__m_data = str.data();
		__m_size = str.size();
		return (*this);
	}

	// (non-standard) basic_string conversion
	template<class _Alloc>
	operator std::basic_string<_Elem, _Traits, _Alloc>() const {
		return std::basic_string<_Elem, _Traits, _Alloc>(__m_data, __m_size);
	}

	// 21.4.2.2 iterator support
	constexpr const_iterator begin() const noexcept {
		return (__m_data);
	}

	constexpr const_iterator end() const noexcept {
		return (__m_data + __m_size);
	}

	constexpr const_iterator cbegin() const noexcept {
		return __m_data;
	}

	constexpr const_iterator cend() const noexcept {
		return (__m_data + __m_size);
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	constexpr const_reverse_iterator crbegin() const noexcept {
		return rbegin();
	}

	constexpr const_reverse_iterator crend() const noexcept {
		return rend();
	}

	// 21.4.2.3 capacity
	constexpr size_type size() const {
		return __m_size;
	}

	constexpr size_type length() const {
		return __m_size;
	}

	constexpr size_type max_size() const {
		return size_type(-1);
	}

	constexpr bool empty() const {
		return (__m_size == 0);
	}

	// 21.4.2.4 element access
	constexpr const_reference operator[](size_type pos) const {
		return __m_data[pos];
	}

	constexpr const_reference at(size_type pos) const {
		if (pos >= __m_size)
			__stdx_except(std::out_of_range, "basic_string_view::at(): pos is out of range");
		return __m_data[pos];
	}

	constexpr const_reference front() const {
		return *(__m_data);
	}

	constexpr const_reference back() const {
		return *(__m_data + (__m_size - 1));
	}

	constexpr const_pointer data() const {
		return __m_data;
	}


	// 21.4.2.5 modifiers
	inline void remove_prefix(size_type n) {
		__m_data += n;
		__m_size -= n;
	}

	inline void remove_suffix(size_type n) {
		__m_size -= n;
	}

	void swap(basic_string_view& s) noexcept {
		std::swap(__m_data, s.__m_data);
		std::swap(__m_size, s.__m_size);
	}


	// 21.4.2.6 string operations
	size_type copy(pointer s, size_type n, size_type pos = 0) const {
		if (pos > __m_size)
			__stdx_except(std::out_of_range, "basic_string_view::copy(): pos is out of range");
		size_type rlen = (std::min)(n, __m_size - pos);
		std::copy_n(begin() + pos, rlen, s);
		return rlen;
	}

	constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const {
		if (pos > __m_size)
			__stdx_except(std::out_of_range, "basic_string_view::substr(): pos is out of range");
		return basic_string_view(__m_data + pos, (std::min)(n, __m_size - pos));
	}

	constexpr int compare(const_pointer __s) const noexcept {
		return __compare(__m_data, __m_size, __s, _Traits::length(__s));
	}

	constexpr int compare(const basic_string_view& __other) const noexcept {
		return __compare(__m_data, __m_size, __other.__m_data, __other.__m_size);
	}

	template<class _Alloc>
	constexpr int compare(const std::basic_string<_Elem, _Traits, _Alloc>& __other) const noexcept {
		return __compare(__m_data, __m_size, __other.data(), __other.size());
	}

	constexpr int compare(size_type pos1, size_type n1, const basic_string_view& __other) const {
		return substr(pos1, n1).compare(__other);
	}

	constexpr int compare(size_type pos1, size_type n1, const basic_string_view& __other,
							size_type pos2, size_type n2) const {
		return substr(pos1, n1).compare(__other.substr(pos2, n2));
	}


	constexpr int compare(size_type pos1, size_type n1,
							const_pointer __s) const
	{
		return substr(pos1, n1).compare(basic_string_view(__s));
	}

	constexpr int compare(size_type pos1, size_type n1,
							const_pointer __s, size_type n2) const
	{
		return substr(pos1, n1).compare(basic_string_view(__s, n2));
	}

	// 21.4.2.7 searching
	size_type find(const_pointer ptr, size_type offset, size_type n) const noexcept
	{	// look for [ptr, ptr + n) beginning at or after offset
		if (n == 0 && offset <= __m_size)
			return (offset);	// null string always matches (if inside string)

		size_type _Nm;
		if (offset < __m_size && n <= (_Nm = __m_size - offset))
		{	// room for match, look for it
			const_pointer _Uptr, _Vptr;
			for (_Nm -= n - 1, _Vptr = __m_data + offset;
				(_Uptr = _Traits::find(_Vptr, _Nm, *ptr)) != 0;
					_Nm -= _Uptr - _Vptr + 1, _Vptr = _Uptr + 1)
				if (_Traits::compare(_Uptr, ptr, n) == 0)
					return (_Uptr - __m_data);	// found a match
		}
		return (npos);	// no match
	}

	inline size_type find(const basic_string_view& s, size_type offset = 0) const noexcept
	{	// look for _Right beginning at or after _Off
		return (find(s.__m_data, offset, s.__m_size));
	}

	template<class _Alloc>
	inline size_type find(const std::basic_string<_Elem, _Traits, _Alloc>& s, size_type offset = 0) const noexcept
	{	// look for _Right beginning at or after _Off
		return (find(s.data(), offset, s.size()));
	}

	inline size_type find(const_pointer p, size_type offset = 0) const noexcept
	{	// look for [_Ptr, <null>) beginning at or after _Off
		return (find(p, offset, _Traits::length(p)));
	}

	inline size_type find(value_type c, size_type offset = 0) const noexcept
	{	// look for _Ch at or after _Off
		return (find((const_pointer)&c, offset, 1));
	}


	size_type rfind(const_pointer p, size_type offset, size_type n) const noexcept
	{	// look for [p, p + n) beginning before offset
		if (n == 0)
			return (offset < __m_size ? offset : __m_size);	// null always matches
		if (n <= __m_size)
		{	// room for match, look for it
			const_pointer _Uptr = __m_data + (offset < __m_size - n ? offset : __m_size - n);
			for (; ; --_Uptr)
				if (_Traits::eq(*_Uptr, *p) && _Traits::compare(_Uptr, p, n) == 0)
					return (_Uptr - __m_data);	// found a match
				else if (_Uptr == __m_data)
					break;	// at beginning, no more chance for match
		}
		return (npos);	// no match
	}

	inline size_type rfind(const basic_string_view& s, size_type offset = npos) const noexcept
	{	// look for _Right beginning at or after _Off
		return (rfind(s.__m_data, offset, s.__m_size));
	}

	template<class _Alloc>
	inline size_type rfind(const std::basic_string<_Elem, _Traits, _Alloc>& s, size_type offset = npos) const noexcept
	{	// look for _Right beginning at or after _Off
		return (rfind(s.data(), offset, s.size()));
	}

	inline size_type rfind(const_pointer p, size_type offset = npos) const noexcept
	{	// look for [_Ptr, <null>) beginning at or after _Off
		return (rfind(p, offset, _Traits::length(p)));
	}

	inline size_type rfind(value_type c, size_type offset = npos) const noexcept
	{	// look for _Ch at or after _Off
		return (rfind((const_pointer)&c, offset, 1));
	}


	size_type find_first_of(const_pointer p,
							size_type offset, size_type n) const
	{	// look for one of [p, p + n) at or after offset
		if (0 < n && offset < __m_size)
		{	// room for match, look for it
			const_pointer _Vptr = __m_data + __m_size;
			for (const_pointer _Uptr = __m_data + offset;
					_Uptr < _Vptr; ++_Uptr)
				if (_Traits::find(p, n, *_Uptr) != 0)
					return (_Uptr - __m_data);	// found a match
		}

		return (npos);	// no match
	}

	inline size_type find_first_of(const basic_string_view& s, size_type offset = 0) const noexcept
	{	// look for _Right beginning at or after _Off
		return (find_first_of(s.__m_data, offset, s.__m_size));
	}

	template<class _Alloc>
	inline size_type find_first_of(const std::basic_string<_Elem, _Traits, _Alloc>& s, size_type offset = 0) const noexcept
	{	// look for _Right beginning at or after _Off
		return (find_first_of(s.data(), offset, s.size()));
	}

	inline size_type find_first_of(const_pointer p, size_type offset = 0) const noexcept
	{	// look for [_Ptr, <null>) beginning at or after _Off
		return (find_first_of(p, offset, _Traits::length(p)));
	}

	inline size_type find_first_of(value_type c, size_type offset = 0) const noexcept
	{	// look for _Ch at or after _Off
		return (find_first_of((const_pointer)&c, offset, 1));
	}


	size_type find_first_not_of(const_pointer p, size_type offset, size_type n) const
	{	// look for none of [p, p + n) at or after offset
		if (offset < __m_size)
		{	// room for match, look for it
			const_pointer _Vptr = __m_data + __m_size;
			for (const_pointer _Uptr = __m_data + offset;
					_Uptr < _Vptr; ++_Uptr)
				if (_Traits::find(p, n, *_Uptr) == 0)
					return (_Uptr - __m_data);
		}
		return (npos);
	}

	inline size_type find_first_not_of(const basic_string_view& s, size_type offset = 0) const noexcept
	{	// look for s beginning at or after offset
		return (find_first_not_of(s.__m_data, offset, s.__m_size));
	}

	template<class _Alloc>
	inline size_type find_first_not_of(const std::basic_string<_Elem, _Traits, _Alloc>& s, size_type offset = 0) const noexcept
	{	// look for s beginning at or after offset
		return (find_first_not_of(s.data(), offset, s.size()));
	}

	inline size_type find_first_not_of(const_pointer p, size_type offset = 0) const noexcept
	{	// look for [p, <null>) beginning at or after offset
		return (find_first_not_of(p, offset, _Traits::length(p)));
	}

	inline size_type find_first_not_of(value_type c, size_type offset = 0) const noexcept
	{	// look for c at or after offset
		return (find_first_not_of((const_pointer)&c, offset, 1));
	}


	size_type find_last_of(const_pointer p, size_type offset, size_type n) const
	{	// look for one of [p, p + n) before offset
		if (0 < n && 0 < __m_size)
		{	// worth searching, do it
			const_pointer _Uptr = __m_data + (offset < __m_size ? offset : __m_size - 1);
			for (; ; --_Uptr)
				if (_Traits::find(p, n, *_Uptr) != 0)
					return (_Uptr - __m_data);	// found a match
				else if (_Uptr == __m_data)
					break;	// at beginning, no more chance for match
		}

		return (npos);	// no match
	}

	inline size_type find_last_of(const basic_string_view& s, size_type offset = npos) const noexcept
	{	// look for s beginning at or after offset
		return (find_last_of(s.__m_data, offset, s.__m_size));
	}

	template<class _Alloc>
	inline size_type find_last_of(const std::basic_string<_Elem, _Traits, _Alloc>& s, size_type offset = npos) const noexcept
	{	// look for s beginning at or after offset
		return (find_last_of(s.data(), offset, s.size()));
	}

	inline size_type find_last_of(const_pointer p, size_type offset = npos) const noexcept
	{	// look for [p, <null>) beginning at or after offset
		return (find_last_of(p, offset, _Traits::length(p)));
	}

	inline size_type find_last_of(value_type c, size_type offset = npos) const noexcept
	{	// look for c at or after offset
		return (find_last_of((const_pointer)&c, offset, 1));
	}


	size_type find_last_not_of(const_pointer p, size_type offset, size_type n) const
	{	// look for none of [p, p + n) before __offset
		if (0 < __m_size)
		{	// worth searching, do it
			const_pointer _Uptr = __m_data + (offset < __m_size ? offset : __m_size - 1);
			for (; ; --_Uptr)
				if (_Traits::find(p, n, *_Uptr) == 0)
					return (_Uptr - __m_data);
				else if (_Uptr == __m_data)
					break;
		}
		return (npos);
	}

	inline size_type find_last_not_of(const basic_string_view& s, size_type offset = npos) const noexcept
	{	// look for s beginning at or after offset
		return (find_last_not_of(s.__m_data, offset, s.__m_size));
	}

	template<class _Alloc>
	inline size_type find_last_not_of(const std::basic_string<_Elem, _Traits, _Alloc>& s, size_type offset = npos) const noexcept
	{	// look for s beginning at or after offset
		return (find_last_not_of(s.data(), offset, s.size()));
	}

	inline size_type find_last_not_of(const_pointer p, size_type offset = npos) const noexcept
	{	// look for [p, <null>) beginning at or after offset
		return (find_last_not_of(p, offset, _Traits::length(p)));
	}

	inline size_type find_last_not_of(value_type c, size_type offset = npos) const noexcept
	{	// look for c at or after offset
		return (find_last_not_of((const_pointer)&c, offset, 1));
	}

	size_t hash_code() const {
#ifdef STDX_CMPLR_MSVC
		return ::std::_Hash_seq(reinterpret_cast<const unsigned char*>(__m_data), __m_size * sizeof(_Elem));
#else
		return ::std::_Hash_bytes(reinterpret_cast<const unsigned char*>(__m_data), __m_size * sizeof(_Elem), 0xdeadbeef);
#endif
	}

private:
	static int __compare(const_pointer left, const size_type lsize,
							const_pointer right, const size_type rsize)
	{	// compare [_Left, _Left + _Left_size) to [_Right, _Right + _Right_size) using _Traits
		const size_type _min_len = lsize < rsize ? lsize : rsize;
		const int _ans = _Traits::compare(left, right, _min_len);

		if (_ans != 0)
			return (_ans);

		if (lsize < rsize)
			return (-1);

		if (lsize > rsize)
			return (1);

		return (0);
	}

private:
	const_pointer __m_data;
	size_type __m_size;
}; // end class basic_string_view<...>



   // 21.4.3 non-member comparision functions

template<class _Elem, class _Traits>
constexpr bool operator== (const basic_string_view<_Elem, _Traits>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return lhs.compare(rhs) == 0;
}

template<class _Elem, class _Traits>
constexpr bool operator!= (const basic_string_view<_Elem, _Traits>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return lhs.compare(rhs) != 0;
}

template<class _Elem, class _Traits>
constexpr bool operator> (const basic_string_view<_Elem, _Traits>& lhs,
						  const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return lhs.compare(rhs) > 0;
}

template<class _Elem, class _Traits>
constexpr bool operator< (const basic_string_view<_Elem, _Traits>& lhs,
	const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<class _Elem, class _Traits>
constexpr bool operator>= (const basic_string_view<_Elem, _Traits>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return lhs.compare(rhs) >= 0;
}

template<class _Elem, class _Traits>
constexpr bool operator<= (const basic_string_view<_Elem, _Traits>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return lhs.compare(rhs) <= 0;
}

// basic_string_view <=> basic_string comparisions

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator== (const basic_string_view<_Elem, _Traits>& lhs,
						   const std::basic_string<_Elem, _Traits, _Alloc>& rhs) noexcept {
	return lhs.compare(rhs) == 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator== (const std::basic_string<_Elem, _Traits, _Alloc>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return rhs.compare(lhs) == 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator!= (const basic_string_view<_Elem, _Traits>& lhs,
						   const std::basic_string<_Elem, _Traits, _Alloc>& rhs) noexcept {
	return lhs.compare(rhs) != 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator!= (const std::basic_string<_Elem, _Traits, _Alloc>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return rhs.compare(lhs) != 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator< (const basic_string_view<_Elem, _Traits>& lhs,
	const std::basic_string<_Elem, _Traits, _Alloc>& rhs) noexcept {
	return lhs.compare(rhs) < 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator< (const std::basic_string<_Elem, _Traits, _Alloc>& lhs,
	const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return rhs.compare(lhs) > 0;
}


template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator> (const basic_string_view<_Elem, _Traits>& lhs,
						  const std::basic_string<_Elem, _Traits, _Alloc>& rhs) noexcept {
	return lhs.compare(rhs) > 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator> (const std::basic_string<_Elem, _Traits, _Alloc>& lhs,
						  const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return rhs.compare(lhs) < 0;
}


template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator<= (const basic_string_view<_Elem, _Traits>& lhs,
						   const std::basic_string<_Elem, _Traits, _Alloc>& rhs) noexcept {
	return lhs.compare(rhs) <= 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator<= (const std::basic_string<_Elem, _Traits, _Alloc>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return rhs.compare(lhs) >= 0;
}



template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator>= (const basic_string_view<_Elem, _Traits>& lhs,
						   const std::basic_string<_Elem, _Traits, _Alloc>& rhs) noexcept {
	return lhs.compare(rhs) >= 0;
}

template<class _Elem, class _Traits, class _Alloc>
constexpr bool operator>= (const std::basic_string<_Elem, _Traits, _Alloc>& lhs,
						   const basic_string_view<_Elem, _Traits>& rhs) noexcept {
	return rhs.compare(lhs) <= 0;
}

// 21.4.4. Inserters and extractors
template<class _Elem, class _Traits>
inline ::std::basic_ostream<_Elem, _Traits>& operator<< (::std::basic_ostream<_Elem, _Traits>& stream,
												  const basic_string_view<_Elem, _Traits>& sv) {
    return ( sv.empty() ? (stream) : (stream.write(sv.data(), sv.size())) );
}

// 21.4.5 Hash support

template<class _Elem, class _Traits>
struct hash
{
	typedef stdx::basic_string_view<_Elem, _Traits> argument_type;
	typedef size_t result_type;

	inline size_t operator()(const argument_type& x) const {
		return x.hash_code();
	}
};

using string_view = basic_string_view< char, std::char_traits<char> >;
using wstring_view = basic_string_view< wchar_t, std::char_traits<wchar_t> >;
using u16string_view = basic_string_view< char16_t, std::char_traits<char16_t> >;
using u32string_view = basic_string_view< char32_t, std::char_traits<char32_t> >;

_STDX_END


