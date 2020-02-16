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
#include <numeric>
#include "iter_range.hpp"

// extended algorithms
#include "../algorithm/algorithm.h"
#include "../algorithm/ext/join.hpp"
#include "../algorithm/ext/split.hpp"


_STDX_BEGIN

// algorothms adapters

/// COMMON ALGORITHMS

template<class _It>
inline typename iter_range<_It>::distance_type distance(const iter_range<_It>& _Range) {
	return _Range.size();
}

template<class _It>
inline iter_range<_It> section(const iter_range<_It>& _Range, size_t _From, size_t _To) {
	return _Range.section(_From, _To);
}



template<
	class _InIt,
	class _Func
>
_Func for_each(iter_range<_InIt>& _Range, _Func _Fn) {
	return std::for_each(_Range.first, _Range.last, _Fn);
}

template<
	class _InIt,
	class _Func
>
_Func for_each(const iter_range<_InIt>& _Range, _Func _Fn) {
	return std::for_each(_Range.first, _Range.last, _Fn);
}




template<
	class _InIt,
	class _OutIt
> 
inline _OutIt copy(const iter_range<_InIt>& _Range, _OutIt _DestIt) {
	return std::copy(_Range.first, _Range.last, _DestIt);
}

template<
	class _InIt,
	class _OutIt,
	class _Fn1
> 
inline _OutIt copy_if(const iter_range<_InIt>& _Range, _OutIt _DestIt, _Fn1 _Pred) {
	return std::copy_if(_Range.first, _Range.last, _DestIt, _Pred);
}

template<
	class _InIt,
	class _OutIt
> 
inline _OutIt copy_backward(const iter_range<_InIt>& _Range, _OutIt _DestIt) {
	return std::copy_backward(_Range.first, _Range.last, _DestIt);
}




template<
	class _InIt,
	class _ValType
> 
inline typename std::iterator_traits<_InIt>::difference_type count(const iter_range<_InIt>& _Range, const _ValType& _Val) {
	return std::count(_Range.first, _Range.last, _Val);
}

template<
	class _InIt,
	class _Fn1
> 
inline typename std::iterator_traits<_InIt>::difference_type count_if(const iter_range<_InIt>& _Range, _Fn1 _Pred) {
	return std::count_if(_Range.first, _Range.last, _Pred);
}





template<
	class _InIt1,
	class _InIt2
>
inline bool equal(const iter_range<_InIt1>& _RangeLhs, const iter_range<_InIt2>& _RangeRhs) {
	if (_RangeLhs.size() != _RangeRhs.size()) return false;
	return std::equal(_RangeLhs.first, _RangeLhs.last, _RangeRhs.first);
}


template<
	class _InIt1,
	class _InIt2
> 
inline bool equal(const iter_range<_InIt1>& _Range, _InIt2 _DestIt) {
	return std::equal(_Range.first, _Range.last, _DestIt);
}

template<
	class _InIt1,
	class _InIt2,
	class _Fn2
> 
inline bool equal(const iter_range<_InIt1>& _Range, _InIt2 _DestIt, _Fn2 _Func) {
	return std::equal(_Range.first, _Range.last, _DestIt, _Func);
}


template<
	class _InIt,
	class _ValType
> 
inline iter_range<_InIt> equal_range(const iter_range<_InIt>& _Range, const _ValType& _Val) {
	return iter_range<_InIt>(std::equal_range(_Range.first, _Range.last, _Val));
}

template<
	class _InIt,
	class _ValType,
	class _CompFn
> 
inline iter_range<_InIt> equal_range(const iter_range<_InIt>& _Range, const _ValType& _Val, _CompFn _Comp) {
	return iter_range<_InIt>(std::equal_range(_Range.first, _Range.last, _Val, _Comp));
}




template<
	class _InIt
>
inline _InIt max_element(const iter_range<_InIt>& _Range) {
	return std::max_element(_Range.first, _Range.last);
}

template<
	class _InIt,
	class _Fn1
>
inline _InIt max_element(const iter_range<_InIt>& _Range, _Fn1 _Pred) {
	return std::max_element(_Range.first, _Range.last, _Pred);
}

template<
	class _InIt
>
inline _InIt min_element(const iter_range<_InIt>& _Range) {
	return std::min_element(_Range.first, _Range.last);
}

template<
	class _InIt,
	class _Fn1
>
inline _InIt min_element(const iter_range<_InIt>& _Range, _Fn1 _Pred) {
	return std::min_element(_Range.first, _Range.last, _Pred);
}

template<
	class _InIt,
	class _ValType
>
inline _InIt find(const iter_range<_InIt>& _Range, const _ValType& _Val) {
	return std::find(_Range.first, _Range.last, _Val);
}

template<
	class _InIt,
	class _Fn1
>
inline _InIt find_if(const iter_range<_InIt>& _Range, _Fn1 _Func) {
	return std::find_if(_Range.first, _Range.last, _Func);
}




template<
	class _InIt1,
	class _InIt2
>
inline _InIt1 search(const iter_range<_InIt1>& _SrcRange, const iter_range<_InIt2> _DstRange) {
	return std::search(_SrcRange.first, _SrcRange.last, _DstRange.first, _DstRange.last);
}

template<
	class _InIt1,
	class _InIt2,
	class _Fn2
>
inline _InIt1 search(const iter_range<_InIt1>& _SrcRange, const iter_range<_InIt2> _DstRange, _Fn2 _Pred) {
	return std::search(_SrcRange.first, _SrcRange.last, _DstRange.first, _DstRange.last, _Pred);
}






template<
	class _It,
	class _Ty
> 
inline void fill(iter_range<_It>& _Range, const _Ty& _Val) {
	return std::fill(_Range.first, _Range.last, _Val);
}



template<
	class _It
> 
inline void sort(iter_range<_It>& _Range) {
	std::sort(_Range.first, _Range.last);
}

template<
	class _It,
	class _Fn1
> 
inline void sort(iter_range<_It>& _Range, _Fn1 _Pred) {
	std::sort(_Range.first, _Range.last, _Pred);
}


template<
	class _It
> 
inline void stable_sort(iter_range<_It>& _Range) {
	std::stable_sort(_Range.first, _Range.last);
}

template<
	class _It,
	class _Fn1
> 
inline void stable_sort(iter_range<_It>& _Range, _Fn1 _Pred) {
	std::stable_sort(_Range.first, _Range.last, _Pred);
}




template<
	class _InIt,
	class _OutIt,
	class _Fn1
> 
inline _OutIt transform(const iter_range<_InIt>& _Range, _OutIt _DestIt, _Fn1 _Func) {
	return std::transform(_Range.first, _Range.last, _DestIt, _Func);
}

template<
	class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Fn2
>
inline _OutIt transform(const iter_range<_InIt1>& _Range, _InIt2 _Second, _OutIt _Dest, _Fn2 _Func) {
	return std::transform(_Range.first, _Range.last, _Second, _Dest, _Func);
}





/// NUMERIC ALGORITHMS


template<
	typename _InIt,
	typename _ValType
>
inline _ValType accumulate(const iter_range<_InIt>& _Range, _ValType _Identity) {
	return std::accumulate(_Range.first, _Range.last, _Identity);
}

template<
	typename _InIt,
	typename _ValType,
	typename _Fn2
>
inline _ValType accumulate(const iter_range<_InIt>& _Range, _ValType _Identity, _Fn2 _Func) {
	return std::accumulate(_Range.first, _Range.last, _Identity, _Func);
}


template<
	typename _InIt1,
	typename _InIt2,
	typename _ValType,
	typename _Fn21,
	typename _Fn22
>
inline _ValType inner_product(const iter_range<_InIt1>& _Range, _InIt2 _First2, _ValType _Identity, _Fn21&& _Func1, _Fn22&& _Func2) {
	return std::inner_product(_Range.first, _Range.last, _First2, _Identity, _Func1, _Func2);
}

template<
	typename _InIt1,
	typename _InIt2,
	typename _ValType
>
inline _ValType inner_product(const iter_range<_InIt1>& _Range, _InIt2 _First2, _ValType _Identity) {
	return std::inner_product(_Range.first, _Range.last, _First2, _Identity);
}


template<
	typename _InIt,
	typename _OutIt,
	typename _Fn2
>
inline _OutIt partial_sum(const iter_range<_InIt>& _Range, _OutIt _Dest, _Fn2 _Func) {
	return std::partial_sum(_Range.first, _Range.last, _Dest, _Func);
}

template<
	typename _InIt,
	typename _OutIt
>
inline _OutIt partial_sum(const iter_range<_InIt>& _Range, _OutIt _Dest) {
	return std::partial_sum(_Range.first, _Range.last, _Dest);
}


/// ALGORITHM EXTENSIONS


template<typename _It, typename _Pr>
inline void insertion_sort(iter_range<_It>& _Range, _Pr _Pred) {
	stdx::insertion_sort(_Range.first, _Range.last, _Pred);
}

template<typename _It, typename _Pr>
inline void insertion_sort(iter_range<_It>& _Range) {
	stdx::insertion_sort(_Range.first, _Range.last);
}



template<typename _It, typename _Fn>
inline void slide_apply(const iter_range<_It>& _Range, size_t width, _Fn _Op, bool _Strict = true) {
	stdx::slide_apply(_Range.first, _Range.last, width, _Op, _Strict);
}

template<typename _It>
inline std::pair<_It, _It> slide_move(const iter_range<_It>& _Range, _It _Pos) {
	stdx::slide_move(_Range.first, _Range.last, _Pos);
}



template<typename _BiIt, typename _Pr>
inline std::pair<_BiIt, _BiIt> gather(const iter_range<_BiIt>& _Range, _BiIt _Pos, _Pr _Pred) {
	stdx::gather(_Range.first, _Range.last, _Pos, _Pred);
}




template<typename _InIt, typename _Pred, typename _Fx>
inline void split(const iter_range<_InIt>& _Range, _Pred pred, _Fx action) {
	stdx::split(_Range.first, _Range.last, pred, action);
}

template<typename _InIt, typename _OutIt, typename _Pred>
inline void split_copy(const iter_range<_InIt>& _Range, _OutIt out, _Pred pred) {
	stdx::split_copy(_Range.first, _Range.last, out, pred);
}



template<typename _InIt, typename _OutIt>
inline _OutIt join(const iter_range<_InIt>& _Range, _OutIt out) {
	stdx::join(_Range.first, _Range.last, out);
}

template<typename _InIt, typename _OutIt, typename _Delim>
inline _OutIt join(const iter_range<_InIt>& _Range, _OutIt out, const _Delim& delim) {
	stdx::join(_Range.first, _Range.last, out, delim);
}

_STDX_END

