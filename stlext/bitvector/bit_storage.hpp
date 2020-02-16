// Copyright (c) 2016-2018, Michael Polukarov (Russia).
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
#include <memory>
#include <memory.h>
#include <algorithm>
#include <vector>
#include <utility>

#include "bit_iterator.hpp"

_STDX_BEGIN

struct buf_opt {};
struct tag_opt {};
typedef void non_opt;

namespace detail {


template<
        class _Alloc,
        bool _IsEmpty = std::is_empty<_Alloc>::value
        >
class alloc_ebo
{
public:
    alloc_ebo() {}
    alloc_ebo(const _Alloc& a) : al(a) {}

    typename _Alloc::pointer allocate(typename _Alloc::size_type n,
                                      const void*) {
        return al.allocate(n);
    }
    typename _Alloc::pointer allocate(typename _Alloc::size_type n) {
        return al.allocate(n);
    }
    void deallocate(typename _Alloc::pointer p,
                    typename _Alloc::size_type n) {
        al.deallocate(p, n);
    }

    const _Alloc& get_alloc() const { return al; }

    operator _Alloc() const { return al; }

private:
    _Alloc al;
};


template<class _Alloc>
class alloc_ebo<_Alloc, true> :
        public _Alloc
{
public:
    const _Alloc& get_alloc() const { return (*this); }

    // that unclear if we need an explicit cast operator
    // for casting derived to base class (bacause we already a
    // derive from it and that kind of conversion is not need
    // according to C++ standart), but on some compilers this may
    // won't work (i.e. because of template instantiation issues)
    // operator _Alloc() const { return al; }
};




template<
        class _Word,
        class _Alloc = std::allocator<_Word>,
        class _Opt = non_opt
        >
class bit_storage 
{
    static_assert(std::is_integral<_Word>::value &&
                  std::is_unsigned<_Word>::value,
                  "_Word type must be unsigned integer type");
public:
    typedef bit_traits<_Word> traits_type;
    typedef bit_iterator<_Word, true> const_iterator;
    typedef bit_iterator<_Word, false> iterator;

    static constexpr size_t max_size = size_t(-1);
    static constexpr size_t bpw = traits_type::bpw;

    bit_storage() :
        __m_nbits(0) {
    }

    bit_storage(const _Alloc& al) :
        __m_blocks(al),
        __m_nbits(0) {
    }

    bit_storage(size_t n, bool v) :
        __m_blocks(traits_type::bit_space(n)),
        __m_nbits(n)
    {
        fill_n(begin(), __m_nbits, v);
    }

    ~bit_storage() {
    }

    template<class _It>
    void assign(size_t n, _It first, _It last)
    {
        __m_blocks.assign(first, last);
        __m_nbits = n;
        trim();
    }

    void resize(size_t n, bool value)
    {
        if (__m_nbits == n)
            return;

        const size_t current_blocks = nblocks();
        const size_t required_blocks = traits_type::bit_space(n);

        const _Word v = (value ? ~_Word(0) : _Word(0));
        __m_blocks.resize(required_blocks, v);

        // At this point:
        //
        //  - if the buffer was shrunk, we have nothing more to do,
        //    except a call to zero extra bits.
        //
        //  - if it was enlarged, all the (used) bits in the new __m_blocks have
        //    the correct value, but we have not yet touched those bits, if
        //    any, that were 'unused bits' before enlarging: if v == true,
        //    they must be set.
        if (value && (n > __m_nbits)) {
            const size_t ebits = traits_type::bit_index(__m_nbits);
            if (ebits) {
                // Set them.
                __m_blocks[current_blocks - 1] |= (v << ebits);
            }
        }
        __m_nbits = n;
        trim();
    }

    void reserve(size_t nbits) {
        __m_blocks.resize(traits_type::bit_space(nbits));
    }

    void shrink() {
        __m_blocks.shrink_to_fit();
    }

    void clear() {
        __m_blocks.clear();
        __m_nbits = 0;
    }

    /*!
    \internal trim(zeroize) extra bits
    */
    inline void trim() {
        if (__m_nbits % bpw != 0)
            __m_blocks.back() &= ((_Word)1 << (__m_nbits) % bpw) - 1;
    }


    void lshift(size_t pos)
    {	// shift left by _Pos
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = __m_blocks.size() - 1;
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = _nwords; 0 <= _wpos; --_wpos) // shift by words
                __m_blocks[_wpos] = _wordshift <= _wpos ? __m_blocks[_wpos - _wordshift] : (_Word)0;

        if ((pos %= bpw) != 0)
        {	// 0 < _Pos < _Bitsperword, shift by bits
            for (std::ptrdiff_t _wpos = _nwords; 0 < _wpos; --_wpos)
                __m_blocks[_wpos] = (_Word)((__m_blocks[_wpos] << pos) | (__m_blocks[_wpos - 1] >> (bpw - pos)));
            __m_blocks[0] <<= pos;
        }
        trim();
    }

    void rshift(size_t pos)
    {	// shift right by _Pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = __m_blocks.size() - 1;
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = 0; _wpos <= (std::ptrdiff_t)_nwords; ++_wpos)
                __m_blocks[_wpos] = (_wordshift <= (std::ptrdiff_t)_nwords - _wpos ? __m_blocks[_wpos + _wordshift] : (_Word)0);

        if ((pos %= bpw) != 0)
        {	// 0 < _Pos < _Bitsperword, shift by bits
            for (std::ptrdiff_t _wpos = 0; _wpos < (std::ptrdiff_t)_nwords; ++_wpos)
                __m_blocks[_wpos] = (_Word)((__m_blocks[_wpos] >> pos) | (__m_blocks[_wpos + 1] << (bpw - pos)));
            __m_blocks[_nwords] >>= pos;
        }
        trim();
    }


    iterator begin() { return iterator(data(), 0); }
    const_iterator begin() const { return const_iterator(data(), 0); }

    iterator end() { return iterator(data() + traits_type::block_index(nbits()), traits_type::bit_index(nbits())); }
    const_iterator end() const { return const_iterator(data() + traits_type::block_index(nbits()), traits_type::bit_index(nbits())); }

    _Word* data() { return __m_blocks.data(); }

    const _Word* data() const { return __m_blocks.data(); }

    size_t nblocks() const { return __m_blocks.size(); }

    size_t nbits() const { return __m_nbits; }

    size_t capacity() const { return (__m_blocks.capacity() * traits_type::bpw); }

private:
    std::vector<_Word, _Alloc> __m_blocks;
    size_t __m_nbits;
};



#ifdef STDX_PROCESSOR_X86_64
template<
        class _Word,
        class _Alloc
        >
class bit_storage<_Word, _Alloc, buf_opt> :
        alloc_ebo<_Alloc>
{
    typedef alloc_ebo<_Alloc> base_type;


    static_assert(CHAR_BIT == 8, "incompatible number of bits in char");
    static_assert(sizeof(void*) == sizeof(size_t), "incompatible pointer model");

    //
    // Memory layout (example for x64 target)
    //
    // s - __m_bits representing current size of heap allocated word buffer (number of words)
    // c - __m_bits representing current capacity of bit vector (number of __m_bits)
    // p - __m_bits of heap allocated words pointer
    // b - __m_bits of local buffer
    // k - __m_bits representing size of local buffer (number of __m_bits)
    // 0 - zero-bit
    //
    // Dymanic buffer allocation
    // ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
    // cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
    // 000ppppppppppppppppppppppppppppppppppppppppppppp0000000000000000
    // ^ ^                                             ^              ^
    // 3 or 2 unused least significant                 16 unused most significant
    // alignment bits of pointer                       bits of pointer
    // (depending on x86-x64 target)
    //
    // Static buffer allocation
    // bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
    // bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
    // bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbkkkkkkkk
    //                                                         ^      ^
    //                                                         bits holdng size
    //                                                         of local buffer
    //


    // heap memory block
    struct heapblk {
        size_t size; // 64/32 bits
        size_t bits; // 64/32 bits
        _Word* addr; // 64/32 bits
    };
    // total size: 194 bits (24 bytes) on x64 target
    // total size: 96 bits (12 bytes) on x86 target


    static constexpr size_t pad_size = sizeof(heapblk) / sizeof(uint8_t) - 1;
    static constexpr size_t sbo_size = sizeof(heapblk) / sizeof(_Word);
    static constexpr size_t sbo_bits = sbo_size * bit_traits<_Word>::bpw - CHAR_BIT;
    static constexpr _Word sbo_mask = ~((_Word(1) << (bit_traits<_Word>::bpw - CHAR_BIT)) - 1);

    union {
        heapblk __m_heap_buf;
        // local buffer (3*64 bits on x64, 3*32 bits on x86)
        _Word   __m_local_buf[sbo_size];
        struct {
            // 23 bytes (184 bits) of __m_unused
            uint8_t __m_unused[pad_size];
            // 1 byte (8 _bits) of local bit counter (0-184)
            uint8_t __m_bits;
        };
    };

    inline bool is_heap() const {
        return (__m_bits == 0);
    }

public:
    typedef bit_traits<_Word> traits_type;
    typedef bit_iterator<_Word, true>  const_iterator;
    typedef bit_iterator<_Word, false> iterator;

    static constexpr size_t max_size = size_t(-1);
    static constexpr size_t bpw = traits_type::bpw;


    bit_storage() {
        // clear buffers
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
    }

    bit_storage(const _Alloc& al) :
        base_type(al) {
        // clear buffers
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
    }

    bit_storage(size_t n, bool v)
    {
        // clear buffers
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));

        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n == 0) // nothing to allocate
            return;

        if (n > sbo_bits) { // can't fit into local buffer
            size_t size = traits_type::bit_space(n);
            __m_heap_buf.addr = this->allocate(size); // allocate space
            __m_heap_buf.size = size; // assign newSize
            __m_heap_buf.bits = n; // assign number of bits
            fill_n(begin(), n, v); // fill heap space
        }
        else {
            __m_bits = static_cast<uint8_t>(n); // assign number of bits
            fill_n(begin(), n, v); // fill local space
        }
    }


    bit_storage(const bit_storage& other) :
        base_type(other)
    {
        if (other.is_heap())
        {
            // allocate space
            __m_heap_buf.addr = this->allocate(other.__m_heap_buf.size);
            // copy internal buffer
            std::copy_n(other.__m_heap_buf.addr, other.__m_heap_buf.size, __m_heap_buf.addr);

            __m_heap_buf.size = other.__m_heap_buf.size; // assign size
            __m_heap_buf.bits = other.__m_heap_buf.bits; // assign number of bits
        }
        else {
            // copy all internal data (including size and number of bits)
            std::copy_n(other.__m_local_buf, sbo_size, __m_local_buf);
        }
    }

    // C++11 Move contructor
    bit_storage(bit_storage&& other)
    {
        assign(std::forward(other));
    }

    bit_storage& operator=(const bit_storage& other)
    {
        if (&other == this)
            return (*this); // escape self assignment

        if (other.nbits() == 0)  // other is empty
            clear(); // clear internal buffers, reset sizes
        else
            copy(other); // copy internal buffers from other

        return(*this);
    }

    // C++11 Move assignment operator
    bit_storage& operator=(bit_storage&& other)
    {
        if (&other == this)
            return (*this);

        assign(std::forward(other));
        return (*this);
    }

    // dtor
    ~bit_storage() {
        clear();
    }

    void copy(const bit_storage& other)
    {
        if (other.is_heap()) // other is heap allocated
        {
            size_t n = other.__m_heap_buf.size;
            if (__m_heap_buf.size != n) // size differ
            {
                if (__m_heap_buf.addr != nullptr && is_heap()) {
                    // deallocate heap buffer of this storage
                    this->deallocate(__m_heap_buf.addr, __m_heap_buf.size);
                }
                // allocate space
                __m_heap_buf.addr = this->allocate(n);
                // reset size
                __m_heap_buf.size = n;
            }
            // copy heap allocated data
            std::copy_n(other.__m_heap_buf.addr, __m_heap_buf.size, __m_heap_buf.addr);
            // reset number of bits
            __m_heap_buf.bits = other.__m_heap_buf.bits;
        }
        else { // other is stack allocated
            if (__m_heap_buf.addr != nullptr && is_heap()) {
                // deallocate heap buffer of this storage
                this->deallocate(__m_heap_buf.addr, __m_heap_buf.size);
            }
            // copy all internal data (including size and number of bits)
            std::copy_n(other.__m_local_buf, sbo_size, __m_local_buf);
        }
    }

    // move semantics
    void assign(bit_storage&& other)
    {
        if (this->get_alloc() == other.get_alloc()) {
            // if allocators is equal we can memmove everything
            std::memmove(__m_local_buf, other.__m_local_buf, sizeof(__m_local_buf));
            // clear contents of other (but DO NOT deallocate data)
            std::memset(other.__m_local_buf, 0, sizeof(other.__m_local_buf));
        }
        else {
            // copy internal data
            copy(other);
            // deallocate and clear other
            other.clear();
        }
    }

    template<class _It>
    void assign(size_t n, _It first, _It last)
    {
        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n == 0) {
            clear(); // clear everything and moves into local buffer
            return;
        }

        size_t s = nbits(); // save old number of bits
        size_t newSize = traits_type::bit_space(n); // calculate new size in words

        if (n > capacity()) { // grow
            // grow is allways goes into heap
            // since minimal capacity is capacity of local buffer
            _Word* newPtr = this->allocate(newSize); // reserve space in heap
            clear(); // clear all internals (also deallocating heap storage if any)
            std::copy_n(first, newSize, newPtr); // copy data into allocated space
            __m_heap_buf.addr = newPtr; // assign new heap pointer
            __m_heap_buf.size = newSize; // assign new size (and capacity)
            __m_heap_buf.bits = n; // assign actual number of bits
        }
        else { // shrink
            std::copy_n(first, newSize, data()); // copy data into allocated space

            if (is_heap()) {
                __m_heap_buf.bits = n; // assign actual number of bits
            }
            else {
                __m_bits = static_cast<uint8_t>(n); // assign actual number of bits
            }
        }
        trim();
    }

    void reserve(size_t n)
    {
        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n <= capacity())
            return; // never shrinks internal buffers

        // reserve will allways run into heap allocated storage
        // since minumum capacity is capacity of local storage

        size_t bitcnt = nbits();  // save current number of bits

        size_t oldSize = nblocks(); // save current size
        _Word* oldPtr = data(); // save old pointer

        size_t newSize = traits_type::bit_space(n); // calculate new size in words
        _Word* newPtr = this->allocate(newSize); // reserve space in heap

        if (!is_heap()) {
            __m_bits = 0; // zeroize size bits
        }

        std::copy_n(oldPtr, oldSize, newPtr); // copy old data into allocated space

        clear(); // clear all internals (also deallocating heap storage if any)
        __m_heap_buf.addr = newPtr; // assign new heap pointer
        __m_heap_buf.size = newSize; // assign new size (and capacity)
        __m_heap_buf.bits = bitcnt; // assign actual number of bits
    }

    void resize(size_t n, bool v)
    {
        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n == 0) {
            clear(); // clear everything and moves into local buffer
            return;
        }

        size_t s = nbits(); // save old number of bits
        size_t oldSize = nblocks(); // save current size
        size_t newSize = traits_type::bit_space(n); // calculate new size in words
        _Word* oldPtr = data(); // save old pointer
        _Word* newPtr = nullptr;

        if (n > capacity()) { // grow
            // grow is allways goes into heap
            // since minimal capacity is capacity of local buffer
            newPtr = this->allocate(newSize); // reserve space in heap
            std::copy_n(oldPtr, oldSize, newPtr); // copy old data into allocated space
            clear(); // clear all internals (also deallocating heap storage if any)
            __m_heap_buf.addr = newPtr; // assign new heap pointer
            __m_heap_buf.size = newSize; // assign new size (and capacity)
            __m_heap_buf.bits = n; // assign actual number of bits
            std::fill_n(begin() + s, n - s, v); // fill trailing bits
        } else { // shrink
            if (n > s) {
                // fill trailing bits with value of v
                fill_n(begin() + s, n - s, v);
            }
            /*else {
                size_t b = (traits_type::bpw - traits_type::bit_index(s)) + s; // word boundary
                fill_n(begin() + n, b - n, false);
            }*/

            if (is_heap()) {
                __m_heap_buf.bits = n; // assign actual number of bits
            }
            else {
                __m_bits = static_cast<uint8_t>(n); // assign actual number of bits
            }

            trim(); // fill (trim) trailing bits up to word boundary with 0 bits
        }
    }

    void shrink()
    {
        size_t s = nbits();
        if (s == 0)
            return; // nothing to shrink

        // compute number of required blocks
        size_t newSize = traits_type::bit_space(s);
        if (newSize == nblocks())
            return; // nothing to shrink

        if (s < sbo_bits)
        {
            if (is_heap())
            {
                _Word* oldPtr = __m_heap_buf.addr;
                size_t oldSize = __m_heap_buf.size;
                // clear local buffer
                std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
                // copy all heap data into local storage
                std::copy_n(oldPtr, newSize, __m_local_buf);
                // deallocate heap buffer of this storage
                this->deallocate(oldPtr, oldSize);
            }
            __m_bits = static_cast<uint8_t>(s); // reset number of bits
            trim();
        }
        else {
            _Word* oldPtr = __m_heap_buf.addr;
            size_t oldSize = __m_heap_buf.size;
            // allocate space
            _Word* newPtr = this->allocate(newSize);
            // copy old blocks into new space
            std::copy_n(oldPtr, newSize, newPtr);
            // deallocate old heap buffer of this storage
            this->deallocate(oldPtr, oldSize);
            __m_heap_buf.addr = newPtr; // assign new pointer
            __m_heap_buf.size = newSize; // assign newSize
            __m_heap_buf.bits = s; // assign number of bits
        }
    }

    void trim()
    {
        size_t s = nbits(); // extract number of bits
        if (s % bpw != 0) {
            size_t n = traits_type::bit_space(s); // calculate number of words
            size_t i = (n == 0 ? 0 : (n - 1)); // index of last word
            _Word mask = (traits_type::bit_mask(s) - 1);
            if (n == sbo_size && !is_heap())
                mask |= sbo_mask;
            data()[i] &= mask; // mask off extra bits
        }
    }

    void clear()
    {
        if (__m_heap_buf.addr != nullptr && is_heap()) {
            this->deallocate(__m_heap_buf.addr, __m_heap_buf.size);
        }
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
    }

    void lshift(size_t pos)
    {	// shift left by _Pos
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = nblocks() - 1;
        const size_t _bits = nbits();
        _Word* ptr = data();
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = _nwords; 0 <= _wpos; --_wpos) // shift by words
                ptr[_wpos] = _wordshift <= _wpos ? ptr[_wpos - _wordshift] : (_Word)0;

        if ((pos %= bpw) != 0)
        {	// 0 < _Pos < _Bitsperword, shift by bits
            for (std::ptrdiff_t _wpos = _nwords; 0 < _wpos; --_wpos)
                ptr[_wpos] = (_Word)((ptr[_wpos] << pos) | (ptr[_wpos - 1] >> (bpw - pos)));
            ptr[0] <<= pos;
        }

        if (is_heap() && __m_heap_buf.addr != nullptr)
            __m_heap_buf.bits = _bits;
        else
            __m_bits = _bits;

        trim();
    }


    void rshift(size_t pos)
    {	// shift right by _Pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = nblocks() - 1;
        const size_t _bits = nbits();
        _Word* ptr = data();
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = 0; _wpos <= (std::ptrdiff_t)_nwords; ++_wpos)
                ptr[_wpos] = (_wordshift <= (std::ptrdiff_t)_nwords - _wpos ? ptr[_wpos + _wordshift] : (_Word)0);

        if ((pos %= bpw) != 0)
        {	// 0 < _Pos < _Bitsperword, shift by bits
            for (std::ptrdiff_t _wpos = 0; _wpos < (std::ptrdiff_t)_nwords; ++_wpos)
                ptr[_wpos] = (_Word)((ptr[_wpos] >> pos) | (ptr[_wpos + 1] << (bpw - pos)));
            ptr[_nwords] >>= pos;
        }

        if (is_heap() && __m_heap_buf.addr != nullptr)
            __m_heap_buf.bits = _bits;
        else
            __m_bits = _bits;

        trim();
    }


    iterator begin() { return iterator(data(), 0); }
    const_iterator begin() const { return const_iterator(const_cast<_Word*>(data()), 0); }

    iterator end() { return iterator(data() + traits_type::block_index(nbits()), traits_type::bit_index(nbits())); }
    const_iterator end() const { return const_iterator(const_cast<_Word*>(data()) + traits_type::block_index(nbits()), traits_type::bit_index(nbits())); }

    _Word* data() {
        return is_heap() ? __m_heap_buf.addr : __m_local_buf;
    }

    const _Word* data() const {
        return is_heap() ? __m_heap_buf.addr : __m_local_buf;
    }

    size_t nblocks() const {
        return is_heap() ? __m_heap_buf.size : traits_type::bit_space(__m_bits);
    }

    size_t nbits() const {
        return is_heap() ? __m_heap_buf.bits : __m_bits;
    }

    size_t capacity() const {
        return (is_heap() ? (std::max)(__m_heap_buf.size * bpw, sbo_bits) : sbo_bits);
    }

};



template<
        class _Word,
        class _Alloc
        >
class bit_storage<_Word, _Alloc, tag_opt> :
        alloc_ebo<_Alloc>
{
    typedef alloc_ebo<_Alloc> base_type;

    static_assert(CHAR_BIT == 8, "incompatible number of bits in char");
    static_assert(sizeof(void*) == sizeof(size_t), "incompatible pointer model");
    static_assert(sizeof(void*) == sizeof(uint64_t), "incompatible pointer size"); // this code for x64 only
    static_assert(sizeof(uintptr_t) == sizeof(uint64_t), "incompatible pointer size"); // this code for x64 only

    //
    // Memory layout (x64 target only)
    //
    // s - bits representing current size of heap allocated word buffer (number of words)
    // c - bits representing current capacity of bit vector (number of bits)
    // p - bits of heap allocated words pointer
    // b - bits of local buffer
    // k - bits representing size of local buffer (number of bits)
    // 0 - zero-bit
    //
    // Dymanic buffer allocation
    //
    // current capacity of bit vector     size of heap buffer
    // |                                  |
    // cccccccccccccccccccccccccccccccccccsssssssssssssssssssssssssssss
    // 000pppppppppppppppppppppppppppppppppppppppppppp00000000000000001
    //    ^                                                           ^
    //    pointer bits                                   heap allocated
    //                                                    signaling bit
    //
    // Static buffer allocation
    // bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
    // bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbkkkkkkk0
    //                                                         ^     ^
    //                                                         bits holdng size
    //                                                         of local buffer
    //

    static constexpr std::size_t size_bits = 29;
    static constexpr std::size_t tag_bits = (sizeof(uint16_t)*CHAR_BIT); // 16 bits maximum
    static constexpr std::size_t ptr_bits = (sizeof(uint64_t)*CHAR_BIT - tag_bits); // 48 bits maximum

    struct heapblk {
        struct {
            size_t bits : (sizeof(size_t) * CHAR_BIT - size_bits); // 35 bits
            size_t size : size_bits; // 29 bits
        };
        union {
            struct {
                uintptr_t addr : ptr_bits; // 48 bits
                uintptr_t padding : tag_bits - 1; // 15 bits
                uintptr_t flag : 1; // 1 bit
            };
            _Word* ptr;
        };
    };


    static constexpr size_t pad_size = sizeof(heapblk) / sizeof(uint8_t) - 1;
    static constexpr size_t sbo_size = sizeof(heapblk) / sizeof(_Word);
    static constexpr size_t sbo_bits = sbo_size * bit_traits<_Word>::bpw - CHAR_BIT;
    static constexpr _Word sbo_mask = ~((_Word(1) << (bit_traits<_Word>::bpw - CHAR_BIT)) - 1);

    union {
        heapblk __m_heap_buf;
        _Word __m_local_buf[sbo_size];
        struct {
            uint8_t __m_unused[pad_size];
            uint8_t __m_bits : CHAR_BIT - 1; // 7 bits
            uint8_t unused : 1; // 1 bit
        };
    };

    inline bool is_heap() const {
        return (__m_heap_buf.flag == 1);
    }

public:
    typedef bit_traits<_Word> traits_type;
    typedef bit_iterator<_Word, true> const_iterator;
    typedef bit_iterator<_Word, false> iterator;

    static constexpr size_t max_size = traits_type::bpw * ((1ULL << size_bits) - 1);
    static constexpr size_t bpw = traits_type::bpw;


    bit_storage() {
        // set all to zero
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
    }

    bit_storage(const _Alloc& al) :
        base_type(al) {
        // set all to zero
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
    }

    bit_storage(size_t n, bool v)
    {
        // set all to zero
        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));

        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n == 0) // nothing to allocate
            return;

        if (n > sbo_bits) {
            size_t size = traits_type::bit_space(n);
            __m_heap_buf.addr = reinterpret_cast<uintptr_t>(this->allocate(size));
            __m_heap_buf.size = size;
            __m_heap_buf.bits = n;
            __m_heap_buf.flag = 1;
            fill_n(begin(), n, v); // fill heap space
        }
        else {
            __m_bits = static_cast<uint8_t>(n);
            __m_heap_buf.flag = 0;
            fill_n(begin(), n, v); // fill local space
        }
    }

    bit_storage(const bit_storage& other) :
        base_type(other)
    {
        if (other.is_heap())
        {
            // allocate space
            __m_heap_buf.addr = reinterpret_cast<uintptr_t>(this->allocate(other.__m_heap_buf.size));
            // copy internal buffer
            std::copy_n(reinterpret_cast<const _Word*>(other.__m_heap_buf.addr),
                        other.__m_heap_buf.size,
                        reinterpret_cast<_Word*>(__m_heap_buf.addr));

            __m_heap_buf.size = other.__m_heap_buf.size; // assign size
            __m_heap_buf.bits = other.__m_heap_buf.bits; // assign number of bits
            __m_heap_buf.flag = 1;
        }
        else {
            // copy all internal data (including size and number of bits)
            std::copy_n(other.__m_local_buf, sbo_size, __m_local_buf);
        }
    }

    bit_storage(bit_storage&& other) {
        assign(std::forward<bit_storage>(other));
    }

    bit_storage& operator=(const bit_storage& other)
    {
        if (&other == this)
            return (*this); // escape self assignment

        if (other.nbits() == 0)  // other is empty
            clear(); // clear internal buffers, reset sizes
        else
            copy(other);

        return(*this);
    }

    // C++11 Move assignment operator
    bit_storage& operator=(bit_storage&& other)
    {
        if (&other == this)
            return (*this);

        assign(std::forward(other));
        return (*this);
    }

    ~bit_storage() {
        clear();
    }

    // copy semantics
    void copy(const bit_storage& other)
    {
        if (other.is_heap()) // other is heap allocated
        {
            size_t n = other.__m_heap_buf.size;
            if (__m_heap_buf.size != n) // size differ
            {
                if (__m_heap_buf.addr != uintptr_t(0) && is_heap()) {
                    // deallocate heap buffer of this storage
                    this->deallocate(reinterpret_cast<_Word*>(__m_heap_buf.addr), __m_heap_buf.size);
                }
                // allocate space
                __m_heap_buf.addr = reinterpret_cast<uintptr_t>(this->allocate(n));
                // reset size
                __m_heap_buf.size = n;
            }
            // copy heap allocated data
            std::copy_n(reinterpret_cast<const _Word*>(other.__m_heap_buf.addr), __m_heap_buf.size, reinterpret_cast<_Word*>(__m_heap_buf.addr));
            // reset number of bits
            __m_heap_buf.bits = other.__m_heap_buf.bits;
            __m_heap_buf.flag = 1;
        }
        else { // other is stack allocated
            if (__m_heap_buf.addr != uintptr_t(0) && is_heap()) {
                // deallocate heap buffer of this storage
                this->deallocate(reinterpret_cast<_Word*>(__m_heap_buf.addr), __m_heap_buf.size);
            }
            // copy all internal data (including size and number of bits)
            std::copy_n(other.__m_local_buf, sbo_size, __m_local_buf);
        }
    }

    // move semantics
    void assign(bit_storage&& other)
    {
        if (this->get_alloc() == other.get_alloc()) {
            // if allocators is equal we can memmove everything
            std::memmove(__m_local_buf, other.__m_local_buf, sizeof(__m_local_buf));
            // clear contents of other (but DO NOT deallocate data)
            std::memset(other.__m_local_buf, 0, sizeof(other.__m_local_buf));
        }
        else {
            // copy internal data
            copy(other);
            // deallocate and clear other
            other.clear();
        }
    }

    template<class _It>
    void assign(size_t n, _It first, _It last)
    {
        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n == 0) {
            clear(); // clear everything and moves into local buffer
            return;
        }

        size_t s = nbits(); // save old number of bits
        size_t newSize = traits_type::bit_space(n); // calculate new size in words

        if (n > capacity()) { // grow
            // grow is allways goes into heap
            // since minimal capacity is capacity of local buffer
            _Word* newPtr = this->allocate(newSize); // reserve space in heap
            clear(); // clear all internals (also deallocating heap storage if any)
            std::copy_n(first, newSize, newPtr); // copy data into allocated space
            __m_heap_buf.addr = newPtr; // assign new heap pointer
            __m_heap_buf.size = newSize; // assign new size (and capacity)
            __m_heap_buf.bits = n; // assign actual number of bits
            __m_heap_buf.flag = 1;
        }
        else { // shrink
            std::copy_n(first, newSize, data()); // copy data into local space

            if (is_heap()) {
                __m_heap_buf.bits = n; // assign actual number of bits
            }
            else {
                __m_bits = static_cast<uint8_t>(n); // assign actual number of bits
            }
        }
        trim(); // zeroize extra bits
    }

    void reserve(size_t n)
    {
        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n <= capacity())
            return; // never shrinks internal buffers

        // reserve will allways run into heap allocated storage
        // since minumum capacity is capacity of local storage

        size_t bitcnt = nbits();  // save current number of bits

        size_t oldSize = nblocks(); // save current size
        _Word* oldPtr = data(); // save old pointer

        size_t newSize = traits_type::bit_space(n); // calculate new size in words
        _Word* newPtr = this->allocate(newSize); // reserve space in heap

        if (!is_heap()) {
            __m_bits = 0; // zeroize size bits
        }

        std::copy_n(oldPtr, oldSize, newPtr); // copy old data into allocated space

        clear(); // clear all internals (also deallocating heap storage if any)
        __m_heap_buf.addr = reinterpret_cast<uintptr_t>(newPtr); // assign new heap pointer
        __m_heap_buf.size = newSize; // assign new size (and capacity)
        __m_heap_buf.bits = bitcnt; // assign actual number of bits
        __m_heap_buf.flag = 1;
    }

    void resize(size_t n, bool v)
    {
        // requested size is too large
        __stdx_assert(n < max_size, std::overflow_error, "bit capacity overflow");

        if (n == 0) {
            clear(); // clear everything and moves into local buffer
            return;
        }

        size_t s = nbits(); // save old number of bits
        size_t oldSize = nblocks(); // save current size
        size_t newSize = traits_type::bit_space(n); // calculate new size in words
        _Word* oldPtr = data(); // save old pointer
        _Word* newPtr = nullptr;

        if (n > capacity()) { // grow
            // grow is allways goes into heap since minimal
            // capacity is capacity of local buffer
            newPtr = this->allocate(newSize); // reserve space in heap
            std::copy_n(oldPtr, oldSize, newPtr); // copy old data into allocated space
            clear(); // clear all internals (also deallocating heap storage if any)
            __m_heap_buf.addr = reinterpret_cast<uintptr_t>(newPtr); // assign new heap pointer
            __m_heap_buf.size = newSize; // assign new size (and capacity)
            __m_heap_buf.bits = n; // assign actual number of _bits
            __m_heap_buf.flag = 1;
            std::fill_n(begin() + s, n - s, v); // fill trailing bits
        }
        else { // shrink
            if (n > s) {
                // fill trailing bits with value of v
                fill_n(begin() + s, n - s, v);
            }
            /*else {
                size_t b = (traits_type::bpw - traits_type::bit_index(s)) + s; // word boundary
                fill_n(begin() + n, b - n, false);
            }*/

            if (is_heap()) {
                __m_heap_buf.bits = n; // assign actual number of bits
                __m_heap_buf.flag = 1;
            }
            else {
                __m_bits = static_cast<uint8_t>(n); // assign actual number of bits
                __m_heap_buf.flag = 0;
            }

            trim(); // trim (fill) trailing bits up to word boundary with 0 bits
        }
    }

    void shrink()
    {
        size_t s = nbits();
        if (s == 0) // nothing to shrink
            return;

        // compute number of required blocks
        size_t newSize = traits_type::bit_space(s);
        if (newSize == nblocks())  // nothing to shrink
            return;

        if (s < sbo_bits)
        {
            if (is_heap())
            {
                _Word* oldPtr = reinterpret_cast<_Word*>(__m_heap_buf.addr);
                size_t oldSize = __m_heap_buf.size;
                // clear local buffer
                std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
                // copy all heap data into local storage
                std::copy_n(oldPtr, newSize, __m_local_buf);
                // deallocate heap buffer of this storage
                this->deallocate(oldPtr, oldSize);
                __m_heap_buf.flag = 0;
            }
            __m_bits = s; // reset number of bits
            trim();
        }
        else {
            _Word* oldPtr = reinterpret_cast<_Word*>(__m_heap_buf.addr);
            size_t oldSize = __m_heap_buf.size;
            // allocate space
            _Word* newPtr = this->allocate(newSize);
            // copy old blocks into new space
            std::copy_n(oldPtr, newSize, newPtr);
            // deallocate old heap buffer of this storage
            this->deallocate(oldPtr, oldSize);
            __m_heap_buf.addr = reinterpret_cast<uintptr_t>(newPtr); // assign new pointer
            __m_heap_buf.size = newSize; // assign newSize
            __m_heap_buf.bits = s; // assign number of bits
            __m_heap_buf.flag = 1;
        }
    }

    void trim()
    {
        size_t s = nbits(); // extract number of bits
        if (s % bpw != 0) {
            size_t n = traits_type::bit_space(s); // calculate number of words
            size_t i = (n == 0 ? 0 : (n - 1)); // index of last word
            _Word mask = (traits_type::bit_mask(s) - 1);
            if (n == sbo_size && !is_heap())
                mask |= sbo_mask;
            data()[i] &= mask; // mask off extra bits
        }
    }

    void clear()
    {
        if (__m_heap_buf.addr != 0 && is_heap())
            this->deallocate(reinterpret_cast<_Word*>(__m_heap_buf.addr), __m_heap_buf.size);

        std::memset(__m_local_buf, 0, sizeof(__m_local_buf));
    }

    void lshift(size_t pos)
    {	// shift left by _Pos
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = nblocks() - 1;
        const size_t _bits = nbits();
        _Word* ptr = data();
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = _nwords; 0 <= _wpos; --_wpos) // shift by words
                ptr[_wpos] = _wordshift <= _wpos ? ptr[_wpos - _wordshift] : (_Word)0;

        if ((pos %= bpw) != 0)
        {	// 0 < _Pos < _Bitsperword, shift by bits
            for (std::ptrdiff_t _wpos = _nwords; 0 < _wpos; --_wpos)
                ptr[_wpos] = (_Word)((ptr[_wpos] << pos) | (ptr[_wpos - 1] >> (bpw - pos)));
            ptr[0] <<= pos;
        }

        if (is_heap() && __m_heap_buf.addr != 0)
            __m_heap_buf.bits = _bits;
        else
            __m_bits = _bits;

        trim();
    }


    void rshift(size_t pos)
    {	// shift right by _Pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = nblocks() - 1;
        const size_t _bits = nbits();
        _Word* ptr = data();
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = 0; _wpos <= (std::ptrdiff_t)_nwords; ++_wpos)
                ptr[_wpos] = (_wordshift <= (std::ptrdiff_t)_nwords - _wpos ? ptr[_wpos + _wordshift] : (_Word)0);

        if ((pos %= bpw) != 0)
        {	// 0 < _Pos < _Bitsperword, shift by bits
            for (std::ptrdiff_t _wpos = 0; _wpos < (std::ptrdiff_t)_nwords; ++_wpos)
                ptr[_wpos] = (_Word)((ptr[_wpos] >> pos) | (ptr[_wpos + 1] << (bpw - pos)));
            ptr[_nwords] >>= pos;
        }

        if (is_heap() && __m_heap_buf.addr != 0)
            __m_heap_buf.bits = _bits;
        else
            __m_bits = _bits;

        trim();
    }


    iterator begin() { return iterator(data(), 0); }
    const_iterator begin() const { return const_iterator(data(), 0); }

    iterator end() { return iterator(data() + traits_type::block_index(nbits()), traits_type::bit_index(nbits())); }
    const_iterator end() const { return const_iterator(data() + traits_type::block_index(nbits()), traits_type::bit_index(nbits())); }

    _Word* data() {
        return (is_heap() ? reinterpret_cast<_Word*>(__m_heap_buf.addr) : __m_local_buf);
    }

    const _Word* data() const {
        return (is_heap() ? reinterpret_cast<_Word*>(__m_heap_buf.addr) : __m_local_buf);
    }

    size_t nblocks() const {
        return (is_heap() ? __m_heap_buf.size : traits_type::bit_space(__m_bits));
    }

    size_t nbits() const {
        return (is_heap() ? __m_heap_buf.bits : __m_bits);
    }

    size_t capacity() const {
        return (is_heap() ? (std::max)(__m_heap_buf.size * bpw, size_t(sbo_bits)) : size_t(sbo_bits));
    }
};
#endif // STDX_PROCESSOR_X86_64

} // end namespace detail

_STDX_END 

