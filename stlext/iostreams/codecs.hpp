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
#include <ios>
#include <iterator>
#include <algorithm>

#include "../platform/common.h"

_STDX_BEGIN

template<
	std::ios::openmode _Flag,
	class _Codec,
	class _OutIt,
	class _Impl
>
class basic_codec : 
	public _Codec,
	public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
	static const std::ios::openmode mode = _Flag;

	typedef _OutIt iterator_type;

	template<class... _Args>
	basic_codec(_OutIt it, _Args... args) :
		_Codec(args...), __m_it(it) {
	}

        basic_codec& operator= (int x) {
		static_cast<_Impl*>(this)->do_assign(x);
		return (*this);
	}

	basic_codec& operator*() {
		return (*this);
	}

	basic_codec& operator++() {
		return (*this);
	}

	basic_codec operator++(int) {
		return (*this);
	}

	basic_codec& flush() {
		_Codec::flush(__m_it);
		return (*this);
	}

	inline operator _OutIt() {
		return __m_it;
	}

	_OutIt __m_it;
};



template<
	class _Codec,
	class _OutIt
>
class basic_decoder : 
	public basic_codec<
		std::ios::out, _Codec, _OutIt,
		basic_decoder<_Codec, _OutIt>
	>
{
	typedef basic_codec<
		std::ios::out, _Codec, _OutIt,
		basic_decoder<_Codec, _OutIt>
	> base_type;

public:
	template<class... _Args>
	basic_decoder(_OutIt it, _Args... args) :
		base_type(it, args...) {
	}

        inline void do_assign(int x) {
		_Codec::decode(x, this->__m_it);
	}
};


template<class _Codec, class _OutIt, class... _Args>
inline basic_decoder<_Codec, _OutIt> make_decoder(_OutIt out, _Args&&... args) {
	return basic_decoder<_Codec, _OutIt>(out, std::forward<_Args>(args)...);
}

template<class _Codec, class _Elem, class _Traits, class... _Args>
inline basic_decoder<_Codec, std::ostreambuf_iterator<_Elem> > make_decoder(std::basic_ostream<_Elem, _Traits>& stream, _Args&&... args) {
	typedef std::ostreambuf_iterator<_Elem, _Traits> iter_type;
	return basic_decoder<_Codec, iter_type>(iter_type(stream), std::forward<_Args>(args)...);
}




template<
	class _Codec,
	class _InIt
>
inline _Codec decode(_InIt first, _InIt last, _Codec c)
{
	static_assert(_Codec::mode == std::ios::out, "decoder mode mismatch");
	c = std::copy(first, last, c);
	//c.flush(); // ???
	return c;
}



template<
	class _Codec,
	class _Elem, 
	class _Traits
>
_Codec decode(std::basic_istream<_Elem, _Traits>& stream, _Codec c)
{
	typedef std::istreambuf_iterator<_Elem, _Traits> iter_type;
	return decode(iter_type(stream), iter_type(), c);
}









template<
	class _Codec,
	class _OutIt
>
class basic_encoder : 
	public basic_codec<
		std::ios::in, _Codec, _OutIt,
		basic_encoder<_Codec, _OutIt>
	>
{
	typedef basic_codec<
		std::ios::in, _Codec, _OutIt,
		basic_encoder<_Codec, _OutIt>
	> base_type;

public:
	template<class... _Args>
	basic_encoder(_OutIt it, _Args... args) :
		base_type(it, args...) {
	}

        inline void do_assign(int x) {
		_Codec::encode(x, this->__m_it);
	}
};

template<class _Codec, class _OutIt, class... _Args>
inline basic_encoder<_Codec, _OutIt> make_encoder(_OutIt out, _Args&&... args) {
	return basic_encoder<_Codec, _OutIt>(out, std::forward<_Args>(args)...);
}

template<class _Codec, class _Elem, class _Traits, class... _Args>
inline basic_encoder<_Codec, std::ostreambuf_iterator<_Elem> > make_encoder(std::basic_ostream<_Elem, _Traits>& stream, _Args&&... args) {
	typedef std::ostreambuf_iterator<_Elem, _Traits> iter_type;
	return basic_encoder<_Codec, iter_type>(iter_type(stream), std::forward<_Args>(args)...);
}



template<
	class _Codec,
	class _InIt
>
_Codec encode(_InIt first, _InIt last, _Codec c)
{
	static_assert(_Codec::mode == std::ios::in, "encoder mode mismatch");
	c = std::copy(first, last, c);
	c.flush();
	return c;
}

template<
	class _Codec,
	class _Elem,
	class _Traits
>
_Codec encode(std::basic_istream<_Elem, _Traits>& stream, _Codec c)
{
	typedef std::istreambuf_iterator<_Elem, _Traits> iter_type;
	return encode(iter_type(stream), iter_type(), c);
}

_STDX_END

