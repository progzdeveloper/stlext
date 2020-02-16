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
#include <sstream>
#include <string>

#include <algorithm>
#include <iterator>

#include <tuple>

#include "../../platform/common.h"

_STDX_BEGIN

namespace detail
{
	template<
		typename _SStream,
		typename _InIt,
		typename _Pred,
		typename _Arg
	>
	_InIt __split_element(_SStream& stream, _InIt first, _InIt last, _Pred pred, _Arg& val)
	{
		if (first == last) return last;

		auto it = first;

		// search the start position
		for (; it != last; ++it)
		{
			if (!pred(*it)) {
				first = it;
				break;
			}
		}

		if (first == last)
			return last;

		++it;
		// search the end position
		for (; it != last && !pred(*it);) {
			++it;
		}

		stream.str({ first, it }); // reset internal buffer
		stream >> val; // read value
		stream.clear(); // buffer cleaup
		return it;
	}

	// TODO: replace it with lambda hack
	template<int I, int N>
	struct tuple_split
	{
		template<
			typename _Tuple,
			typename _SStream,
			typename _InIt,
			typename _Pred
		>
		static inline _InIt apply(_Tuple& src, _SStream& stream, _InIt first, _InIt last, _Pred pred) {
			first = __split_element(stream, first, last, pred, std::get<I>(src));
			return (tuple_split<I + 1, N>::apply(src, stream, first, last, pred));
		}
	};

	template<int N>
	struct tuple_split<N, N>
	{
		template<
			typename _Tuple,
			typename _SStream,
			typename _InIt,
			typename _Pred
		>
		static inline _InIt apply(_Tuple& src, _SStream& stream, _InIt first, _InIt last, _Pred pred) {
			return __split_element(stream, first, last, pred, std::get<N>(src));
		}
	};

	template<>
	struct tuple_split<0, 0>
	{
		template<
			typename _Tuple,
			typename _SStream,
			typename _InIt,
			typename _Pred
		>
		static inline _InIt apply(_Tuple& src, _SStream& stream, _InIt first, _InIt last, _Pred pred) {
			return __split_element(stream, first, last, pred, std::get<0>(src));
		}
	};

	template<>
	struct tuple_split<0, -1>
	{
		template<
			typename _Tuple,
			typename _SStream,
			typename _InIt,
			typename _Pred
		>
		static inline _InIt apply(_Tuple& src, _SStream& stream, _InIt first, _InIt last, _Pred pred) { }
	};

} // end namespace detail

/*!
 * \brief split
 * split [first, last) range of elements using 
 * boolean predicate \a pred, and invoke action \a action
 * while passing subrange on it.
 *
 */
template<typename _InIt, typename _Pred, typename _Fx>
void split(_InIt first, _InIt last, _Pred pred, _Fx action)
{
	if (first == last) return;

	_InIt it = first;
	for (;;)
	{
		// search the start position
		for (; it != last; ++it)
		{
			if (!pred(*it)) {
				first = it;
				break;
			}
		}

		if (first == last)
			break;

		++it;
		// search the end position
		for (; it != last && !pred(*it);) {
			++it;
		}

		// apply action to sub range
		action(first, it);
		first = last;
	}

}

/*!
 * \brief split_copy
 * split [first, last) range of elements using
 * boolean predicate \a pred, and copy result into out
 *
 */
template<typename _InIt, typename _OutIt, typename _Pred>
_OutIt split_copy(_InIt first, _InIt last, _OutIt out, _Pred pred)
{
	if (first == last) return out;

	_InIt it = first;
	for (;;)
	{
		// search the start position
		for (; it != last; ++it)
		{
			if (!pred(*it)) {
				first = it;
				break;
			}
		}

		if (first == last)
			break;

		++it;
		// search the end position
		for (; it != last && !pred(*it);) {
			++it;
		}

		// get the sub range and 
		// create result using 
		// brace initialization
		*out = { first, it };
		first = last;
	}
	return out;
}


/*!
 * \brief split_copy
 * split container \a c using
 * boolean predicate \a pred, and copy result into out
 *
 */
template<typename _Container, typename _OutIt, typename _Pred>
inline _OutIt split_copy(const _Container& c, _OutIt out, _Pred pred) {
	return split_copy(std::begin(c), std::end(c), out, pred);
}

/*!
 * \brief split_copy
 * split fixed size array \a input
 * boolean predicate \a pred, and copy result into out
 *
 */
template<typename _Tx, size_t _Size, typename _OutIt, typename _Pred>
inline _OutIt split_copy(const _Tx (&input)[_Size], _OutIt out, _Pred pred) {
	return split_copy(std::begin(input), std::end(input), out, pred);
}




/*!
 * \brief split_into
 * split string \a line using
 * boolean predicate \a pred, and copy results into \a args tuple
 *
 */
template<
	typename _Elem,
	typename _Traits,
	typename _Alloc,
	typename _Pred,
	typename... _Args
>
inline void split_into(const std::basic_string<_Elem, _Traits, _Alloc>& line, _Pred pred, std::tuple<_Args...>& args)
{
	using namespace std;
	typedef basic_istringstream<_Elem, _Traits, _Alloc> stream_type;
	typedef tuple<_Args...> tuple_type;
	typedef detail::tuple_split<0, int(tuple_size<tuple_type>::value) - 1> visitor;

	stream_type stream;
	visitor::apply(args, stream, begin(line), end(line), pred);
}



/*!
 * \brief split_into
 * split literal string \a line using
 * boolean predicate \a pred, and copy results into \a args tuple
 *
 */
template<
	typename _Elem,
	size_t _Size,
	typename _Pred,
	typename... _Args
>
inline void split_into(const _Elem (&line)[_Size], _Pred pred, std::tuple<_Args...>& args)
{
	using namespace std;
	typedef basic_istringstream< _Elem, char_traits<_Elem> > stream_type;
	typedef tuple< _Args... > tuple_type;
	typedef detail::tuple_split<0, int(tuple_size<tuple_type>::value) - 1> visitor;

	stream_type stream;
	visitor::apply(args, stream, begin(line), end(line), pred);
}


/*!
 * \brief split_into
 * split string \a line using
 * boolean predicate \a pred, and copy results into \a args tuple
 *
 */
template <
	typename _Elem,
	typename _Traits,
	typename _Alloc,
	typename _Pred,
	typename... _Args
>
inline void split_into(const std::basic_string<_Elem, _Traits, _Alloc>& line, _Pred pred, _Args&... args)
{
	auto packed_args = std::forward_as_tuple(args...);
	split_into(line, pred, packed_args);
}


/*!
 * \brief split_into
 * split literal string \a line using
 * boolean predicate \a pred, and copy results into \a args tuple
 *
 */
template<
	typename _Elem,
	size_t _Size,
	typename _Pred,
	typename... _Args
>
inline void split_into(const _Elem(&line)[_Size], _Pred pred, _Args&... args)
{
	auto packed_args = std::forward_as_tuple(args...);
	split_into(line, pred, packed_args);
}


_STDX_END

