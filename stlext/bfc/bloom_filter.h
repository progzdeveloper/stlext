#pragma once
#include <cstdint>
#include <cmath>
#include <climits>


#include "../platform/common.h"


_STDX_BEGIN

template<class _Impl>
class bloom_filter_optimizer_base
{
public:
	inline void operator()(double __fpp, size_t __projected) const {
		static_cast<const _Impl*>(this)->optimize(__fpp, __projected);
	}

	size_t hash_count() const { return __m_hashes; }
	size_t capacity() const { return __m_size; }

protected:
	size_t __m_hashes, __m_size;
};




template<size_t _BitsPerWord = 64>
class bloom_filter_optimizer : 
	public bloom_filter_optimizer_base< 
		bloom_filter_optimizer<_BitsPerWord> 
	>
{
	typedef bloom_filter_optimizer_base <
		bloom_filter_optimizer<_BitsPerWord>
	> base_type;

	friend class base_type;

public:
	static const size_t bpw = _BitsPerWord;

	bloom_filter_optimizer(size_t min_size = 0, size_t max_size = UINT_MAX,
						   size_t min_nhashes = 1, size_t max_nhashes = USHRT_MAX) :
		__m_min_size(min_size), __m_max_size(max_size),
		__m_min_hashes(min_nhashes), __m_max_hashes(max_nhashes),
		__m_hashes(min_nhashes), __m_size(min_size)
	{
	}

private:
	// __fpp - false positive probability
	// __projected - projected_element_count
	void optimize(double __fpp, size_t __projected) const
	{
		double min_m = std::numeric_limits<double>::infinity();
		double min_k = 0.0;
		double curr_m = 0.0;
		double k = 1.0;

		while (k < static_cast<double>(__projected))
		{
			double numerator = (-k * __projected);
			double denominator = std::log(1.0 - std::pow(false_positive_probability, 1.0 / k));
			curr_m = numerator / denominator;
			if (curr_m < min_m)
			{
				min_m = curr_m;
				min_k = k;
			}
			k += 1.0;
		}

		__m_hashes = static_cast<size_t>(min_k);
		__m_size = static_cast<size_t>(min_m);
		__m_size += (((__m_size % bpw) != 0) ? (bpw - (__m_size % bpw)) : 0);

		if (__m_hashes < __m_min_hashes)
			__m_hashes = __m_min_hashes;
		else if (__m_hashes > __m_max_hashes)
			__m_hashes = __m_max_hashes;

		if (__m_size < __m_min_size)
			__m_size = __m_min_size;
		else if (__m_size > __m_max_size)
			__m_size = __m_max_size;
	}

private:
	size_t __m_min_size, __m_max_size;
	size_t __m_min_hashes, __m_max_hashes;
};





template<class _Traits>
class bloom_filter : public _Traits
{
	typedef _Traits base_type;
	
public:
	bloom_filter() : base_type() {
	}

	bloom_filter(size_t __capacity, size_t __nhashes) :
		base_type(__capacity, __nhashes)
	{
		//rehash(__nhashes);
		//resize(__capacity);
	}

	template<typename _Optimizer>
	bloom_filter(size_t __capacity, double __fpp, _Optimizer& __opt) :
		base_type(__capacity, __nhashes)
	{
		//__opt(__n, __fpp);
		//rehash(__opt.hash_count());
		//resize(__opt.capacity());
	}

	~bloom_filter() {
	}




	template<typename _It>
	inline void insert(_It __first, _It __last) { 
		for (; __first != __last; ++__first)
			insert(*__first);
	}

	inline bool operator[](const_reference __key) const {
		return contains(__x);
	}

	template<typename _It>
	inline bool any_of(_It __first, _It __last) const
	{
		for (; __first != __last; ++__first) {
			if (contains(*__first))
				return true;
		}
		return false;
	}

	template<typename _It>
	inline _It all_of(_It __first, _It __last) const
	{
		for (; __first != __last; ++__first) {
			if (!contains(*__first))
				return __first;
		}
		return __last;
	}

	template<typename _It>
	inline _It none_of(_It __first, _It __last) const
	{
		for (; __first != __last; ++__first) {
			if (contains(*__first))
				return false;
		}
		return true;
	}

	inline double effective_fpp() const
	{
		const double e = 2.718281828459045235360287471352662497757247093699959574966;
		const double n = static_cast<double>(capacity());
		const double k = static_cast<double>(hash_count());
		const double m = static_cast<double>(size());
		return std::pow(1.0 - std::pow(e, -k * m / n), k);
	}

};


_STDX_END
