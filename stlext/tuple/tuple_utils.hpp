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

#include <array>
#include <tuple>
#include <utility>
#include <type_traits>

#include "../platform/common.h"
#include "tuple_algo.hpp"


_STDX_BEGIN



/// TUPLE HASHING

template<class Tuple>
struct tuple_hash;


template<class... _Args>
struct tuple_hash< std::tuple<_Args...> > :
        public std::hash<_Args>...
{
    typedef tuple_hash< std::tuple<_Args...> > this_type;
    typedef size_t               result_type;
    typedef std::tuple<_Args...> argument_type;

    inline result_type operator()(const argument_type& arg) const {
        size_t seed = 0xdeadbeaf;
        foreach_element([this, &seed](auto x) { seed = this_type::__hash_combine(seed, this->__hash_code(x)); }, arg);
        return seed;
    }

private:
    template<class T>
    inline size_t __hash_code(const T& x) const {
        return static_cast<const std::hash<T>&>(*this)(x);
    }

    // Helper function for combining indiviual values of hashed elements
    static inline size_t __hash_combine(size_t seed, size_t hs) {
        static constexpr size_t __ratio_seed = (sizeof(size_t) < 8 ? 0x9e3779b9 : 0xc6a4a7935bd1e995);
        // golden ratio based hash combine from BOOST
        seed ^= hs +__ratio_seed + (seed << 6) + (seed >> 2);
        return seed;
    }
};




/// TUPLE ELEMENT-WISE SELECTING

template<
    size_t _Idx,
    typename _Tuple = void
>
struct select_ith :
        std::unary_function<_Tuple, typename std::tuple_element<_Idx, _Tuple>::type >
{
    typedef std::unary_function<_Tuple, typename std::tuple_element<_Idx, _Tuple>::type > base_type;
    typedef typename base_type::argument_type argument_type;
    typedef typename base_type::result_type result_type;
    inline result_type operator()(const argument_type& x) {
        return std::get<_Idx>(x);
    }
};

template<size_t _Idx>
struct select_ith<_Idx, void>
{
    template<typename _Tuple>
    inline auto operator()(_Tuple&& x)
        -> decltype(std::get<_Idx>(std::forward<_Tuple>(x)))
    {
        return std::get<_Idx>(std::forward<_Tuple>(x));
    }
};


/// TUPLE ELEMENT-WISE COMPARING

template<
    size_t _I, //size_t _J = _I,
    class _Pred = std::less<void>
>
struct nth_comparer : public _Pred
{
    typedef bool result_type;


    template<class... _Args>
    nth_comparer(_Args&&... __args) : _Pred(std::forward<_Args>(__args)...) {}

    template<class... _Args>
    inline bool operator()(const std::tuple<_Args...>& __x,
                           const std::tuple<_Args...>& __y) const
    {
        using namespace std;
        static const size_t SIZE = tuple_size<tuple<_Args...> >::value;
        static_assert(_I < SIZE, "I out of range");
        return _Pred::operator()(get<_I>(__x), get<_I>(__y));
    }


    template<class _First, class _Second>
    inline bool operator()(const std::pair<_First, _Second>& __x,
                           const std::pair<_First, _Second>& __y) const {
        using namespace std;
        static_assert(_I < 2, "I out of range");
        return _Pred::operator()(get<_I>(__x), get<_I>(__y));
    }

    template<class _Tp, size_t _Size>
    inline bool operator()(const std::array<_Tp, _Size>& __x,
                           const std::array<_Tp, _Size>& __y) const {
        using namespace std;
        static_assert(_I < _Size, "I out of range");
        return _Pred::operator()(get<_I>(__x), get<_I>(__y));
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


