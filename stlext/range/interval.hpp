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
#include <stdexcept>
#include <type_traits>

#include "../platform/common.h"
#include "../iterator/numeric_iterator.hpp"

_STDX_BEGIN



template<class T>
struct interval :
    public std::pair<T, T>
{
    typedef std::pair<T, T> base_type;

    static_assert(std::is_arithmetic<T>::value, "type T is not arithmetic type");

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef numeric_iterator<T,1> iterator;
    typedef numeric_iterator<T,1> const_iterator;

    interval() :
        base_type() {
    }

    interval(const_reference low, const_reference high) :
        base_type(low, high) {
    }

    interval(const base_type& other) :
        base_type(other) {
    }

    interval(base_type&& other) :
        base_type(std::forward<base_type>(other)) {
    }

    interval(const interval& other) :
        base_type(other) {
    }

    interval(interval&& other) :
        base_type(std::forward<base_type>(other)) {
    }

    inline interval& operator=(const interval& other) {
        return interval::operator=(other);
    }

    inline interval& operator=(interval&& other) {
        return interval::operator=(other);
    }

    interval& assign(const_reference low, const_reference high) {
        this->first = low;
        this->second = high;
        return (*this);
    }


    iterator begin() { return iterator(this->first); }
    const_iterator begin() const { return const_iterator(this->first); }
    numeric_iterator<T> begin(const T& step) const { return numeric_iterator<T>(this->first, step); }

    iterator end() { return iterator(this->second); }
    const_iterator end() const { return const_iterator(this->second); }
    numeric_iterator<T> end(const T& step) const { return numeric_iterator<T>(this->second, step); }

    inline explicit operator bool() const {
        return (valid());
    }

    const_reference lower() const { return this->first; }
    const_reference upper() const { return this->second; }

    reference lower() { return this->first; }
    reference upper() { return this->second; }

    value_type median() const {
        return static_cast<value_type>(std::round((this->first + this->second) / 2.0));
    }

    value_type norm() const { return std::abs(this->second); }

    inline bool valid() const { return (this->first < this->second); }

    inline bool single() const { return ((this->second - this->first) == 1); }

    inline bool null() { return (this->first == 0 && this->second == 0); }

    inline bool empty() const { return ((this->second - this->first) == 0); }

    inline value_type size() const { return (valid() ? ((this->second - this->first) + 1) : 0); }

    inline size_t count(const T& step = T(1)) const {
        return (valid() ? static_cast<size_t>(std::floor((this->second - this->first) / step)) : 0);
    }

    interval subset(const T& low, size_t count, const T& step = T(1)) const
    {
        if (low > this->second)
            return interval();

        T high = std::max(this->first, low);
        for (; count-- > 0 || high < this->second;)
            high += step;
        return interval<T>(low, high);
    }

    inline interval& normalize()
    {
        if (this->second < this->first)
            std::swap(this->second, this->first);
        return (*this);
    }

    // relative location functions

    inline bool touches(const interval& other) const {
        return (other.second == this->first || other.first == this->second);
    }

    inline bool overlaps(const interval& other) const {
        return (contains(other.first) || contains(other.second) ||
                (other.contains(this->first) || other.contains(this->second)));
    }

    inline bool contains(const interval& other) const {
        return ((this->first <= other.first) && (other.second <= this->second));
    }

    inline bool contains(const T& value) const {
        return (value >= this->first && value <= this->second);
    }

    // set-like functions

    inline interval intersect(const interval& other) const
    {
        return (overlaps(other) ?
            interval((std::max)(this->first, other.first),
                     (std::min)(this->second, other.second)) :
            interval());
    }

    inline interval& operator&=(const interval& other)
    {
        if (overlaps(other)) {
            this->first = (std::max)(this->first, other.first);
            this->second = (std::min)(this->second, other.second);
        }
        return (*this);
    }

    inline interval unite(const interval& other) const
    {
        return (overlaps(other) ?
            interval((std::min)(this->first, other.first),
                     (std::max)(this->second, other.second)) :
            interval());
    }

    inline interval& operator|=(const interval& other)
    {
        if (overlaps(other)) {
            this->first = (std::min)(this->first, other.first);
            this->second = (std::max)(this->second, other.second);
        }
        return (*this);
    }


    // interval arithmetic functions

    inline interval& operator+=(const T& amount) {
        this->first += amount;
        this->second += amount;
        return (*this);
    }

    inline interval& operator-=(const T& amount) {
        this->first -= amount;
        this->second -= amount;
        return (*this);
    }

    inline interval& operator*=(const T& amount) {
        this->first *= amount;
        this->second *= amount;
        return (*this);
    }

    inline interval& operator/=(const T& amount) {
        if (amount == 0)
            throw std::logic_error("divide by zero");
        this->first /= amount;
        this->second /= amount;
        return (*this);
    }


    inline interval& operator+=(const interval& other) {
        // [a, b] + [c, d] = [a + c, b + d]
        this->first += other.first;
        this->second += other.second;
        return (*this);
    }

    inline interval& operator-=(const interval& other) {
        // [a, b] - [c, d] = [a - d, b - c]
        this->first -= other.second;
        this->second -= other.first;
        return (*this);
    }

    inline interval& operator*=(const interval& other)
    {
        // [a, b] * [c, d] = [min(a*c, a*d, b*c, b*d), max(a*c, a*d, b*c, b*d)]
        this->first = std::min({this->first  * other.first,
                                this->first  * other.second,
                                this->second * other.first,
                                this->second * other.second});

        this->second = std::max({this->first  * other.first,
                                 this->first  * other.second,
                                 this->second * other.first,
                                 this->second * other.second});
        return (*this);
    }

    inline interval& operator/=(const interval& other)
    {
        // [a, b] * [c, d] = [min(a/c, a/d, b/c, b/d), max(a/c, a/d, b/c, b/d)]
        // c, d != 0

        if (other.first == 0 || other.second == 0)
            throw std::logic_error("divide by zero");

        this->first = std::min({this->first  / other.first,
                                this->first  / other.second,
                                this->second / other.first,
                                this->second / other.second});

        this->second = std::max({this->first  / other.first,
                                 this->first  / other.second,
                                 this->second / other.first,
                                 this->second / other.second});
        return (*this);
    }

    /* std::pair<T, T> already defines this operators
    inline bool operator==(const base_type& other) const  {
        return ((this->first == other.first) && (this->second == other.second));
    }

    inline bool operator!=(const base_type& other) const {
        return !((*this) == other);
    }

    inline bool operator< (const value_range& other) const {
        return (this->first < other.first || (!(other.first < this->first) && this->second < other.second));
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
inline interval<T> operator& (const interval<T>& lhs,
                              const interval<T>& rhs) {
    return lhs.intersect(rhs);
}

template<class T>
inline interval<T> operator| (const interval<T>& lhs,
                              const interval<T>& rhs) {
    return lhs.unite(rhs);
}


template<class T>
inline interval<T> operator+ (const interval<T>& range,
                              const T& amount) {
    interval<T> result(range);
    return (result += amount);
}

template<class T>
inline interval<T> operator- (const interval<T>& range,
                              const T& amount) {
    interval<T> result(range);
    return (result -= amount);
}

template<class T>
inline interval<T> operator* (const interval<T>& range,
                              const T& amount) {
    interval<T> result(range);
    return (result *= amount);
}

template<class T>
inline interval<T> operator/ (const interval<T>& range,
                              const T& amount) {
    interval<T> result(range);
    return (result /= amount);
}



template<class T>
inline interval<T> operator+ (const interval<T>& lhs,
                              const interval<T>& rhs) {
    interval<T> result(lhs);
    return (result += rhs);
}

template<class T>
inline interval<T> operator- (const interval<T>& lhs,
                              const interval<T>& rhs) {
    interval<T> result(lhs);
    return (result -= rhs);
}


template<class T>
inline interval<T> operator* (const interval<T>& lhs,
                              const interval<T>& rhs) {
    interval<T> result(lhs);
    return (result *= rhs);
}

template<class T>
inline interval<T> operator/ (const interval<T>& lhs,
                              const interval<T>& rhs) {
    interval<T> result(lhs);
    return (result /= rhs);
}



_STDX_END



