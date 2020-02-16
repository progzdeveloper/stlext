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

#include <tuple>
#include <utility>
#include <type_traits>

#include "../platform/common.h"
#include "tuple_algo.hpp"


_STDX_BEGIN

// Helper function for combining indiviual values of hashed elements
template<typename T>
inline void hash_combine(size_t& seed, const T& x) {
	static constexpr size_t __ratio_seed = 0x9e3779b9;
	// golden ratio based hash combine from BOOST
	seed ^= std::hash<T> {}(x) +__ratio_seed + (seed << 6) + (seed >> 2);
}


struct hash_combiner
{
	static const size_t __hash_seed = 0xdeadbeaf;
	
	hash_combiner() :
		seed(__hash_seed) {
	}

	hash_combiner(size_t s) : 
		seed(s) {
	}

	template<typename T>
	inline void operator()(size_t, const T& x) {
		hash_combine(seed, x);
	}

	size_t seed;
};


/// TUPLE HASHING

template<typename T>
struct tuple_hash :
	public std::hash<T>
{
	inline result_type operator()(const argument_type& arg) const {
		return std::hash<T>::operator()(arg);
	}
};


template<typename... _Args>
struct tuple_hash< std::tuple<_Args...> > :
	hash_combiner,
	std::unary_function<std::tuple<_Args...>, size_t>
{
	inline result_type operator()(const argument_type& arg) const {
		seed = __hash_seed;
		::stdx::foreach_element(static_cast<const hash_combiner&>(*this), arg);
		return seed;
	}
};




/// TUPLE ELEMENT-WISE SELECTING

template<
	size_t _Idx, 
	typename _Tuple = void
>
struct tuple_selector :
	std::unary_function<_Tuple, typename std::tuple_element<_Idx, _Tuple>::type >
{
	inline result_type operator()(const argument_type& x) {
		return std::get<_Idx>(x);
	}
};

template<size_t _Idx>
struct tuple_selector<_Idx, void>
{
	template<typename _Tuple>
	inline typename std::tuple_element<_Idx, _Tuple >::type&
		operator()(const _Tuple& x) {
		return std::get<_Idx>(x);
	}
};


/// TUPLE ELEMENT-WISE COMPARING

template<
	size_t I,
	class _Pred = std::less<void>
>
struct nthcomparer :
	public _Pred
{
	typedef bool result_type;


	template<typename... _Args>
	nthcomparer(_Args&&... __args) : _Pred(std::forward<_Args>(__args)...) {}

#if 0
	template<class T>
	inline bool operator()(const T& __x, const T& __y) const {
		using namespace std;
		return _Pred::operator()(get<I>(__x), get<I>(__y));
	}
#endif

	template<class... _Args>
	inline bool operator()(const std::tuple<_Args...>& __x, 
						   const std::tuple<_Args...>& __y) const
	{
		using namespace std;
		static const size_t SIZE = tuple_size<tuple<_Args...> >::value;
		static_assert(I < SIZE, "I out of range");
		return _Pred::operator()(get<I>(__x), get<I>(__y));
	}


	template<class _First, class _Second>
	inline bool operator()(const std::pair<_First, _Second>& __x, 
						   const std::pair<_First, _Second>& __y) const {
		using namespace std;
		static_assert(I < 2, "I out of range");
		return _Pred::operator()(get<I>(__x), get<I>(__y));
	}

};

#if 0

	/// GET FUNCTIONS FOR std::complex<T>
template<size_t I, class T, size_t N>
T& get(T(&_Arr)[N])
{	// return element at I in array _Arr
	static_assert(I < N, "array index out of bounds");
	return (_Arr[I]);
}

template<size_t I, class T, size_t N>
const T& get(const T(&_Arr)[N])
{	// return element at I in array _Arr
	static_assert(I < N, "array index out of bounds");
	return (_Arr[I]);
}

template<size_t I, class T, size_t N>
T&& get(T(&&_Arr)[N])
{	// return element at I in array _Arr
	static_assert(I < N, "array index out of bounds");
	return (std::move(_Arr[I]));
}


template<size_t I, class T>
T& get(std::complex<T>& c) {
	static_assert(I < 2, "index out of bounds");
	typedef  T(&_Arr)[2];
	return get<I>(reinterpret_cast<_Arr&>(c));
}

template<size_t I, class T>
const T& get(const std::complex<T>& c) {
	static_assert(I < 2, "index out of bounds");
	typedef  T(&_Arr)[2];
	return get<I>(reinterpret_cast<const _Arr&>(c));
}

template<size_t I, class T>
T&& get(std::complex<T>&& c) {
	static_assert(I < 2, "array index out of bounds");
	typedef T(&&_Arr)[2];
	return get<I>(reinterpret_cast<_Arr&&>(c));
}
#endif

_STDX_END


