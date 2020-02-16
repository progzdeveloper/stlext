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

#include "features.hpp"

_STDX_BEGIN

template<
	class T,
	class... _Features
>
class collector :
	public mixin<_Features...>
{
public:
	static_assert(mixin<_Features...>::N != 0, 
				  "unable to define collector with no features");

	static constexpr typename stdx::counter<T>::size_type zero_count = stdx::counter<T>::ident_elem;

	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::tuple<_Features...> tuple_type;

	template<class _Feature, class T>
	struct result_type {
		typedef typename std::common_type<
			T, typename _Feature::result_type
		>::type type;
	};

public:

	void insert(const_reference x) {
		do_insert(x, seq_type());
	}

	template<class _Weight>
	void insert(const_reference x, const _Weight& w) {
		do_insert(x * w, seq_type());
	}

	void emplace(T&& x) {
		do_insert(std::forward<T>(x), seq_type());
	}

	collector& operator()(const_reference x) {
		insert(x);
		return (*this);
	}

	collector& operator<< (const_reference x) {
		return (*this)(x);
	}

	collector& operator+= (const_reference x) {
		return (*this)(x);
	}

	collector& operator+= (const collector& other) {
		do_merge(other, seq_type());
		return (*this);
	}

	void clear() {
		do_reset(seq_type());
	}

	bool empty() const {
		return (this->count() == zero_count);
	}

	auto mean() const
	{
		typedef typename result_type<counter<T>, double>::type type;
		auto n = this->count();
		// make sure that size is not zero
		return (n == zero_count ? type(0) : (this->sum() / ((type)n)));
	}

	auto variance() const 
	{
		typedef typename result_type<variance_value<T>, T>::type type;
		auto n = this->count();
		// make sure that size is not zero
		return (n == zero_count ? type(0) : (this->mean_sum() / ((type)n)));
	}

	auto stddev() const {
		return std::sqrt(variance());
	}

	auto normalized(const T& val) const
	{
		typedef typename result_type<variance_value<T>, T>::type type;
		auto n = this->count();
		// make sure that size is not zero
		return (n == zero_count ? type(0) : (((type)val - mean()) / stddev()));
	}

private:
	template<class _Arg, size_t... _Idx>
	inline void do_insert(_Arg&& arg, std::index_sequence<_Idx...>) {
		int dummy[] = { 0, (at<_Idx>(*this)(std::forward<_Arg>(arg), *this), 0)... };
		(void)dummy;
	}

	template<size_t... _Idx>
	inline void do_reset(std::index_sequence<_Idx...>) {
		int dummy[] = { 0, (at<_Idx>(*this).reset(), 0)... };
		(void)dummy;
	}

	template<size_t... _Idx>
	inline void do_merge(const collector& other, std::index_sequence<_Idx...>) {
		int dummy[] = { 0, ((at<_Idx>(*this) += at<_Idx>(other)), 0)... };
		(void)dummy;
	}
};


template<class T, class... _Features>
inline collector<T, _Features...> operator+ (const collector<T, _Features...>& lhs,
					   const collector<T, _Features...>& rhs) {
	collector<T, _Features...> tmp(lhs);
	return (tmp += rhs);
}


template<
	class T,
	class U = T
>
struct normalizer
{
	U _mean, _inv_sd;
public:
	typedef T argument_type;
	typedef T result_type;

	normalizer(const U& m, const U& s) :
		_mean(m), _inv_sd(s) {
		_inv_sd = U(1) / _stddev;
	}

	template<class _Collector>
	explicit normalizer(const _Collector& c) :
		_mean(c.mean()), _inv_sd(c.stddev()) {
		_inv_sd = U(1) / _inv_sd;
	}

	inline T operator()(const T& x) const {
		return static_cast<T>(((U)x - _mean) * _inv_sd);
	}
};




_STDX_END

