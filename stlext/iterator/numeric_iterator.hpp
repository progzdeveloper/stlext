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

#include "../platform/common.h"
#include "../numeric/identity_element.hpp"
_STDX_BEGIN


template<
	class T,
	ptrdiff_t _Stride = 0
>
class numeric_iterator
{

public:
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T*const pointer;
	typedef const T* const const_pointer;

	typedef std::ptrdiff_t difference_type;
	typedef std::ptrdiff_t distance_type;

	typedef std::random_access_iterator_tag iterator_category;


	numeric_iterator(const T& x) :
		__m_val(x) {
	}

	numeric_iterator(const numeric_iterator& other) :
		__m_val(other.__m_val) {
	}

	numeric_iterator(numeric_iterator&& other) :
		__m_val(std::move(other.__m_val)) {
	}

	numeric_iterator& operator= (const numeric_iterator& other) {
		__m_val = other.__m_val;
		return (*this);
	}

	numeric_iterator& operator= (numeric_iterator&& other) {
		__m_val = std::move(other.__m_val);
		return (*this);
	}

	inline const_reference operator*() const { return __m_val; }

    inline const_pointer operator->() const { return (&__m_val); }

	inline const_reference operator[] (difference_type offset) const {
		return (__m_val + offset * _Stride);
	}

	inline numeric_iterator& operator+= (difference_type offset) {
		__m_val += (offset * _Stride);
		return (*this);
	}

	inline numeric_iterator& operator++() {
		__m_val += _Stride;
		return (*this);
	}

	inline numeric_iterator operator++(int) {
		numeric_iterator t(*this);
		__m_val += _Stride;
		return t;
	}

	inline numeric_iterator operator+ (difference_type offset) const {
        numeric_iterator t(*this);
		return (t += offset);
	}


	inline numeric_iterator& operator-= (difference_type offset) {
		__m_val -= (offset * _Stride);
		return (*this);
	}

	inline numeric_iterator& operator--() {
		__m_val -= _Stride;
		return (*this);
	}

	inline numeric_iterator operator--(int) {
		numeric_iterator t(*this);
		__m_val -= _Stride;
		return t;
	}


	inline numeric_iterator operator- (difference_type offset) const {
		numeric_iterator t(*this);
		t -= offset;
		return t;
	}

	inline T operator- (const numeric_iterator& other) const {
		return ((__m_val - other.__m_val) / _Stride);
	}



	friend bool operator== (const numeric_iterator& lhs,
							const numeric_iterator& rhs) {
		return (lhs.__m_val == rhs.__m_val);
	}

	friend bool operator!= (const numeric_iterator& lhs,
							const numeric_iterator& rhs) {
		return !(lhs == rhs);
	}

	friend bool operator< (const numeric_iterator& lhs,
						   const numeric_iterator& rhs) {
		return (lhs.__m_val < rhs.__m_val);
	}

	friend bool operator<= (const numeric_iterator& lhs,
							const numeric_iterator& rhs) {
		return (lhs.__m_val <= rhs.__m_val);
	}

	friend bool operator> (const numeric_iterator& lhs,
						   const numeric_iterator& rhs) {
		return (lhs.__m_val > rhs.__m_val);
	}

	friend bool operator>= (const numeric_iterator& lhs,
							const numeric_iterator& rhs) {
		return (lhs.__m_val >= rhs.__m_val);
	}

private:
	T __m_val;
};


template<class T>
class numeric_iterator<T, 1>
{
public:
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T*const pointer;
	typedef const T* const const_pointer;

	typedef std::ptrdiff_t difference_type;
	typedef std::ptrdiff_t distance_type;

	typedef std::random_access_iterator_tag iterator_category;


	numeric_iterator(const T& x) :
		__m_val(x) {
	}

	numeric_iterator(const numeric_iterator& other) :
		__m_val(other.__m_val) {
	}

	numeric_iterator(numeric_iterator&& other) :
		__m_val(std::move(other.__m_val)) {
	}

	numeric_iterator& operator= (const numeric_iterator& other) {
		__m_val = other.__m_val;
		return (*this);
	}

	numeric_iterator& operator= (numeric_iterator&& other) {
		__m_val = std::move(other.__m_val);
		return (*this);
	}

	inline const_reference operator*() const { return __m_val; }

    inline const_pointer operator->() const { return (&__m_val); }


	inline numeric_iterator operator+ (difference_type offset) const {
		numeric_iterator t(*this);
		t += offset;
		return t;
	}

	inline numeric_iterator& operator+=(difference_type n) {
		__m_val += n;
		return (*this);
	}

	inline numeric_iterator& operator++() {
		++__m_val;
		return (*this);
	}

	inline numeric_iterator operator++(int) {
		numeric_iterator t(*this);
		++__m_val;
		return t;
	}

	inline numeric_iterator& operator-=(difference_type n) {
		__m_val -= n;
		return (*this);
	}

	inline numeric_iterator& operator--() {
		--__m_val;
		return (*this);
	}

	inline numeric_iterator operator--(int) {
		numeric_iterator t(*this);
		--__m_val;
		return t;
	}

	inline numeric_iterator operator- (difference_type offset) const {
		numeric_iterator t(*this);
		t -= offset;
		return t;
	}

	inline T operator- (const numeric_iterator& other) const {
		return (__m_val - other.__m_val);
	}

	friend bool operator== (const numeric_iterator& lhs, const numeric_iterator& rhs) {
		return (lhs.__m_val == rhs.__m_val);
	}

	friend bool operator!= (const numeric_iterator& lhs, const numeric_iterator& rhs) {
		return !(lhs == rhs);
	}

	friend bool operator< (const numeric_iterator& lhs, const numeric_iterator& rhs) {
		return (lhs.__m_val < rhs.__m_val);
	}

	friend bool operator<= (const numeric_iterator& lhs, const numeric_iterator& rhs) {
		return (lhs.__m_val <= rhs.__m_val);
	}

	friend bool operator> (const numeric_iterator& lhs, const numeric_iterator& rhs) {
		return (lhs.__m_val > rhs.__m_val);
	}

	friend bool operator>= (const numeric_iterator& lhs, const numeric_iterator& rhs) {
		return (lhs.__m_val >= rhs.__m_val);
	}

private:
	T __m_val;
};



template<class T>
class numeric_iterator<T, 0>
{
	typedef identity_element< std::multiplies<T> > _IdentityElem;
public:
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T*const pointer;
	typedef const T* const const_pointer;

	typedef std::ptrdiff_t difference_type;
	typedef std::ptrdiff_t distance_type;

	typedef std::random_access_iterator_tag iterator_category;


	numeric_iterator(const T& x, const T& step = _IdentityElem::get()) :
		__m_val(x),
		__m_step(step) {
	}

	numeric_iterator(const numeric_iterator& other) :
		__m_val(other.__m_val),
		__m_step(other.__m_step) {
	}

	numeric_iterator(numeric_iterator&& other) :
		__m_val(std::move(other.__m_val)),
		__m_step(std::move(other.__m_step)) {
	}

	numeric_iterator& operator= (const numeric_iterator& other) {
		__m_val = other.__m_val;
		__m_step = other.__m_step;
		return (*this);
	}

	numeric_iterator& operator= (numeric_iterator&& other) {
		__m_val = std::move(other.__m_val);
		__m_step = std::move(other.__m_step);
		return (*this);
	}

	inline const_reference operator*() const { return __m_val; }

    inline const_pointer operator->() const { return (&__m_val); }

	inline const_reference operator[] (difference_type offset) const {
		return (__m_val + offset * __m_step);
	}

	

	inline numeric_iterator& operator+= (difference_type offset) {
		__m_val += (offset * __m_step);
		return (*this);
	}

	inline numeric_iterator& operator++() {
		__m_val += __m_step;
		return (*this);
	}

	inline numeric_iterator operator++(int) {
		numeric_iterator t(*this);
		__m_val += __m_step;
		return t;
	}

	inline numeric_iterator operator+ (difference_type offset) const {
        numeric_iterator t(*this);
		return (t += offset);
	}


	inline numeric_iterator& operator-= (difference_type offset) {
		__m_val -= (offset * __m_step);
		return (*this);
	}

	inline numeric_iterator& operator--() {
		__m_val -= __m_step;
		return (*this);
	}

	inline numeric_iterator operator--(int) {
		numeric_iterator t(*this);
		__m_val -= __m_step;
		return t;
	}


	inline numeric_iterator operator- (difference_type offset) const {
		numeric_iterator t(*this);
		t -= offset;
		return t;
	}

	inline T operator- (const numeric_iterator& other) const {
        __stdx_assertx(__m_val == other.__m_val, std::logic_error, "iterators stride not equal");
		return ((__m_val - other.__m_val) / __m_step);
	}



	friend bool operator== (const numeric_iterator& lhs, 
							const numeric_iterator& rhs) {
		return (lhs.__m_val == rhs.__m_val);
	}

	friend bool operator!= (const numeric_iterator& lhs, 
							const numeric_iterator& rhs) {
		return !(lhs == rhs);
	}

	friend bool operator< (const numeric_iterator& lhs, 
						   const numeric_iterator& rhs) {
		return (lhs.__m_val < rhs.__m_val);
	}

	friend bool operator<= (const numeric_iterator& lhs, 
							const numeric_iterator& rhs) {
		return (lhs.__m_val <= rhs.__m_val);
	}

	friend bool operator> (const numeric_iterator& lhs, 
						   const numeric_iterator& rhs) {
		return (lhs.__m_val > rhs.__m_val);
	}

	friend bool operator>= (const numeric_iterator& lhs,
							const numeric_iterator& rhs) {
		return (lhs.__m_val >= rhs.__m_val);
	}

private:
	T __m_val, __m_step;
};



_STDX_END


