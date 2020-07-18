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
#include <limits>

#include <iosfwd>
#include <sstream>

#include <string>
#include <tuple>

#include "../platform/common.h"

_STDX_BEGIN

#define _STLEXT_TUPLE_FORMAT_PLACEHOLDER_BUFFER_SIZE (sizeof(_Elem) * (std::numeric_limits<unsigned>::digits10 + 2))

namespace detail
{
	template<int I, int N>
	struct tuple_io
	{
		template<typename _OStream, typename _Tuple>
		static inline _OStream& put(_OStream& stream, const _Tuple& src) {
			typedef typename _OStream::char_type elem_t;
			stream << std::get<I>(src) << (elem_t)' ';
			return (typename tuple_io<I + 1, N>::put(stream, src));
		}

		template<typename _IStream, typename _Tuple>
		static inline _IStream& get(_IStream& stream, _Tuple& src) {
			stream >> std::get<I>(src);
			return (typename tuple_io<I + 1, N>::get(stream, src));
		}
	};

	template<int N>
	struct tuple_io<N, N>
	{
		template<typename _OStream, typename _Tuple>
		static inline _OStream& put(_OStream& stream, const _Tuple& src) {
			return ((_OStream&)(stream << std::get<N>(src)));
		}

		template<typename _IStream, typename _Tuple>
		static inline _IStream& get(_IStream& stream, _Tuple& src) {
			return ((_IStream&)(stream >> std::get<N>(src)));
		}
	};

	template<>
	struct tuple_io<0, 0> {
		template<typename _OStream, typename _Tuple>
		static inline _OStream& put(_OStream& stream, const _Tuple& src) {
			return ((_OStream&)(stream << std::get<0>(src)));
		}

		template<typename _IStream, typename _Tuple>
		static inline _IStream& get(_IStream& stream, _Tuple& src){
			return ((_IStream&)(stream >> std::get<0>(src)));
		}
	};

	template<>
	struct tuple_io<0, -1> {
		template<typename _OStream, typename _Tuple>
		static inline _OStream& put(_OStream& stream, const _Tuple& src) { return (stream); }

		template<typename _IStream, typename _Tuple>
		static inline _IStream& get(_IStream& stream, _Tuple& src) { return (stream); }
	};




	struct tuple_formatter_base
	{
	protected:
		template<typename _Elem>
		static inline void do_format(std::basic_string<_Elem>& source, _Elem* pattern, size_t length, const std::basic_string<_Elem>& what)
		{
			typedef std::basic_string<_Elem> string_type;
			typedef typename std::basic_string<_Elem>::size_type size_type;

			size_type pos = 0;
			while (pos < source.size()) {
				size_type off = source.find(pattern, pos, length);
				if (off != string_type::npos) {
					source.replace(source.begin() + off, source.begin() + (off + length), what.begin(), what.end());
					pos += what.size();
					continue;
				}
				break;
			}
		}
#ifdef STDX_CMPLR_MSVC // disable some warnings on msvc++ compiler
#pragma warning(push,1)
#pragma warning(disable:4996)
#endif
        template<unsigned _Idx>
		static int placeholder(char* buffer) {
            return  snprintf(buffer, _STLEXT_TUPLE_FORMAT_PLACEHOLDER_BUFFER_SIZE, "{%u}", _Idx);
		}
        template<unsigned _Idx>
		static int placeholder(wchar_t* buffer) {
            return  snwprintf(buffer, _STLEXT_TUPLE_FORMAT_PLACEHOLDER_BUFFER_SIZE, L"{%u}", _Idx);
		}
#ifdef STDX_CMPLR_MSVC
#pragma warning(pop)
#endif
	};

	template<int I, int N>
	struct tuple_formatter : public tuple_formatter_base
	{
		template<typename _Tuple, typename _Elem>
		static void apply(const _Tuple& x, std::basic_string<_Elem>& target, _Elem* buffer, std::basic_ostringstream<_Elem>& oss) {
			oss << std::get<I>(x);
			int n = placeholder<I>((_Elem*)buffer);
			do_format(target, buffer, n, oss.str());
			oss.str(std::basic_string<_Elem>());
			typename tuple_formatter<I + 1, N>::apply(x, target, buffer, oss);
		}
	};

	template<int N>
	struct tuple_formatter<N, N> : public tuple_formatter_base
	{
		template<typename _Tuple, typename _Elem>
		static void apply(const _Tuple& x, std::basic_string<_Elem>& target, _Elem* buffer, std::basic_ostringstream<_Elem>& oss) {
			oss << std::get<N>(x);
			int n = placeholder<N>(buffer);
			do_format(target, buffer, n, oss.str());
			oss.str(std::basic_string<_Elem>());
		}
	};

	template<>
	struct tuple_formatter<0, 0> : public tuple_formatter_base
	{
		template<typename _Tuple, typename _Elem>
		static void apply(const _Tuple& x, std::basic_string<_Elem>& target, _Elem* buffer, std::basic_ostringstream<_Elem>& oss) {
			oss << std::get<0>(x);
			int n = placeholder<0>(buffer);
			do_format(target, buffer, n, oss.str());
			oss.str(std::string());
		}
	};

	template<>
	struct tuple_formatter<0, -1>
	{
		template<typename _Tuple, typename _Elem>
		static void apply(const _Tuple&, std::basic_string<_Elem>&, _Elem*, std::basic_ostringstream<_Elem>&) { }
	};

}

/// BASIC TUPLE I/O

template<typename _Elem, typename _Tuple>
inline std::basic_string<_Elem> put_tuple(const _Elem* format, const _Tuple& tpl)
{
	using namespace std;
	typedef detail::tuple_io<0, int(tuple_size<_Tuple>::value) - 1> default_format;
	typedef detail::tuple_formatter<0, int(tuple_size<_Tuple>::value) - 1> formatter_type;
	typedef std::basic_string<_Elem> string_type;
	typedef std::basic_ostringstream<_Elem> stream_type;

    _Elem placeholder[_STLEXT_TUPLE_FORMAT_PLACEHOLDER_BUFFER_SIZE];
	stream_type oss;
	if (format == nullptr) {
		default_format::put(oss, tpl);
		return oss.str();
	}

	string_type result(format);
	typename formatter_type::apply(tpl, result, placeholder, oss);
	return result;
}

template<typename _Elem, typename _Tuple>
inline std::basic_string<_Elem> put_tuple(const std::basic_string<_Elem>& format, const _Tuple& tpl) {
	return put_tuple(format.c_str(), tpl);
}


#undef _STLEXT_TUPLE_FORMAT_PLACEHOLDER_BUFFER_SIZE

_STDX_END
