// Copyright (c) 2016-2017, Michael Polukarov (Russia).
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
#include <type_traits>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <string>
#include <chrono>
#include <utility>
#include <limits>
#include <cmath>
#ifdef _WIN32
#include <io.h>
#define fileno _fileno
#define isatty _isatty
#else
#include <unistd.h>
#endif

#include "../platform/common.h"
#include "../iostreams/utility.hpp"

_STDX_BEGIN

namespace detail
{
	template<class _Elem>
	struct format_traits;

	template<>
	struct format_traits < char >
	{
		static constexpr const char* pfmt() { return "%p"; }
		static constexpr const char* sfmt() { return "%s"; }
		static constexpr const char* efmt() { return "%e"; }
		static constexpr const char* cfmt() { return "%c"; }
		static constexpr const char* tfmt() { return "%t"; }
		static constexpr const char* format()  { return "%p%"; }
	};

	template<>
	struct format_traits < wchar_t >
	{
		static constexpr const wchar_t* pfmt() { return L"%p"; }
		static constexpr const wchar_t* sfmt() { return L"%s"; }
		static constexpr const wchar_t* efmt() { return L"%e"; }
		static constexpr const wchar_t* cfmt() { return L"%c"; }
		static constexpr const wchar_t* tfmt() { return L"%t"; }
		static constexpr const wchar_t* format()  { return L"%p%"; }
	}; 
}



template<
	class _Value,
	class _Duration = std::chrono::seconds,
	class _Char = char,
	class _Traits = std::char_traits<_Char>,
	class _Allocator = std::allocator < _Char >
>
class basic_progress_viewer
{
	static_assert(std::is_integral<_Value>::value,
		"value type is not an integer-like type");

public:
	typedef _Value value_type;
	typedef _Value& reference;
	typedef _Value const_reference;

	typedef _Char char_type;
	typedef _Traits traits_type;
	typedef size_t size_type;

	typedef std::basic_streambuf<_Char, _Traits> streambuf_type;
	typedef std::basic_ostream<_Char, _Traits> stream_type;
	typedef std::basic_string<_Char, _Traits, _Allocator> string_type;
	typedef std::pair<value_type, value_type> range_type;

	typedef _Duration duration_type;
	typedef std::chrono::system_clock clock_type;
	typedef std::chrono::time_point<clock_type> timepoint_t;

	basic_progress_viewer(value_type lower = 0, value_type upper = 0,
				const string_type& format = detail::format_traits<_Char>::format(),
				int filler = '*', size_type max_length = 32) 
	{
		__construct(std::make_pair(lower, upper), format, filler, max_length);
	}

	basic_progress_viewer(const range_type& range,
                const string_type& format = detail::format_traits<_Char>::format(),
				int filler = '*', size_type max_length = 32)
	{
		__construct(range, format, filler, max_length);
	}

	void reset(value_type lower, value_type upper, const string_type& format = detail::format_traits<_Char>::format(),
		   int filler = '*', size_type max_length = 32) 
	{
		__construct(std::make_pair(lower, upper), format, filler, max_length);
	}
	
	void reset(const range_type& range, const string_type& format = detail::format_traits<_Char>::format(),
		   int filler = '*', size_type max_length = 32) 
	{
		__construct(range, format, filler, max_length);
	}

	inline void precision(size_type __val) { 
		__m_prec = __val; 
		__repaint();
	}

	inline size_type precision() const { 
		return __m_prec; 
	}

	inline void noflush(bool on = true) {
		__m_noflush = on;
	}

	inline bool noflush() const {
		return __m_noflush;
	}

	inline void fill(int __ch) { 
		__m_filler = traits_type::to_char_type(__ch);
		__repaint();
	}
	inline int fill() const {
		return __m_filler;
	}


	inline void format(const string_type& __fmt) {
		__m_format = __fmt;
		__repaint();
	}
	inline const string_type& format() const { 
		return __m_format; 
	}

	inline const string_type& str() const {
		return __m_buffer;
	}

	inline size_type length() const { return __m_length; }
	inline value_type size() const { return __m_total; }

	inline value_type lower_bound() const { return __m_range.first; }
	inline value_type upper_bound() const { return __m_range.second; }

	inline duration_type elapsed() const {
		using std::chrono::duration_cast;
		return duration_cast<_Duration>(clock_type::now() - __m_timestamp);
	}

	inline double progress() const {
		return (__empty() ? std::numeric_limits<double>::infinity() : __m_progress);
	}

	inline value_type value() const { return __m_value; }

	const basic_progress_viewer& operator()(value_type __x) const 
	{
		if ((__m_value == __x) || (!__contains(__x) && !__empty())) {
			__m_repaint = true;
			return (*this);
		}

		__m_repaint = true;
		if (__m_timestamp == timepoint_t(timepoint_t::duration(0)))
			__m_timestamp = clock_type::now();

		__m_value = __x;
		__repaint();
		return (*this);
	}

	inline stream_type& render(stream_type& __stream) const {
		if (__m_repaint) __stream << __m_buffer;
		return __flush(__stream);
	}

	friend inline stream_type& operator<< (stream_type& __stream, const basic_progress_viewer& __viewer) {
		return __viewer.render(__stream);
	}

private:
	void __construct(const range_type& __range, const string_type& __format,
					int __filler, size_type __max_length)
	{
		__m_noflush  = false;
		__m_range    = __range;
		__m_total    = __range.second - __range.first;
		__m_format   = __format;
		__m_filler   = traits_type::to_char_type(__filler);
		__m_length   = __max_length;
		__m_value    = 0;
		__m_progress = 0;
		__m_prec     = 0;
		__m_timestamp = timepoint_t(timepoint_t::duration(0));
		__m_buffer.resize(__max_length, char_type(' '));
		__m_buffer += __m_format;
		__format_text();
		__m_text_size = __m_buffer.size() - __m_length;
	}

	void __repaint() const
	{
		using namespace std;

		if (__empty()) {
			++__m_value;
			__m_value = (__m_value) % (sizeof(frames)/ sizeof(frames[0]));
			__m_buffer[0] = traits_type::to_char_type(frames[__m_value]);
		}
		else {
			__m_progress = (double(__m_value) - __m_range.first) * 100.0 / double(__m_total);
			std::fill_n(__m_buffer.begin(), (size_type)(__m_progress * (__m_length / 100.0)), __m_filler);
			__format_text();
		}
	}

	/*
	format specification
	%p - percentage
	%s - speed
	%e - estimation
	%c - current
	%t - total
	*/

	void __format_text() const
	{
		using namespace std;
		typedef detail::format_traits<_Char> __fmt_traits;
		if (__empty()) {
			__m_buffer.erase(__m_length, __m_text_size);
			return;
		}

		size_t n = __m_buffer.size();
		auto it = copy(__m_format.begin(), __m_format.end(), __m_buffer.begin() + __m_length);
		__m_buffer.erase(it, __m_buffer.end());

		double time = (double)(elapsed().count());
		double speed = __m_value / time;
		size_t estimate = static_cast<size_t>(std::ceil((__m_total - __m_value) / speed));
		__format_aspect(__m_length, __fmt_traits::pfmt(), __m_progress);
		__format_aspect(__m_length, __fmt_traits::sfmt(), speed);
		__format_aspect(__m_length, __fmt_traits::efmt(), (value_type)estimate);
		__format_aspect(__m_length, __fmt_traits::cfmt(), __m_value);
		__format_aspect(__m_length, __fmt_traits::tfmt(), __m_total);

		if (__m_buffer.size() < n)
			__m_buffer.resize(__m_buffer.size() + (n - __m_buffer.size()), char_type(' '));
	}

	template<class T>
	void __format_aspect(size_t __offset, const string_type& __pattern, T __x) const
	{
		typedef typename string_type::size_type pos_t;
		for (pos_t pos = __offset; (pos = __m_buffer.find(__pattern, pos)) != string_type::npos;) {
			pos += __format_value(__m_buffer.begin() + pos, __x);
			__m_buffer.erase(pos, __pattern.size());
		}
	}

	size_t __format_value(typename string_type::iterator __it, double __x) const
	{
		using namespace std;

		typedef insert_iterator<string_type> iterator;
		typedef num_put<char_type, iterator> number_format;

		static locale loc("");
		if (!has_facet<number_format>(loc)) {
			loc = locale(loc, new number_format);
		}
		const number_format& formatter = use_facet<number_format>(loc);

		size_t n = __m_buffer.size();
		stream_type output((streambuf_type*)0);
		output.setf(ios::fixed);
		output.precision(__m_prec);
		formatter.put(iterator(__m_buffer, __it), output, char_type(' '), __x);
		return (__m_buffer.size() - n);
	}

	size_t __format_value(typename string_type::iterator __it, value_type __x) const
	{
		using namespace std;
		// ###
		typedef typename conditional<
			is_signed<value_type>::value,
			long long, unsigned long long
		>::type type;

		typedef insert_iterator<string_type> iterator;
		typedef num_put<char_type, iterator> number_format;
		
		static locale loc("");
		if (!has_facet<number_format>(loc)) {
			loc = locale(loc, new number_format);
		}
		const number_format& formatter = use_facet<number_format>(loc);

		size_t n = __m_buffer.size();
		size_t w = count_digits<10>(__m_total);
		stream_type output((streambuf_type*)0);
		output.setf(ios::fixed);
		output.width(w);
		formatter.put(iterator(__m_buffer, __it), output, char_type(' '), static_cast<type>(__x));
		return (__m_buffer.size() - n);
	}

	stream_type& __flush(stream_type& __stream) const
	{
		using namespace std;

		__stream << flush;
		if (__m_noflush) 
			return __stream;

		void* stream_ptr = static_cast<void*>(&__stream);
		if (stream_ptr == addressof(clog) || stream_ptr == addressof(wclog)) {
			__stream.put(char_type('\r'));
			return (__stream << flush);
		}

		if (__istty() && (stream_ptr == addressof(cout) || stream_ptr == addressof(wcout))) {
			__stream.put(char_type('\r'));
		}
		else  {
			__stream.seekp(-static_cast<ios::pos_type>(__m_buffer.size()), ios::end);
		}
		
		return (__stream << flush);
	}


	inline bool __empty() const {
		return (__m_range.first == __m_range.second);
	}

	inline bool __contains(value_type __x) const {
		return ((__x >= __m_range.first) && (__x <= __m_range.second));
	}

	static inline bool __istty() { return (isatty(fileno(stdout)) != 0); }

#if 0
	static inline size_t __ndigits(value_type x) {
		size_t n = 1;
		while (x /= value_type(10)) { ++n; }
		return n;
	}
#endif
private:
	value_type    __m_total;
	size_type     __m_prec;
	size_type     __m_length;
	size_type     __m_text_size;
	char_type     __m_filler; // ASCII codes [176, 177, 178, 219, 254] on Win32
	range_type    __m_range;
	string_type   __m_format;
	bool          __m_noflush;
	mutable string_type  __m_buffer;
	mutable bool         __m_repaint;
	mutable value_type   __m_value;
	mutable double       __m_progress;
	mutable timepoint_t  __m_timestamp;

private:
	static const int frames[4];
};



template<
	class _Value,
	class _Duration,
	class _Char,
	class _Traits,
	class _Alloc
>
const int basic_progress_viewer<_Value, _Duration, _Char, _Traits, _Alloc>::frames[4] = { '|', '/', '-', '\\' };


template<
	class _Value, 
	class _Duration = std::chrono::seconds
>
using progress_viewer = basic_progress_viewer < _Value, _Duration, char >;

template<
	class _Value,
	class _Duration = std::chrono::seconds
>
using wprogress_viewer = basic_progress_viewer < _Value, _Duration, wchar_t >;

_STDX_END

