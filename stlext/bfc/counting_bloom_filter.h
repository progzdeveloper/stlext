#pragma once
#include "basic_bloom_filter.h"

_STDX_BEGIN

template<
	class _Key,
	class _Hasher = std::hash<_Key>,
	class _Alloc = std::allocator<_Key>
>
class counting_bloom_filter :
	public basic_bloom_filter<
	_Key, _Hasher, _Alloc,
	counting_bloom_filter<_Key, _Hasher, _Alloc>
	>
{
	typedef basic_bloom_filter <
		_Key, _Hasher, _Alloc,
		counting_bloom_filter<_Key, _Hasher, _Alloc>
	> base_type;

	friend class base_type;

public:


	counting_bloom_filter() : base_type() {
	}

	counting_bloom_filter(size_t __capacity, size_t __nhashes) :
		base_type(__capacity, __nhashes) {
	}

	template<typename _Optimizer>
	counting_bloom_filter(size_t __capacity, double __fpp, _Optimizer& __opt) :
		base_type(__capacity, __fpp, __opt) {
	}

	~counting_bloom_filter() {
	}

#if 0 // that's questionable ???
	inline bloom_filter& operator&= (const bloom_filter& __other) {
		__m_bits &= __other.__m_bits;
		return (*this);
	}

	inline bloom_filter& operator|= (const bloom_filter& __other) {
		__m_bits |= __other.__m_bits;
		return (*this);
	}

	inline bloom_filter& operator^= (const bloom_filter& __other) {
		__m_bits ^= __other.__m_bits;
		return (*this);
	}

	inline bloom_filter& operator~() {
		__m_bits.flip();
		return (*this);
	}
#endif

private:
	inline void __insert(const_reference __key)
	{
		size_t hash_code = 0;
		auto seeds = this->__m_seeds.begin();
		for (auto it = this->__m_hashes.begin(), ; it != this->__m_hashes.end(); ++it, ++seeds) {
			hash_code = base_type::hash_combine((*it)(__key), *seeds);
			__m_bits[hash_code]++;
		}
	}

	inline bool __contains(const_reference __key) const
	{
		size_t hash_code = 0;
		auto seeds = this->__m_seeds.begin();
		for (auto it = this->__m_hashes.begin(), ; it != this->__m_hashes.end(); ++it, ++seeds) {
			hash_code = base_type::hash_combine((*it)(__key), *seeds);
			if (__m_bits[hash_code] == 0)
				return false;
		}
		return true;
	}

	inline void __resize(size_t __capacity) {
		__m_bits.resize(__capacity, 0);
	}

	inline void __clear() {
		__m_bits.clear();
	}

	inline bool __empty() const  {
		return __m_bits.empty();
	}

	inline size_t __capacity() const {
		return __m_bits.size();
	}

private:
	// type of hasher allocator
	typedef typename base_type::alloc_traits::template rebind_alloc<uint64_t>::other word_allocator;
	std::vector<size_t> __m_bits;
};


_STDX_END



