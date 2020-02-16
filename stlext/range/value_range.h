// Copyright (c) 2013-2016, Michael Polukarov (Russia).
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
#include <cmath>
#include <utility>
#include <type_traits>

#include "../platform/common.h"
#include "../iterator/numeric_iterator.hpp"
_STDX_BEGIN

template<class T>
struct value_range : 
	public std::pair<T, T>
{
	typedef std::pair<T, T> pair_type;

	static_assert(std::is_arithmetic<T>::value, "type T is not arithmetic type");

	T lower, upper;

	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* pointer;
	typedef const T* const_pointer;

	typedef numeric_iterator iterator;
	typedef numeric_iterator const_iterator;

	value_range() :
		pair_type() {
	}

	value_range(const T& low, const T& high) :
		pair_type(low, high) {
	}

	value_range(const std::pair<T, T>& other) :
		pair_type(other) {
	}

	value_range(std::pair<T, T>&& other) :
		pair_type(other) {
	}

	value_range(const value_range<T>& other) :
		pair_type(other) {
	}

	value_range(value_range<T>&& other) :
		pair_type(other) {
	}

	inline value_range& operator=(const value_range& other) {
		return pair_type::operator=(other);
	}

	inline value_range& operator=(value_range&& other) {
		return pair_type::operator=(other);
	}

	inline explicit operator bool() const { 
		return (valid()); 
	}

	const T& lower() const { return this->first; }
	const T& upper() const { return this->second; }

	iterator begin() { return numeric_iterator<T, 1>(lower); }
	const_iterator begin() const { return numeric_iterator<T, 1>(lower); }
	const_iterator begin(const T& step) const { return numeric_iterator<T>(lower, step); }

	iterator end() { return numeric_iterator<T, 1>(upper); }
	const_iterator end() const { return numeric_iterator<T, 1>(upper); }
	const_iterator end(const T& step) const { return numeric_iterator<T>(upper, step); }


	inline bool valid() const { return (lower < upper); }

	inline bool single() const { return ((upper - lower) == 1); }

	inline bool null() { return (lower == 0 && upper == 0); }

	inline bool empty() const { return ((upper - lower) == 0); }

	inline T size() const { return (valid() ? ((upper - lower) + 1) : 0); }

	inline size_t count(const T& step = T(1)) const {
		return (valid() ? static_cast<size_t>(std::floor((upper - lower) / step)) : 0);
	}

	value_range section(const T& low, size_t count, const T& step = T(1)) const 
	{
		if (low > upper) 
			return value_range();

		T high = std::max(lower, low);
		for (; count-- > 0 || high < upper;)
			high += step;
		return value_range<T>(low, high);
	}


	inline bool touches(const value_range& other) const {
		return (other.upper == lower || other.lower == upper);
	}

	inline bool intersects(const value_range& other) const {
		return (contains(other.lower) || contains(other.upper) ||
			(other.contains(lower) || other.contains(upper)));
	}

	inline bool contains(const value_range& other) const {
		return ((lower <= other.lower) && (other.upper <= upper));
	}

	inline bool contains(const T& value) const {
		return (value >= lower && value < upper);
	}


	inline void resize(T n) { upper += (n - size()); }

	inline void normalize() { if (upper < lower) std::swap(upper, lower); }

	inline value_range intersection(const value_range& other) const
	{
		return (intersects(other) ?
			value_range((std::max)(lower, other.lower), (std::min)(upper, other.upper)) :
			value_range());
	}

	inline value_range& operator&=(const value_range& other)
	{
		if (intersects(other)) {
			lower = (std::max)(lower, other.lower);
			upper = (std::min)(upper, other.upper);
		}
		return (*this);
	}

	inline value_range& operator+=(const T& amount) {
		lower += amount;
		upper += amount;
		return (*this);
	}

	inline value_range& operator-=(const T& amount) {
		lower -= amount;
		upper -= amount;
		return (*this);
	}

	/*inline bool operator==(const value_range& other) const  {
		return ((lower == other.lower) && (upper == other.upper));
	}

	inline bool operator!=(const value_range& other) const {
		return !((*this) == other);
	}

	inline bool operator< (const value_range& other) const {
		return (lower < other.lower || (!(other.lower < lower) && upper < other.upper));
	}

	inline bool operator> (const value_range& other) const {
		return (other < (*this));
	}

	inline bool operator<= (const value_range& other) const {
		return !(other < (*this));
	}

	inline bool operator>= (const value_range& other) const {
		return (!((*this) < other));
	}*/
};


template<class T>
inline value_range<T> operator& (const value_range<T>& lhs, 
								 const value_range<T>& rhs) {
	return lhs.intersection(rhs);
}

template<class T>
inline value_range<T> operator+ (const value_range<T>& range, 
								 const T& amount) {
	value_range<T> result(range);
	return (result += amount);
}

template<class T>
inline value_range<T> operator- (const value_range<T>& range, 
								 const T& amount) {
	value_range<T> result(range);
	return (result -= amount);
}



_STDX_END



