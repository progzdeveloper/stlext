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
#include <cmath>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "../platform/common.h"

_STDX_BEGIN

template<class T, class _Impl>
struct basic_metric 
{
	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

	// calculate distance between x and y ranges, given as pairs of iterators
	template<class _It>
	inline auto operator()(const std::pair<_It, _It>& x,
						   const std::pair<_It, _It>& y) const
	{
        __stdx_assertx(std::distance(x.first, x.second) == std::distance(y.first, y.second),
                       std::invalid_argument,
                       "x and y must have same dimensions");
		return _Impl::distance(x.first, x.second, y.first);
	}

	// calculate norm of x range, given as pair of iterators
	template<class _It>
	inline auto operator()(const std::pair<_It, _It>& x) const {
		return _Impl::norm(x.first, x.second);
	}

	// calculate distance between x and y vectors
	template<class _Vector>
	inline auto operator()(const _Vector& x, const _Vector& y) const
	{
        __stdx_assertx(std::size(x) == std::size(y),
                       std::invalid_argument,
                       "x and y must have same dimensions");
		return _Impl::distance(std::begin(x), std::end(x), std::begin(y));
	}

	// calculate norm of x vector
	template<class _Vector>
	inline auto operator()(const _Vector& x) const {
		return _Impl::norm(std::begin(x), std::end(x));
	}
};


template<class T>
struct minkowski_metric : basic_metric< T, minkowski_metric<T> >
{
	template<class _It>
	static inline auto distance(_It xfirst, _It xlast, _It yfirst, const T& p)
	{
		T sum = 0;
		T min_d = std::numeric_limits<T>::infinity();
		T max_d = -std::numeric_limits<T>::infinity();
		for (; xfirst != xlast; ++xfirst, ++yfirst) {
			auto d = std::fabs(*xfirst - *yfirst);
			sum += std::pow(d, p);
			min_d = std::min(min_d, d);
			max_d = std::max(max_d, d);
		}
		return std::isnan(sum) ? sum
			: !std::isnormal(sum) && std::signbit(p) ? min_d
			: !std::isnormal(sum) && !std::signbit(p) ? max_d
			: std::pow(sum, 1.0 / p);
	}

	template<class _It>
	static inline auto norm(_It xfirst, _It xlast, const T& p)
	{
		T sum = 0;
		T min_d = std::numeric_limits<T>::infinity();
		T max_d = -std::numeric_limits<T>::infinity();
		for (; xfirst != xlast; ++xfirst) {
			auto d = std::fabs(*xfirst);
			sum += std::pow(d, p);
			min_d = std::min(min_d, d);
			max_d = std::max(max_d, d);
		}
		return std::isnan(sum) ? sum
			: !std::isnormal(sum) && std::signbit(p) ? min_d
			: !std::isnormal(sum) && !std::signbit(p) ? max_d
			: std::pow(sum, 1.0 / p);
	}
};

/// PARTIAL SPECIALIZATION 
/*!
 * We need a special version of basic_metric for 
 * minkowski_metric because it's a most general case
 * and it must be threated separetly.
 * Setup of the power for minkowsky metrics is done in ctor.
 */
template<class T>
struct basic_metric< T, minkowski_metric<T> >
{
	typedef minkowski_metric<T> _Impl;

	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

	T __m_deg; // degree of power

	basic_metric(const T& p) : __m_deg(p) {}

	// calculate distance between x and y ranges
	template<class _It>
	inline auto operator()(const std::pair<_It, _It>& x,
						   const std::pair<_It, _It>& y) const
	{
        __stdx_assertx(std::distance(x.first, x.second) == std::distance(y.first, y.second),
                       std::invalid_argument,
                       "x and y must have same dimensions");
		return _Impl::distance(x.first, x.second, y.first, __m_deg);
	}

	// calculate norm of x range
	template<class _It>
	inline auto operator()(const std::pair<_It, _It>& x) const {
		return _Impl::norm(x.first, x.second, __m_deg);
	}

	// calculate distance between x and y vectors
	template<class _Vector>
	inline auto operator()(const _Vector& x, const _Vector& y) const
	{
        __stdx_assertx(std::size(x) == std::size(y),
                       std::invalid_argument,
                       "x and y must have same dimensions");
		return _Impl::distance(std::begin(x), std::end(x), std::begin(y), __m_deg);
	}

	// calculate norm of x vector
	template<class _Vector>
	inline auto operator()(const _Vector& x) const {
		return _Impl::norm(std::begin(x), std::end(x), __m_deg);
	}
};


template<class T>
struct cityblock_metric : basic_metric< T, cityblock_metric<T> >
{
	template<class _It>
	static inline auto distance(_It xfirst, _It xlast, _It yfirst) {
		T sum = 0;
		for (; xfirst != xlast; ++xfirst, ++yfirst) {
			sum += std::fabs(*xfirst - *yfirst);
		}
		return sum;
	}
	template<class _It>
	static inline auto norm(_It xfirst, _It xlast) {
		T sum = 0;
		for (; xfirst != xlast; ++xfirst) {
			sum += std::fabs(*xfirst);
		}
		return std::sqrt(sum);
	}
};

template<class T>
struct euclidian_metric : basic_metric< T, euclidian_metric<T> >
{
	template<class _It>
	static inline auto distance(_It xfirst, _It xlast, _It yfirst) {
		T sum = 0;
		for (; xfirst != xlast; ++xfirst, ++yfirst) {
			auto d = *xfirst - *yfirst;
			sum += d * d;
		}
		return std::sqrt(sum);
	}

	template<class _It>
	static inline auto norm(_It xfirst, _It xlast) {
		T sum = 0;
		for (; xfirst != xlast; ++xfirst) {
			auto d = *xfirst;
			sum += d * d;
		}
		return std::sqrt(sum);
	}


};


template<class T>
struct euclidian2_metric : basic_metric< T, euclidian2_metric<T> >
{
	template<class _It>
	static inline auto distance(_It xfirst, _It xlast, _It yfirst) {
		T sum = 0;
		for (; xfirst != xlast; ++xfirst, ++yfirst) {
			auto d = *xfirst - *yfirst;
			sum += d * d;
		}
		return sum;
	}

	template<class _It>
	static inline auto norm(_It xfirst, _It xlast) {
		T sum = 0;
		for (; xfirst != xlast; ++xfirst) {
			auto d = *xfirst;
			sum += d * d;
		}
		return sum;
	}
};

template<class T>
struct chebyshev_metric : basic_metric< T, chebyshev_metric<T> >
{
	template<class _It>
	static inline auto distance(_It xfirst, _It xlast, _It yfirst)
	{
		T maximum = -std::numeric_limits<T>::infinity();
		for (; xfirst != xlast; ++xfirst, ++yfirst) {
			auto d = *xfirst - *yfirst;
			if (d > maximum) {
				maximum = d;
			}
		}
		return maximum;
	}

	template<class _It>
	static inline auto norm(_It xfirst, _It xlast) {
		return (xfirst == xlast ? T() : std::max_element(xfirst, xlast));
	}
};


/*
template<class _It, class T>
T minkowski_distance(_It dfirst, _It dlast, const T& p)
{
	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
	// [dfirst, dlast) contains d[i] = v[i] - w[i] for two vectors v and w
	T ex = 0.0;
	T min_d = std::numeric_limits<T>::infinity();
	T max_d = -std::numeric_limits<T>::infinity();
	for (int i = 0 ; i < ds.size() ; ++i) {
		fp_type d = std::fabs(ds[i]);
		ex += std::pow(d, p);
		min_d = std::min(min_d, d);
		max_d = std::max(max_d, d);
	}

	return std::isnan(ex) ? ex
		: !std::isnormal(ex) && std::signbit(p) ? min_d
		: !std::isnormal(ex) && !std::signbit(p) ? max_d
		: std::pow(ex, 1.0/p);
}
*/



/// VECTOR DISTANCES

template<class _It, class T>
inline auto minkowski_distance(_It xfirst, _It xlast, _It yfirst, const T& p)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return minkowski_metric<value_type>::distance(xfirst, xlast, yfirst, p);
}
template<class _Container, class T>
inline auto minkowski_distance(const _Container& x, const _Container& y, const T& p) {
	return minkowski_distance(std::begin(x), std::end(x), std::begin(y), p);
}


template<class _It>
inline auto cityblock_distance(_It xfirst, _It xlast, _It yfirst)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return cityblock_metric<value_type>::distance(xfirst, xlast, yfirst);
}
template<class _Container>
inline auto cityblock_distance(const _Container& x, const _Container& y) {
	return cityblock_distance(std::begin(x), std::end(x), std::begin(y));
}


template<class _It>
inline auto euclidian_distance(_It xfirst, _It xlast, _It yfirst)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return euclidian_metric<value_type>::distance(xfirst, xlast, yfirst);
}
template<class _Container>
inline auto euclidian_distance(const _Container& x, const _Container& y) {
	return euclidian_distance(std::begin(x), std::end(x), std::begin(y));
}


template<class _It>
inline auto euclidian2_distance(_It xfirst, _It xlast, _It yfirst)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return euclidian2_metric<value_type>::distance(xfirst, xlast, yfirst);
}
template<class _Container>
inline auto euclidian2_distance(const _Container& x, const _Container& y) {
	return euclidian2_distance(std::begin(x), std::end(x), std::begin(y));
}



template<class _It>
inline auto chebyshev_distance(_It xfirst, _It xlast, _It yfirst)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return chebyshev_metric<value_type>::distance(xfirst, xlast, yfirst);
}
template<class _Container>
inline auto chebyshev_distance(const _Container& x, const _Container& y) {
	return chebyshev_distance(std::begin(x), std::end(x), std::begin(y));
}




/// VECTOR NORMS

template<class _It, class T>
inline auto minkowski_norm(_It xfirst, _It xlast, const T& p)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return minkowski_metric<value_type>::norm(xfirst, xlast, p);
}
template<class _Container, class T>
inline auto minkowski_norm(const _Container& x, const T& p)
{
	return minkowski_norm(std::begin(x), std::end(x), p);
}


template<class _It>
inline auto cityblock_norm(_It xfirst, _It xlast)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return cityblock_metric<value_type>::norm(xfirst, xlast);
}
template<class _Container>
inline auto cityblock_norm(const _Container& x) {
	return cityblock_norm(std::begin(x), std::end(x));
}


template<class _It>
inline auto euclidian_norm(_It xfirst, _It xlast)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return euclidian_metric<value_type>::norm(xfirst, xlast);
}
template<class _Container>
inline auto euclidian_norm(const _Container& x) {
	return euclidian_norm(std::begin(x), std::end(x));
}


template<class _It>
inline auto euclidian2_norm(_It xfirst, _It xlast)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return euclidian2_metric<value_type>::norm(xfirst, xlast);
}
template<class _Container>
inline auto euclidian2_norm(const _Container& x, const _Container& y) {
	return euclidian2_norm(std::begin(x), std::end(x));
}


template<class _It>
inline auto chebyshev_norm(_It xfirst, _It xlast)
{
	typedef typename std::iterator_traits<_It>::value_type value_type;
	return chebyshev_metric<value_type>::norm(xfirst, xlast);
}
template<class _Container>
inline auto chebyshev_norm(const _Container& x) {
	return chebyshev_norm(std::begin(x), std::end(x));
}

_STDX_END

