// Copyright (c) 2019, Michael Polukarov (Russia).
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

#include <iterator>

_STDX_BEGIN

namespace detail
{
	template<class _It>
	class circular_iterator_base
	{
		typedef typename std::iterator_traits<_It>::value_type         value_type;
	public:
		circular_iterator_base(_It first, _It last, _It curr) :
			__m_first(first), __m_last(last), __m_curr(curr) {
		}

		inline value_type& operator*() { return (*__m_curr); }

		inline value_type& operator*() const { return (*__m_curr); }

		inline value_type* operator->() { return std::addressof(*__m_curr); }

		inline const value_type* operator->() const { return std::addressof(*__m_curr); }

		inline bool operator== (const circular_iterator_base& other) const {
			return ((__m_curr == other.__m_curr) &&
					(__m_first == other.__m_first) &&
					(__m_last == other.__m_last));
		}

		inline bool operator!= (const circular_iterator_base& y) const {
			return (!((*this) == y));
		}

		template<typename _IterType>
		inline bool operator== (const _IterType& _Other) const {
			return (__m_curr == _Other);
		}

		template<typename _IterType>
		inline bool operator!= (const _IterType& _Other) const {
			return (__m_curr != _Other);
		}

	protected:
		_It __m_first, __m_last, __m_curr;
	};


	template<class _Iterator, class _Category>
	class circular_iterator_adapter;

	template<class _It>
	class circular_iterator_adapter<_It, std::forward_iterator_tag> : 
		public circular_iterator_base<_It>
	{
		typedef circular_iterator_base<_It> base_type;

	public:
		circular_iterator_adapter(_It first, _It last, _It curr) :
			base_type(first, last, curr) {
		}

		inline circular_iterator_adapter& operator++() {
			__next();
			return *this;
		}

		inline circular_iterator_adapter operator++(int) {
			circular_iterator_adapter tmp(*this);
			__next();
			return tmp;
		}

	protected:
		inline void __next() {
			++(this->__m_curr);
			if (this->__m_curr == this->__m_last) {
				this->__m_curr = this->__m_first;
			}
		}
	};



	template<class _It>
	class circular_iterator_adapter<_It, std::bidirectional_iterator_tag> :
		public circular_iterator_adapter<_It, std::forward_iterator_tag>
	{
		typedef circular_iterator_adapter<_It, std::forward_iterator_tag> base_type;

	public:
		circular_iterator_adapter(_It first, _It last, _It curr) :
			base_type(first, last, curr) {
		}


		inline circular_iterator_adapter& operator--() {
			__prev();
			return *this;
		}

		inline circular_iterator_adapter operator--(int) {
			circular_iterator_adapter tmp(*this);
			__prev();
			return tmp;
		}

	protected:
		inline void __prev() {
			if (this->__m_curr == this->__m_first)
				this->__m_curr = std::prev(this->__m_last);
			else
				--(this->__m_curr);
		}
	};




	template<class _It>
	class circular_iterator_adapter<_It, std::random_access_iterator_tag> :
		public circular_iterator_adapter<_It, std::bidirectional_iterator_tag>
	{
		typedef circular_iterator_adapter<_It, std::bidirectional_iterator_tag> base_type;

	public:
		typedef typename std::iterator_traits<_It>::difference_type difference_type;

		circular_iterator_adapter(_It first, _It last, _It curr) :
			base_type(first, last, curr) {
		}

		circular_iterator_adapter& operator+= (difference_type n) {
			__advance(n);
			return (*this);
		}

		circular_iterator_adapter& operator-= (difference_type n) {
			__advance(n);
			return (*this);
		}

		circular_iterator_adapter operator+ (difference_type n) const {
			circular_iterator_adapter res(*this);
			res += n;
			return (res);
		}

		circular_iterator_adapter operator- (difference_type n) const {
			circular_iterator_adapter res(*this);
			res -= n;
			return (res);
		}



		friend difference_type operator- (const circular_iterator_adapter& x, 
										  const circular_iterator_adapter& y) {
			return x.__distance(y);
		}

		friend difference_type operator- (const circular_iterator_adapter& x,
										  const _It& y)  {
			return x.__distance(y);
		}

		friend difference_type operator- (const _It& x,
										  const circular_iterator_adapter& y) {
			return x.__distance(y);
		}

		friend bool operator< (const circular_iterator_adapter& lhs,
							   const circular_iterator_adapter& rhs) {
			return (lhs.__m_curr < rhs.__m_curr);
		}

		friend bool operator<= (const circular_iterator_adapter& lhs,
								const circular_iterator_adapter& rhs) {
			return (lhs.__m_curr <= rhs.__m_curr);
		}

		friend bool operator> (const circular_iterator_adapter& lhs,
							   const circular_iterator_adapter& rhs) {
			return (lhs.__m_curr > rhs.__m_curr);
		}

		friend bool operator>= (const circular_iterator_adapter& lhs,
								const circular_iterator_adapter& rhs) {
			return (lhs.__m_curr >= rhs.__m_curr);
		}


	protected:
		inline difference_type __distance(const circular_iterator_adapter& other) {
			return std::distance(this->__m_curr, other.__m_curr);
		}

		inline difference_type __distance(const _It& other) {
			return std::distance(this->__m_curr, other.__m_curr);
		}

		void __advance(difference_type d) 
		{
			size_t n = std::distance(this->__m_first, this->__m_last);
			size_t k = std::distance(this->__m_first, this->__m_curr);
			size_t s = 0;
			if (d < 0) { 
				d = -d;
				size_t r = d % n;
				s = (r > 0) ? (n - r) : r;
			}
			else { 
				s = (k + (d % n)); 
			}
			this->__m_curr = this->__m_first + s;
		
		}
	};

}



/*!

\tparam _Iter adaptee iterator

Iterator adapter that makes possible infinite cyclical iterations through the container
*/
template<class _It>
class circular_iterator : 
	public detail::circular_iterator_adapter<_It, typename std::iterator_traits<_It>::iterator_category>
{
	typedef detail::circular_iterator_adapter<
		_It, typename std::iterator_traits<_It>::iterator_category
	> base_type;
public:
	typedef typename std::iterator_traits<_It>::iterator_category  iterator_category;
	typedef typename std::iterator_traits<_It>::value_type         value_type;      //!< Type of contained value
	typedef typename std::iterator_traits<_It>::pointer            pointer;         //!< Type of contained value pointer
	typedef typename std::iterator_traits<_It>::reference          reference;       //!< Type of contained value reference
	typedef typename std::iterator_traits<_It>::difference_type    difference_type; //!< Type of difference between iterators
	typedef const pointer                                          const_pointer;   //!< Type of contained value const pointer
	typedef const reference                                        const_reference; //!< Type of contained value const reference
	
	
	explicit circular_iterator() : 
		base_type() {
	}

	explicit circular_iterator(_It _Begin, _It _End) :
		base_type(_Begin, _End, _Begin) {
	}

	explicit circular_iterator(_It _Begin, _It _End, _It _Curr) :
		base_type(_Begin, _End, _Begin) {
	}

	circular_iterator& operator= (const _It& _Curr) {
		this->__m_curr = _Curr;
		return (*this);
	}

	template<class _Container>
	explicit circular_iterator(_Container& c) :
		base_type(std::begin(c), std::end(c), std::begin(c)) {
	}

	template<class _Container>
	explicit circular_iterator(_Container& c, _It _Curr) :
		base_type(std::begin(c), std::end(c), _Curr) {
	}

	

}; // end class circular_iterator

_STDX_END





