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

#include "../../platform/common.h"

#include <type_traits>
#include <iterator>
#include <random> // for distributions


_STDX_BEGIN

namespace detail {

	/// Reservoir sampling algorithm
	template<typename _InIt, typename _RandIt, typename _Size, typename _UniRand>
	_RandIt sample_impl(_InIt first, _InIt last, std::input_iterator_tag, _RandIt out, std::random_access_iterator_tag,
						_Size n, _UniRand&& r)
	{
		typedef typename std::uniform_int_distribution<_Size> distrib_type;
		typedef typename distrib_type::param_type param_type;

		distrib_type distr{};
		_Size sample_sz = 0;

		for (; first != last && sample_sz != n; ++first) {
			out[sample_sz++] = *first;
		}

		for (_Size pop_sz = sample_sz; first != last; ++first, ++pop_sz) {
			const auto k = distr(r, param_type{ 0, pop_sz });
			if (k < n) out[k] = *first;
		}
		return (out + sample_sz);
	}

	/// Selection sampling agorithm
	template<typename _FwdIt, typename _OutIt, typename _Cat, typename _Size, typename _UniRand>
	_OutIt sample_impl(_FwdIt first, _FwdIt last, std::forward_iterator_tag, _OutIt out, _Cat,
					   _Size n, _UniRand&& r)
	{
		typedef typename std::uniform_int_distribution<_Size> distrib_type;
		typedef typename distrib_type::param_type param_type;

		distrib_type distr{};
		_Size unsampled_sz = std::distance(first, last);
		for (n = (std::min)(n, unsampled_sz); n != 0; ++first) {
			if (distr(r, param_type{ 0, --unsampled_sz }) < n) {
				*out++ = *first;
				--n;
			}
		}
		return out;
	}
}


/// C++17 proposal
/*!
 * \fn sample(_PopulationIt first, _PopulationIt last, _SampleIt out, _Distance n, _UniRand&& r)
 * \brief Take a random sample from a population 
 * 
 * \tparam _PopulationIt models input or forward iterator
 * \tparam _SampleIt     models output iterator
 * \tparam _Distance     models integer distance type
 * \tparam _UniRand      models unifirm random generator
 * 
 * \param first The start of the input sequence
 * \param last  One past the end of the input sequence
 * \param out   An output iterator to write the elements
 * \param n     Number of samples
 * \param r     Universal reference to random generator
 * \return The updated output iterator
 */
template<typename _PopulationIt, typename _SampleIt, typename _Distance, typename _UniRand>
inline _SampleIt sample(_PopulationIt first, _PopulationIt last, _SampleIt out, _Distance n, _UniRand&& r)
{
	using namespace std;
	typedef typename iterator_traits<_PopulationIt>::iterator_category __pop_cat;
	typedef typename iterator_traits<_PopulationIt>::iterator_category __samp_cat;

	static_assert(is_integral<_Distance>::value, "sample size must be an integer type");
	static_assert((is_convertible<__pop_cat, forward_iterator_tag>::value ||
				   is_convertible<__samp_cat, random_access_iterator_tag>::value),
				  "output range must use RandomAccessIterator when input range does not meet the ForwardIterator requipments");

	return detail::sample_impl(first, last, __pop_cat{}, out, __samp_cat{}, n, forward<_UniRand>(r));
}



_STDX_END





