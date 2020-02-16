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
#include <cstdint>
#include <functional>
#include <algorithm>

#include "../components/mixin.h"
#include "identity_element.hpp"
#include "power.hpp"

_STDX_BEGIN


template<
	class T,
	class _Size = uintmax_t
>
struct counter
{
	static constexpr _Size ident_elem = 0;

	typedef _Size size_type;
	typedef _Size result_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	counter() : value(ident_elem) {}

	template<class _Collector>
	void operator()(const_reference x, const _Collector&) {
		++value;
	}

	counter& operator+=(const counter& other) {
		value += other.value;
		return (*this);
	}

	result_type count() const {
		return value;
	}

	void reset() {
		value = ident_elem;
	}
private:
	result_type value;
};



template<
	class T,
	class _Op = std::plus<T> 
>
struct sum_value :
	public mixin<_Op>
{
	static constexpr T ident_elem = identity_element<_Op>::value;

	typedef T result_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	sum_value() : value(ident_elem) {}

	template<class _Collector>
	void operator()(const_reference x, const _Collector&) {
		value = as<_Op>()(value, x);
	}

	sum_value& operator+=(const sum_value& other) {
		value += other.value;
		return (*this);
	}

	result_type sum() const {
		return value;
	}

	void reset() {
		value = ident_elem;
	}
private:
	result_type value;
};


template<
	class T, 
	size_t N,
	class _Mul = std::multiplies<T>
>
class moment :
	public mixin<_Mul>
{
	static constexpr T ident_elem = identity_element<_Add>::value;

	typedef T result_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	template<class _Collector>
	void operator()(const_reference x, _Collector&) {
		value += stdx::power(x, N, as<_Mul>());
	}

	result_type result() const {
		return value;
	}

private:
	result_type value;
};




template<
	class T,
	class _Add = std::plus<T>,
	class _Mul = std::multiplies<T>
>
struct variance_value : 
	public mixin<_Add, _Mul>
{
	static constexpr T ident_elem = identity_element<_Add>::value;

	typedef T result_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	variance_value() :
		value(ident_elem),
		m(ident_elem) {
	}

	template<class _Collector>
	void operator()(const_reference x, _Collector& c) 
	{
		/*
		double prev_mean = m;
		m = c.mean<double>();
		value += (x - m) * (x - prev_mean);
		*/
		T prev_mean = m;
		m = c.mean();
		auto t1 = as<_Add>()(x, -m);
		auto t2 = as<_Add>()(x, -prev_mean);
		value = as<_Add>()(value, as<_Mul>()(t1, t2));
	}

	variance_value& operator+=(const variance_value& other) {
		value += other.value;
		return (*this);
	}

	void reset() {
		value = ident_elem;
		m = ident_elem;
		
	}

protected:
	result_type mean_sum() const {
		return value;
	}

private:
	T value;
	T m;
};



template<
	class T,
	class _Comp = std::less<T>
>
struct min_value : 
	public mixin<_Comp>
{
#ifndef STDX_CMPLR_INTEL
	static constexpr T ident_elem = (std::numeric_limits<T>::max)();
#else
	static constexpr T ident_elem = ((std::numeric_limits<T>::max))();
#endif
	typedef T result_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	min_value() : 
		value(ident_elem) {
	}

	template<class _Collector>
	void operator()(const_reference x, const _Collector&) {
		value = (std::min)(x, value, as<_Comp>());
	}

	min_value& operator+=(const min_value& other) {
		value = (std::min)(value, other.value, as<_Comp>());
		return (*this);
	}

	result_type minimum() const {
		return value;
	}

	void reset() {
		value = ident_elem;
	}
private:
	result_type value;
};



template<
	class T,
	class _Comp = std::less<T>
>
struct max_value :
	public mixin<_Comp>
{
#ifndef STDX_CMPLR_INTEL
	static constexpr T ident_elem = (std::numeric_limits<T>::min)();
#else
	static constexpr T ident_elem = ((std::numeric_limits<T>::min))();
#endif
	typedef T result_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	max_value() :
		value(ident_elem) {
	}

	template<class _Collector>
	void operator()(const_reference x, const _Collector&) {
		value = (std::max)(x, value, as<_Comp>());
	}

	max_value& operator+=(const max_value& other) {
		value = (std::max)(value, other.value, as<_Comp>());
		return (*this);
	}

	result_type maximum() const {
		return value;
	}

	void reset() {
		value = ident_elem;
	}
private:
	result_type value;
};

_STDX_END

