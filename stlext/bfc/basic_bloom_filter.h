#pragma once
#include <cstdint>
#include <vector>
#include <iterator>
#include <random>

#include "../bitvector/bitvector.hpp"

_STDX_BEGIN




template<
	class _Key,
	class _Hasher = std::hash<_Key>,
	class _Alloc = std::allocator<_Key>
>
class basic_bloom_filter
{
public:
	typedef uint64_t word_type;
	static const size_t bpw = sizeof(word_type) * CHAR_BIT;

	typedef size_t size_type;

	typedef _Key   key_type;
	typedef _Key   value_type;

	typedef _Hasher hasher;

	typedef _Alloc allocator_type;

	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef value_type* pointer;
	typedef const value_type* const_pointer;

private:

	typedef std::allocator_traits<allocator_type> alloc_traits;

	// type of hasher allocator
	typedef typename alloc_traits::template rebind_alloc<_Hasher>::other hasher_allocator;

	// type of seed allocator
	typedef typename alloc_traits::template rebind_alloc<uint64_t>::other word_allocator;

	typedef std::vector<_Hasher, hasher_allocator> hasher_set;
	typedef std::vector<uint64_t, word_allocator> seed_set;
	typedef stdx::bitvector<uint64_t, word_allocator> bitvec;

public:
	basic_bloom_filter() {

	}

	basic_bloom_filter(size_t __capacity, size_t __nhashes)
	{
		rehash(__nhashes);
		resize(__capacity);
	}

	template<typename _Optimizer>
	basic_bloom_filter(size_t __capacity, double __fpp, _Optimizer& __opt)
	{
		__opt(__n, __fpp);
		rehash(__opt.hash_count());
		resize(__opt.capacity());
	}

	~basic_bloom_filter() {
	}

	inline basic_bloom_filter& operator&= (const basic_bloom_filter& __other) {
        __stdx_assert(__other.hash_count() == this->hash_count(), std::logic_error);
		__m_bits &= __other.__m_bits;
		return (*this);
	}

	inline basic_bloom_filter& operator|= (const basic_bloom_filter& __other) {
        __stdx_assert(__other.hash_count() == this->hash_count(), std::logic_error);
		__m_bits |= __other.__m_bits;
		return (*this);
	}

	inline basic_bloom_filter& operator^= (const basic_bloom_filter& __other) {
        __stdx_assert(__other.hash_count() == this->hash_count(), std::logic_error);
		__m_bits ^= __other.__m_bits;
		return (*this);
	}

	inline basic_bloom_filter& operator~() {
		__m_bits.flip();
		return (*this);
	}


	inline void insert(const_reference __key)
	{
		size_t hash_code = 0;
		size_t n = __m_bits.size();
		auto seeds = this->__m_seeds.begin();
		for (auto it = this->__m_hashes.begin(), ; it != this->__m_hashes.end(); ++it, ++seeds) {
			hash_code = __key_hash(*it, __key, *seeds) % n;
			__m_bits.set(hash_code, true);
		}
	}

	inline bool contains(const_reference __key) const
	{
		size_t hash_code = 0;
		size_t n = __m_bits.size();
		auto seeds = this->__m_seeds.begin();
		for (auto it = this->__m_hashes.begin(); it != this->__m_hashes.end(); ++it, ++seeds) {
			hash_code = __key_hash(*it, __key, *seeds) % n;
			if (!__m_bits.at(hash_code))
				return false;
		}
		return true;
	}


	inline void clear() {
		__m_bits.clear();
	}

	inline bool empty() const  {
		return __m_bits.empty();
	}

	inline size_t capacity() const {
		return __m_bits.size();
	}

	inline size_t count() const {
		return __m_bits.count();
	}


protected:

	inline void resize(size_t __capacity) {
		__m_bits.resize(__capacity, false);
	}

	inline void rehash(size_t __n)
	{
		__m_hashes.resize(__n);
		__m_seeds.resize(__n);
		__reseed();
	}

private:
	inline size_t __key_hash(_Hasher& __hasher, const _Key& __key, size_t __seed) {
		return __hasher(__key) ^ __seed;
	}


	inline void __reseed() 
	{
		std::mt19937 rnddev;
		// TODO: implement random_device seeding
		std::uniform_int_distribution<size_t> distrib;
		std::generate(this->__m_seeds.begin(), this->__m_seeds.end(), [&]{
			return distrib(rnddev));
		});
	}

private:
	bitvec     __m_bits;
	hasher_set __m_hashes;
	seed_set   __m_seeds;
};


_STDX_END



