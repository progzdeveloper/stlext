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

#include "../platform/common.h"

_STDX_BEGIN

template<class _Container>
class uinsert_iterator
{	// wrap inserts into associative container as output iterator

private:
	typedef typename _Container::value_type __val_t;
public:
	typedef _Container container_type;
	
	typedef std::output_iterator_tag iterator_category;

	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void const_reference;
	typedef void reference;


	uinsert_iterator(container_type& c)
		: __m_container(std::addressof(c))
	{	// construct with container 
	}

	uinsert_iterator& operator=(const __val_t& x)
	{	// insert value into container
		__m_container->insert(x);
		return (*this);
	}

	uinsert_iterator& operator=(__val_t&& x)
	{	// insert value into container
		__m_container->emplace(std::forward<__val_t>(x));
		return (*this);
	}

	uinsert_iterator& operator*()
	{	// pretend to return designated value
		return (*this);
	}

	uinsert_iterator& operator++()
	{	// pretend to preincrement
		return (*this);
	}

	uinsert_iterator& operator++(int)
	{	// pretend to postincrement
		return (*this);
	}

protected:
	_Container *__m_container;	// pointer to container
};

template<class _Container> 
inline uinsert_iterator<_Container> uinserter(_Container& c)
{	// return insert_iterator
	return{ c };
}

_STDX_END

