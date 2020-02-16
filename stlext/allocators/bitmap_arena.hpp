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
#include <climits>
#include <cstdint>
#include "basic_arena.hpp"

#include "../platform/bits.h"

_STDX_BEGIN


template<
	typename _Arena,
	size_t _BlockSize = sizeof(void*),
	size_t _MaxBlocks = (1 << 16),
	typename _Mutex = void,
	typename _Destroyer = empty_delete<>
>
struct bitmap_arena :
	public basic_arena<_Mutex, _Destroyer, false>
{
	static_assert(_MaxBlocks <= _MAX_STACK_SIZE, "incorrect maximum number of bitmap blocks");
	static_assert(_BlockSize >= sizeof(void*), "size of block is too small");
	static_assert(!(_MaxBlocks % _BlockSize), "size of block is not multiplyable with cache size");

	__disable_copy(bitmap_arena)

	typedef arena_traits<_Arena> traits_type;
public:
	typedef uint64_t word_type;

	// size of memory block in bytes
	static const size_t BLOCKSIZE = _BlockSize;

	// total number of blocks
	static const size_t BLOCKCOUNT = _MaxBlocks;

	// number of bits per one word
	static const size_t BITSPERWORD = sizeof(word_type) * CHAR_BIT;

	// size of memory bitmap in words
	static const size_t BITMAPSIZE = (BLOCKCOUNT / BITSPERWORD);

	// number of extra bits
	//static const unsigned EXTRABITS = static_cast<unsigned>(BLOCKCOUNT % BITSPERWORD);


	bitmap_arena() : 
		__m_memblk(0) {
		__construct(traits_type::global());
	}

	bitmap_arena(const _Arena& __arena) : 
		__m_memblk(0) {
		__construct(__arena);
	}

	~bitmap_arena() {
		__dispose();
	}

	inline void* allocate(size_t __nbytes) {
		return (__nbytes != 0 ? __allocate(__block_count(__nbytes)) : nullptr);
	}

	inline void* reallocate(size_t __nbytes, void*) {
		return allocate(__nbytes);
	}

	inline void* deallocate(void* __addr, size_t __nbytes) {
		return __deallocate(__addr, __block_count(__nbytes));
	}

	template<typename T>
	inline size_t max_size() const
	{	// estimate maximum array size
		return (BLOCKCOUNT / __block_count(sizeof(T)));
	}

#ifdef _DEBUG
	inline std::ostream& dump_state(std::ostream& __stream) {
		return __dump(__stream);
	}
#endif

private:
	enum block_state { USED = 0, FREE = 1 };


	/*!
	allocate continious nblocks of memory
	*/
	inline void* __allocate(size_t __nblocks)
	{
		if (__nblocks == 0 || __nblocks > __m_nfree_blocks)
			return nullptr; // failed to allocate - no free space

		// O(n) slow search
		size_t offset = __locate(__m_bitmap, __m_bitmap + BLOCKCOUNT * BLOCKSIZE, __nblocks);
		if (offset < BLOCKCOUNT) {
			__mark_segments(offset, USED, __nblocks);
			return __address(offset);
		}

		// failed to allocate - no free space
		return nullptr;
	}

	/*!
	deallocate continious nblocks of memory at address addr
	*/
	inline void* __deallocate(void* __addr, size_t __nblocks)
	{
		if (__contains(__addr) && __nblocks != 0) {
			__mark_segments(__offset(__addr), FREE, __nblocks);
			return nullptr;
		}
		return __addr; // address is not valid
	}


	/*!
	helper function for __mark_segments
	fill range [msb, lsb) in word element with flag __flag
	*/
	inline void __fill_range(word_type& element, size_t msb, size_t lsb, bool __flag)
	{
		word_type mask = (word_type(-1) << lsb) & (word_type(-1) >> (BITSPERWORD - msb - 1));
		if (__flag) {
			element |= mask;
		}
		else {
			element &= ~mask;
		}
	}

	/*!
	mark range of bits starting at position __pos with size __n to flag __flag
	*/
	void __mark_segments(size_t __pos, bool __flag, size_t __n)
	{
		// update number of free blocks
		__m_nfree_blocks += __flag ? __n : -(static_cast<int64_t>(__n));


		size_t blk_pos = __block_index(__pos); // get word index
		size_t bit_pos = __bit_index(__pos);   // get bit index

		size_t nbits = (__n <= BITSPERWORD - bit_pos) ? __n : (BITSPERWORD - bit_pos);
		__fill_range(*(__m_bitmap + blk_pos), bit_pos + nbits - 1, bit_pos, __flag);

		__n -= nbits;
		if (!__n)
			return;

		blk_pos += 1;
		for (; __n >= 0; blk_pos++)
		{
			if (__n <= BITSPERWORD) {
				__fill_range(*(__m_bitmap + blk_pos), __n - 1, 0, __flag);
				return;
			}
			else {
				__m_bitmap[blk_pos] = __flag ? word_type(-1) : 0;
			}
			__n -= BITSPERWORD;
		}
	}


	/*!
	evaluate next shift value
	*/
	static inline size_t __next_shift(word_type __mask, word_type __blocks)
	{
		static const word_type __ctz_mask = (word_type(1) << (BITSPERWORD >> 1));
		using stdx::__ctz;
		using stdx::__clz;
		size_t r = 0;
		word_type attempt = __mask & __blocks;
		if (!attempt & ~(__mask >> 1)) {
			return __ctz(__blocks & ~__mask) & __ctz_mask;
		}
		else {
			return (BITSPERWORD - __clz(__mask & ~attempt));
		}
	}

	/*!
	locates a first free space in single word
	*/
	static inline word_type __ffs_word(word_type __word, size_t __nblocks)
	{
		size_t offset = 0;
		size_t shift_amount = 0;
		word_type mask = (1ULL << __nblocks) - 1;
		while ((mask & __word) != mask)
		{
			shift_amount = __next_shift(mask, __word);
			__word >>= shift_amount;
			if (!__word)
				return word_type(-1);

			offset += shift_amount;
		}

		return offset;
	}

	/*!
	\inner 
	locates a first free space in free block bitmap
	using slow search in each word (O(n) complexity)
	\param __pfbb pointer to free bitmap begin
	\param __pfbe pointer to free bitmap end
	\param __nblocks number of free blocks to find
	*/
	static size_t __locate(const word_type* __pfbb, const word_type* __pfbe, size_t __nblocks)
	{
		using stdx::__ctz;
		using stdx::__clz;

		const word_type* bitmap = __pfbb;
		if (__nblocks < BITSPERWORD)
		{
			for (; __pfbb < __pfbe; ++__pfbb)
			{
				word_type w = *__pfbb;

				if (w == 0)
					continue;

				word_type idx = __ffs_word(w, __nblocks);
				if (idx != word_type(-1)) {
					return static_cast<size_t>((__pfbb - bitmap) * BITSPERWORD + idx);
				}

				if (__pfbb == (__pfbe - 1))
					break;

				word_type next = *(__pfbb + 1);
				//size_t pbits = ((BITSPERWORD - 1) - fls(~w));
				//size_t nbits = (next == UINT64_MAX ? BITSPERWORD : ffs(~next));
				size_t pbits = __clz(~w);
				size_t nbits = __ctz(~next);
				if ((pbits + nbits) >= __nblocks) { // word boundary
					return  static_cast<size_t>((__pfbb - bitmap) * BITSPERWORD + (BITSPERWORD - pbits));
				}
			}
		}
		else {
			size_t bcnt = 0;
			size_t offset = 0;
			word_type w;
			for (; __pfbb < __pfbe; ++__pfbb)
			{
				w = *__pfbb;
				if (w == 0)
					continue;

				//bcnt = (w == UINT64_MAX ? BITSPERWORD : ((BITSPERWORD - 1) - fls(~w)));
				bcnt = (w == word_type(-1) ? BITSPERWORD : __clz(~w));
				offset = static_cast<size_t>((__pfbb - bitmap) * BITSPERWORD + (BITSPERWORD - bcnt));

				for (__pfbb = __pfbb + 1; __pfbb < __pfbe; ++__pfbb)
				{
					word_type w = *__pfbb;
					//size_t n = (w == UINT64_MAX ? BITSPERWORD : ffs(~w));
					size_t n = __ctz(~w);
					bcnt += n;

					if (n < BITSPERWORD && bcnt < __nblocks)
						break;

					if (bcnt >= __nblocks) {
						return offset;
					}
				}
			}

		}
		return BLOCKCOUNT;
	}

#ifdef _DEBUG
	/* dump internal state of bitmap (for debugging only)*/
	std::ostream& __dump(std::ostream& __stream) const
	{
		using namespace std;
		for (ptrdiff_t i = BITMAPSIZE - 1; i >= 0; i--) {
			for (int8_t pos = BITSPERWORD - 1; pos >= 0; --pos) {
				__stream << ((__m_bitmap[i] & ((word_type)1 << pos)) ? '1' : '0');
			}
		}
		return (__stream << endl);
	}
#endif

private:
	inline void __construct(const _Arena& __arena) 
	{
		__m_nfree_blocks = BLOCKCOUNT;
		__init_from(__arena, __m_arena);
		__m_memblk = static_cast<char*>(traits_type::allocate(__m_arena, BLOCKCOUNT * BLOCKSIZE));
		memset(__m_bitmap, 0xFF, sizeof(word_type) * BITMAPSIZE);
	}

	inline void __dispose() {
		traits_type::deallocate(__m_arena, __m_memblk, BLOCKCOUNT * BLOCKSIZE);
	}

	// check if __addr pointer contains in pre-allocated storage
	inline bool __contains(const void* __addr) const {
		return (static_cast<const char*>(__addr) >= __m_memblk && static_cast<const char*>(__addr) < (__m_memblk + BLOCKCOUNT * BLOCKSIZE));
	}
	
	// convert bit index offset to memory address
	inline void* __address(size_t __offset)  {
		return (__m_memblk + (__offset * BLOCKSIZE));
	}

	// convert memory address to bit index offset
	inline size_t __offset(void* __addr) const {
		return static_cast<size_t>((static_cast<char*>(__addr) - __m_memblk) / BLOCKSIZE);
	}

	/*!
	\internal bit index
	\param __pos absolute bit position
	\return bit index
	*/
	static inline unsigned __bit_index(size_t __pos) {
		return static_cast<unsigned>(__pos % BITSPERWORD);
	}


	/*!
	\internal bit mask
	\param __pos absolute bit position
	\return  bit mask
	*/
	static inline word_type __bit_mask(size_t __pos) {
		return (word_type(1) << __bit_index(__pos));
	}

	/*!
	\internal word index
	\param __pos absolute bit position
	\return word index
	*/
	static inline size_t __block_index(size_t __pos) {
		return (__pos / BITSPERWORD);
	}

	/*!
	\internal number of blocks to hold specified number of bytes
	\param __nbytes number of bytes 
	\return number of occupied blocks
	*/
	static inline size_t __block_count(size_t __nbytes) {
		return (__nbytes / BLOCKSIZE + static_cast<size_t>(__nbytes % BLOCKSIZE != 0));
	}

	/*!
	\internal lowest bit
	\param x word
	\return lowest bit in word x
	*/
	static inline size_t __lowest_bit(word_type __x) {
		__x = (__x - (__x & (__x - 1)));
		return (__x != 0 ? static_cast<size_t>(stdx::__ctz(__x)) : 0);
	}

#if 0
	// ### FIX ME: use generalized implementation from platform.h header
	static inline unsigned long ffs(unsigned long x) {
		unsigned long idx = 0;
		_BitScanForward(&idx, x);
		return idx;
	}

	// ### FIX ME: use generalized implementation from platform.h header
	static inline unsigned long fls(unsigned long x) {
		unsigned long idx = 0;
		_BitScanReverse(&idx, x);
		return idx;
	}

#if defined(_M_X64) || defined(_LP64) || defined(__x86_64) || defined(_WIN64)
	// ### FIX ME: use generalized implementation from platform.h header
	static inline unsigned long ffs(uint64_t x) {
		unsigned long idx = 0;
		_BitScanForward64(&idx, x);
		return idx;
	}
	// ### FIX ME: use generalized implementation from platform.h header
	static inline unsigned long fls(uint64_t x) {
		unsigned long idx = 0;
		_BitScanReverse64(&idx, x);
		return idx;
	}
#endif
#endif

private:
	typename traits_type::member_type __m_arena;
	word_type __m_bitmap[BITMAPSIZE];
	char* __m_memblk;
	size_t __m_nfree_blocks;
};

_STDX_END
