// Copyright (c) 2010-2016, Michael Polukarov (Russia).
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
#include <iostream>
#include <iterator>
#include <algorithm>

#include "../platform/common.h"

_STDX_BEGIN

/*!

\tparam _Contained models regular type
\tparam _IterCat models iterator category (input category by default)
\tparam _DiffType models iterator difference type (std::size_t by default)

The any_iterator class supports the following operations:
* default construction
* assignment/copy
* equality compare
* dereference
* member access
* pre increment
* post increment
* post increment and dereference

*/
template<
	typename _Contained,
	typename _IterCat = std::input_iterator_tag,
	typename _DiffType = std::size_t
>
class any_iterator
{
public:
		
	typedef _Contained value_type; //!< Type of contained value
	typedef _Contained* pointer;   //!< Type of contained value pointer
	typedef _Contained& reference; //!< Type of contained value reference
	typedef const _Contained* const_pointer;   //!< Type of contained value constant pointer
	typedef const _Contained& const_reference; //!< Type of contained value constant reference
	typedef _IterCat iterator_category; //!< Type of iterator category
	typedef _DiffType difference_type;  //!< Type of difference between iterators

	//! Default constructor
	inline any_iterator () 
        : __m_body(nullptr) {
	}
		
	//! Copy constructor
	/*!
	Creates a deep copy of iterator.
		
	A constructor for a class cannot be virtual. 
	Normally, you know the concrete type of the object being created, 
	and so this does not cause a problem. 
		
	In some circumstances (here for example), 
	we have only access to the base class type of the object 
	we want to construct. 
	It is meaningless to create an empty one, 
	but perfectly reasonable to require a copy, 
	or a clone. 
		
	It forms part of the interface to the \a wrapper class.
	*/
	inline any_iterator (const any_iterator & rhs) 
        : __m_body(rhs.__m_body ? rhs.__m_body->clone() : nullptr) {
	}
		
	//! Conversion copy constructor
	/*!
	Creates a deep copy of iterator of other 
	type rathen than this iterator
	*/
	template<typename _Iterator>
	inline any_iterator(const _Iterator& rhs) 
		: __m_body(new adaptor<_Iterator>(rhs)) {
	}
		
	//! Destructor
	/*!
	Destroy iterator and free all allocated memory
	*/
	inline ~any_iterator() {
        if (__m_body != nullptr)
            delete __m_body;
        __m_body = nullptr;
	}
	//! Assign operator
	inline any_iterator& operator = (const any_iterator& rhs){
		swap(any_iterator(rhs));  
		return *this;
	}
		
	//! Dereference operator (constant version)
	inline const_reference operator* () const{
		return __m_body->current();
	}
	//! Dereference operator
	inline reference operator* () {
		return __m_body->current();
	}

	//! Pre-Increment operator
	inline any_iterator& operator++ () {
		__m_body->increment();
		return *this;
	}
	//! Pre-Decrement operator
	inline any_iterator& operator-- () {
		__m_body->decrement();
		return *this;
	}
	//! Post-Increment operator
	inline any_iterator operator++ (int) {
		any_iterator tmp(*this);
		__m_body->increment();
		return tmp;
	}
	//! Post-Decrement operator
	inline any_iterator operator-- (int) {
		any_iterator tmp(*this);
		__m_body->decrement();
		return tmp;
	}
		
	//! Equality compare operator
	inline bool operator== (const any_iterator & rhs) {
		return __m_body->equal(rhs.__m_body);
	}
		
	//! Equality compare operator
	inline bool operator!= (const any_iterator & rhs) {
		return (!(*this == rhs));
	}
		
private:
	//! \inner iterator swapping function
	inline void swap(any_iterator& rhs){
		std::swap(__m_body, rhs.__m_body);
		return *this;
	}
	/*!
	Abstract Interface (Wrapper) for iterators
		
	Standard Library Iterators are expressly 
	not related by inheritance; they are related 
	by concept (Gamma+, 1995). 
	They do not have virtual functions, 
	and cannot be treated polymorphically, 
	in the late-binding sense. 
		
	Our requirements for any_iterator are to treat 
	Iterators as if they had a common ancestor. 
	They could then be treated in the common manner.
		
	We need to provide the interface those functions 
	which will allow us to provide an iterator-like 
	interface in any_iterator.
	*/
	class wrapper 
	{
	public:
		virtual ~wrapper(){}
			
		virtual void increment () = 0;
		virtual void decrement () = 0;
		virtual wrapper * clone () const = 0;
			
		virtual value_type& current () const = 0;

		virtual bool equal (const wrapper * rhs) const = 0;
		virtual void assign (const wrapper * rhs) = 0;
	}; // end class wrapper
		
	//! Iterator Adaptor class
	/*!
	\tparam Iterator models concrete iterator
		
	Implements \a wrapper interface.
		
	The adaptor class has member functions 
	which forward the call to the member function of Iterator. 
	In this sense, this structure is similar in some 
	respects to the Adaptor pattern (Gamma+, 1995), 
	but it has differing motivations; 
	it does not set out to convert the interface 
	of Iterator in any way (athough it could),
	only to provide Iterator with polymorphic behaviour. 
		
	The client class, using a pointer to \a wrapper interface, 
	can use it as if it were a Iterator.
	*/
	template<typename Iterator> 
	class adaptor
		: public wrapper 
	{
	public:
		//! Copy constructor
		adaptor (const Iterator & rhs)
			: __m_adaptee(rhs) {
		}
		//! Clone adaptor
		/*!
		Preforms deep copying of adaptor

		This uses the already given 
		conversion from Iterator (the type of adaptee), 
		which has been used before in the conversion 
		constructor of any_iterator. 
		The return from clone() could be covariant, 
		i.e. return a pointer to the actual class rather than 
		a pointer to its base class, but there is no 
		benefit in this, since the target for the returned pointer 
		is a pointer to the base class.
		*/
		wrapper * clone () const {
			return new adaptor<Iterator>(__m_adaptee);
		}
		//! Increment function
		/*!
		Directly increment underlaying iterator
		*/
		virtual void increment () { 
			++__m_adaptee;
		}
		//! Decrement function
		/*!
		Directly decrement underlaying iterator
		*/
		virtual void decrement () { 
			--__m_adaptee;
		}
		//! Dereference method (constant version)
		/*!
		Directly dereference underlaying iterator
		*/
		virtual value_type& current () const { 
			return (value_type&)(*(const_cast<adaptor*>(this)->__m_adaptee));
		}
		//! Dereference method
		/*!
		Directly dereference underlaying iterator
		*/
		virtual value_type& current () { 
			return (value_type&)(*(const_cast<adaptor*>(this)->__m_adaptee));
		}
			
		//! Compare iterators under equality
		/*!
		The rhs pointer could have a different adaptee 
		type to this - in our case, e.g., a list<>::iterator 
		when this one is a vector<>::iterator.
		It is the polymorphic type which interests us, 
		so we can make use of RTTI to determine consistency of type.
			
		The nature of using dynamic_cast on pointers means that tmp 
		will be null if the conversion fails, i.e. the runtime type 
		of rhs is different to this. 
		*/
		virtual bool equal (const wrapper * rhs) const { 
			const adaptor<Iterator> * tmp = dynamic_cast<const adaptor<Iterator>*>(rhs);
			return (tmp && __m_adaptee == tmp->__m_adaptee);
		}
		//! Assign iterator \a rhs to this iterator
		virtual void assign (const wrapper * rhs) { 
			__m_adaptee = static_cast<const adaptor<Iterator> *>(rhs)->__m_adaptee;
		}
			
	private:
		Iterator __m_adaptee;
	}; // end class adaptor
		
	mutable wrapper * __m_body; //!< pointer to iterator wrapper interface

}; // end class any_iterator

_STDX_END // end namespace stlext


