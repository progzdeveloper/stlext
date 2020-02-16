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

_STDX_BEGIN

// class for basic free list logic
template<size_t _MaxSize = 0>
class freelist
{
public:
	freelist()
		: __m_head(0), __m_nblocks(0)
	{	// construct with empty list
	}

	bool push(void *__p)
	{	// push onto free list depending on max
		if (full())
			return (false);
		else
		{	// push onto free list
			(reinterpret_cast<node*>(__p))->next = __m_head;
			__m_head = reinterpret_cast<node*>(__p);
			saved();
			return (true);
		}
	}

	void *pop()
	{	// pop node from free list
		void *ptr = __m_head;
		if (ptr != 0)
		{	// relink
			__m_head = __m_head->next;
			released();
		}
		return (ptr);
	}

	inline bool empty() const {
		return (__m_head == nullptr);
	}
private:
	inline bool full() const
	{	// test for full
		return (_MaxSize <= __m_nblocks);
	}

	inline void saved()
	{	// increment saved count
		++__m_nblocks;
	}

	inline void released()
	{	// decrement saved count
		--__m_nblocks;
	}

private:
	struct node
	{	// list node
		node* next;
	};
	node* __m_head;
	size_t __m_nblocks;
};



template<>
class freelist<0>
{
public:
	freelist()
		: __m_head(0)
	{	// construct with empty list
	}

	bool push(void *__p)
	{	// push onto free list
		(reinterpret_cast<node*>(__p))->next = __m_head;
		__m_head = reinterpret_cast<node*>(__p);
		return true;
	}

	void *pop()
	{	// pop node from free list
		void *ptr = __m_head;
		if (ptr != 0)
		{	// relink
			__m_head = __m_head->next;
		}
		return (ptr);
	}

	inline bool empty() const {
		return (__m_head == nullptr);
	}

private:
	struct node
	{	// list node
		node* next;
	};
	node* __m_head;
};


_STDX_END
