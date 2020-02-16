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
#include <atomic>
#include <thread>
#include <chrono>
#include <numeric>
#include <array>

#include "progress_viewer.hpp"


_STDX_BEGIN

template<
	class _Counter,
	class _Duration = std::chrono::seconds,
	class _Char = char,
	size_t _MaxThreads = 64
>
class basic_progress_watcher : 
	public basic_progress_viewer<_Counter, _Duration, _Char>
{
	typedef basic_progress_viewer<_Counter, _Duration, _Char> base_type;

	basic_progress_watcher();
	basic_progress_watcher(const basic_progress_watcher&);
	basic_progress_watcher& operator= (const basic_progress_watcher&);

public:
	typedef _Duration  duration_type;

	typedef typename base_type::value_type value_type;
	typedef typename base_type::reference reference;
	typedef typename base_type::reference const_reference;

	typedef typename base_type::string_type string_type;
	typedef typename base_type::stream_type stream_type;

	static constexpr const _Char* default_fmt =
		detail::format_traits<_Char>::format();

	basic_progress_watcher(duration_type __interval,
						   _Counter __lower, _Counter __upper,
						   const string_type& __format = default_fmt,
						   const char __fill = '*',
						   size_t __length = 32) : 
		base_type(0, __upper - __lower, __format, __fill, __length),
		__m_interval(__interval),
		__m_nactive(0)
	{
		__s_main_threadid = std::this_thread::get_id();
		__m_counters.fill(0);
	}

	inline basic_progress_watcher& operator++() {
		attach();
		return (*this);
	}

	inline basic_progress_watcher& operator--() {
		detach();
		return (*this);
	}

	inline void attach() { ++__m_nactive; }
	inline void detach() { --__m_nactive; }

	inline size_t count() const {
		return __m_nactive;
	}

	inline reference operator[](size_t idx) {
		return __m_counters[idx];
	}

	inline const_reference operator[](size_t idx) const {
		return __m_counters[idx];
	}

	stream_type& operator()(stream_type& __stream) 
	{
		using namespace std;
		if (std::this_thread::get_id() != __s_main_threadid) {
			return __stream; // we can report progress from main thread only
		}
		// sleep for some time
		this_thread::sleep_for(__m_interval); 
		while (__m_nactive > 0) {
			// collect statistics and show it
			__stream << (viewer()(collect()));
			// sleep for some time
			this_thread::sleep_for(__m_interval); 
		}
		return (__stream << (viewer()(collect())) << endl);
	}

private:
	inline value_type collect() const { // collect statistics
		//typedef stdx::identity_element<counter_type, std::plus<counter_type> > identity_element;
		//return std::accumulate(__m_states.begin(), __m_states.end(), identity_element::get());
		return std::accumulate(__m_counters.begin(), __m_counters.end(), value_type());
	}

private:
	inline base_type& viewer() { return static_cast<base_type&>(*this); }

private:
	duration_type       __m_interval;
	std::atomic<size_t> __m_nactive;
	std::array<value_type, _MaxThreads> __m_counters;
	static std::thread::id __s_main_threadid;
};

template<
	class _Counter,
	class _Duration,
	class _Char,
	size_t _MaxThreads
>
std::thread::id basic_progress_watcher<_Counter, _Duration, _Char, _MaxThreads>::__s_main_threadid;

template<
	class _Counter = uint64_t, 
	class _Duration = std::chrono::seconds
>
using progress_watcher = basic_progress_watcher < _Counter, _Duration, char > ;

template<
	class _Counter = uint64_t,
	class _Duration = std::chrono::seconds
>
using wprogress_watcher = basic_progress_watcher < _Counter, _Duration, wchar_t >;

_STDX_END

