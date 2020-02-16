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
#include <memory>
#include <iosfwd>
#include <string>

#include "../platform/common.h"


_STDX_BEGIN

template<
	class _Elem,
	class _Traits = std::char_traits<_Elem>,
	class _Alloc = std::allocator<_Elem>,
	class _Diff = size_t
>
class istring_iterator
{
public:
	typedef _Elem char_type;
	typedef _Traits traits_type;
	typedef std::basic_istream<_Elem, _Traits> istream_type;

	typedef std::input_iterator_tag iterator_category;
	typedef std::basic_string<_Elem, _Traits, _Alloc> value_type;
	typedef _Diff difference_type;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef value_type& reference;
    typedef const value_type& const_reference;

	constexpr istring_iterator()
		: __m_stream(nullptr)
	{	// construct singular iterator
	}

	istring_iterator(istream_type& stream) :
		__m_stream(&stream)
	{	// construct with input stream
		next();
	}

	istring_iterator(istream_type& stream, const _Alloc& al) :
		__m_stream(&stream), __m_val(al)
	{	// construct with input stream
		next();
	}

	istring_iterator(istream_type& stream, size_t reserved)
		: __m_stream(&stream)
	{	// construct with input stream
		reserve(reserved);
		next();
	}

	istring_iterator(istream_type& stream, size_t reserved, const _Alloc& al) :
		__m_stream(&stream), __m_val(al)
	{	// construct with input stream
		reserve(reserved);
		next();
	}

	reference operator*() 
	{	// return designated value
		return (__m_val);
	}

	pointer operator->() 
	{	// return pointer to class object
		return (std::pointer_traits<pointer>::pointer_to(**this));
	}

	const_reference operator*() const
	{	// return designated value
		return (__m_val);
	}

	const_pointer operator->() const
	{	// return pointer to class object
		return (std::pointer_traits<const_pointer>::pointer_to(**this));
	}

	istring_iterator& operator++()
	{	// preincrement
		next();
		return (*this);
	}

	istring_iterator operator++(int)
	{	// postincrement
		istring_iterator _Tmp = *this;
		++*this;
		return (_Tmp);
	}

	bool equal(const istring_iterator& _Right) const
	{	// test for iterator equality
		return (__m_stream == _Right.__m_stream);
	}

protected:
	void next()
	{	// get a value if possible
		if (__m_stream != 0 && !std::getline(*__m_stream, __m_val))
			__m_stream = 0;
	}

	void reserve(size_t n) {
		if (n > 0)
			__m_val.reserve(n);
	}

	// pointer to input stream
	istream_type *__m_stream;
	// lookahead value (valid if __m_stream is not null)
	std::basic_string<_Elem, _Traits, _Alloc> __m_val;
};

template<
	class _Elem,
	class _Traits,
	class _Alloc,
	class _Diff
>
inline bool operator==(
	const istring_iterator<_Elem, _Traits, _Alloc, _Diff>& lhs,
	const istring_iterator<_Elem, _Traits, _Alloc, _Diff>& rhs)
{	// test for istream_iterator equality
	return (lhs.equal(rhs));
}

template<
	class _Elem,
	class _Traits,
	class _Alloc,
	class _Diff
>
inline bool operator!=(
	const istring_iterator<_Elem, _Traits, _Alloc, _Diff>& lhs,
	const istring_iterator<_Elem, _Traits, _Alloc, _Diff>& rhs)
{	// test for istream_iterator inequality
	return (!(lhs == rhs));
}



_STDX_END


