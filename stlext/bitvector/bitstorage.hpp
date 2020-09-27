// Copyright (c) 2020, Michael Polukarov (Russia).
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

#include <vector>

#include "bittraits.hpp"
#include "bitalgo.hpp"

_STDX_BEGIN

enum bitspace_optimization {
    none,
    sbo,
    compact
};


static constexpr size_t __DefaultBitStorageTag =
    //bitspace_optimization::none;
    bitspace_optimization::sbo;


template<size_t _Tag, class _Word, class _Alloc>
class bitstorage;

template<class _Word, class _Alloc>
class bitstorage<none, _Word, _Alloc>
{
public:
    typedef _Alloc            allocator_type;
    typedef _Word             word_type;
    typedef _Word*            pointer;
    typedef const _Word*      const_pointer;
    typedef bit_traits<_Word> traits_type;

    static constexpr size_t max_bits = ~(size_t(0));
    static constexpr size_t max_local_bits = 0;
    static constexpr size_t bpw = traits_type::bpw;

    bitstorage() : __m_nbits(0) {}
    bitstorage(const _Alloc& al) : __m_blocks(al), __m_nbits(0) {}

    bitstorage operator=(const bitstorage& other) = delete;

    allocator_type get_allocator() const {
        return __m_blocks.get_allocator();
    }
    pointer data() { return __m_blocks.data(); }
    const_pointer data() const { return __m_blocks.data(); }
    size_t size() const { return __m_nbits; }

    void clear() { __m_blocks.clear(); __m_nbits = 0; }
    void shrink() { __m_blocks.shrink_to_fit(); }

    bitstorage& move(bitstorage&& other) {
        __m_blocks = std::move(other.__m_blocks);
        __m_nbits = std::move(__m_nbits);
        return (*this);
    }

    void swap(bitstorage& other) {
        std::swap(__m_blocks, other.__m_blocks);
        std::swap(__m_nbits, other.__m_nbits);
    }

protected:


    void __bitwise_shl(size_t pos)
    {
        // shift left by pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = __m_blocks.size() - 1;
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = _nwords; 0 <= _wpos; --_wpos) // shift by words
                __m_blocks[_wpos] = _wordshift <= _wpos ? __m_blocks[_wpos - _wordshift] : (_Word)0;

        if ((pos %= bpw) != 0)
        {   // 0 < pos < bpw, shift by bits
            for (std::ptrdiff_t _wpos = _nwords; 0 < _wpos; --_wpos)
                __m_blocks[_wpos] = (_Word)((__m_blocks[_wpos] << pos) | (__m_blocks[_wpos - 1] >> (bpw - pos)));
            __m_blocks[0] <<= pos;
        }
        __sanitize_bits();
    }

    void __bitwise_shr(size_t pos)
    {
        // shift right by pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = __m_blocks.size() - 1;
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = 0; _wpos <= (std::ptrdiff_t)_nwords; ++_wpos)
                __m_blocks[_wpos] = (_wordshift <= (std::ptrdiff_t)_nwords - _wpos ? __m_blocks[_wpos + _wordshift] : (_Word)0);

        if ((pos %= bpw) != 0)
        {   // 0 < pos < bpw, shift by bits
            for (std::ptrdiff_t _wpos = 0; _wpos < (std::ptrdiff_t)_nwords; ++_wpos)
                __m_blocks[_wpos] = (_Word)((__m_blocks[_wpos] >> pos) | (__m_blocks[_wpos + 1] << (bpw - pos)));
            __m_blocks[_nwords] >>= pos;
        }
        __sanitize_bits();
    }

    void __resize(size_t nbits) {
        __m_blocks.resize(traits_type::bit_space(nbits));
        __m_nbits = nbits;
    }

    void __sanitize_bits() {
        if ((__m_nbits % bpw) != 0)
            __m_blocks.back() &= (static_cast<_Word>(1) << (__m_nbits % bpw)) - 1;
    }

private:
    std::vector<_Word, _Alloc> __m_blocks;
    size_t __m_nbits;
};



#if (CHAR_BIT == 8)

template<class _Word, class _Alloc>
class bitstorage<sbo, _Word, _Alloc> : _Alloc
{
    static_assert(sizeof(_Word) <= sizeof(uintptr_t), "");


    // bits per pointer
    static const size_t bpp = sizeof(uintptr_t)*CHAR_BIT;

    // maximum number of local bits
    static const size_t max_local_bits = 2 * bpp - CHAR_BIT;

    // bit indicated heap allocated storage
    static const uintptr_t heap_bit = uintptr_t(1) << (bpp - 1);

    // bit-mask for local control block	(pointer aligned)
    static const uintptr_t local_mask = (~uintptr_t(0) >> CHAR_BIT);

    // bit-mask for local control block (word aligned)
    static const _Word wlocal_mask = (~_Word(0) >> CHAR_BIT);

    // control block offset in bits
    static const size_t local_offset = bpp - CHAR_BIT;

public:
    typedef _Alloc            allocator_type;
    typedef size_t            size_type;
    typedef _Word             word_type;
    typedef _Word*            pointer;
    typedef const _Word*      const_pointer;
    typedef bit_traits<_Word> traits_type;

    static constexpr size_t max_bits = size_t(1) << (sizeof(size_t) * CHAR_BIT - 1);

    static constexpr size_t bpw = traits_type::bpw;


    bitstorage()
    {
        __clearmem(buf, 2);
    }

    bitstorage(const _Alloc& al)
        : _Alloc(al)
    {
        __clearmem(buf, 2);
    }

    bitstorage(const bitstorage& other) :
        _Alloc(other)
    {
        if (other.__is_local()) {
            __movemem((pointer)buf, (pointer)other.buf, size_t(2));
        } else {
            size_t n = other.size();
            size_t nwords = bit_traits<_Word>::bit_space(n);
            const_pointer wp = other.data();
            pointer p = this->allocate(nwords);
            __movemem(p, wp, nwords);
            __reset(p, n);
        }
    }

    bitstorage operator=(const bitstorage& other) = delete;

    ~bitstorage() {
        this->clear();
    }

    allocator_type get_allocator() const { return (*this); }


    pointer data() {
        return (__is_heap() ?
                    reinterpret_cast<pointer>(buf[0]) :
                    reinterpret_cast<pointer>(buf));
    }

    const_pointer data() const {
        return (__is_heap() ?
                    reinterpret_cast<const_pointer>(buf[0]) :
                    reinterpret_cast<const_pointer>(buf));
    }

    size_t size() const {
        /*if (__is_heap())
            return (buf[1] & ~heap_bit);
         else
            return (buf[1] >> (bpp - CHAR_BIT));
        */
        // branchless version of the above
        unsigned flag =  __is_local();
        return (size_t) ((buf[1] & ~(heap_bit << flag)) >> (local_offset * flag));
    }

    void clear()
    {
        if (__is_heap()) {
            this->deallocate(reinterpret_cast<pointer>(buf[0]), bit_traits<_Word>::bit_space(buf[1] & ~heap_bit));
        }
        __clearmem(buf, 2);
    }

    void shrink() {
        // noop
    }

protected:
    void __resize(size_t n)
    {
        if (n == 0) {
            clear();
            return;
        }

        size_t nbits = this->size();
        if (n < nbits)
            __shrink(nbits, n);
        else {
            __expand(nbits, n);
        }
    }

    void __sanitize_bits()
    {
        // FIXME: test me carefully!!!
        // FIXME: fix bugs!!!

        // get storage type
        unsigned flag = __is_local();
        // get actual number of bits without branching
        size_t n = (size_t) ((buf[1] & ~(heap_bit << flag)) >> (local_offset * flag));
        // get pointer to the words
        pointer wp = flag ? reinterpret_cast<pointer>(buf) : reinterpret_cast<pointer>(buf[0]);

        size_t blk = (n / bpw); // block index
        size_t bit = (n % bpw); // bit index

        // mask bits higher than bit
        _Word mask = (bit != 0) ? ((_Word(1) << bit) - 1) : (~_Word(0));

        size_t i = (sizeof(buf)/sizeof(_Word)-1);
        if (blk == i && flag)
            mask |= ~wlocal_mask;
        // mask higest bits of higest word
        wp[blk] &= mask;
    }

    void __bitwise_shl(size_t pos)
    {
        bool _s_local = __is_local();
        uint8_t _cblk = _s_local ? __get_ctrl() : 0;
        pointer blocks = this->data();

        // shift left by pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = traits_type::bit_space(this->size()) - 1;
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = _nwords; 0 <= _wpos; --_wpos) // shift by words
                blocks[_wpos] = _wordshift <= _wpos ? blocks[_wpos - _wordshift] : (_Word)0;

        if ((pos %= bpw) != 0)
        {   // 0 < pos < bpw, shift by bits
            for (std::ptrdiff_t _wpos = _nwords; 0 < _wpos; --_wpos)
                blocks[_wpos] = (_Word)((blocks[_wpos] << pos) | (blocks[_wpos - 1] >> (bpw - pos)));
            blocks[0] <<= pos;
        }

        if (_s_local) { // restore control block
            __put_ctrl(_cblk);
        }
        __sanitize_bits();
    }

    void __bitwise_shr(size_t pos)
    {
        bool _s_local = __is_local();
        uint8_t _cblk = _s_local ? __get_ctrl() : 0;
        pointer blocks = this->data();

        // shift right by pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = traits_type::bit_space(this->size()) - 1;
        if (_wordshift != 0)
            for (std::ptrdiff_t _wpos = 0; _wpos <= (std::ptrdiff_t)_nwords; ++_wpos)
                blocks[_wpos] = (_wordshift <= (std::ptrdiff_t)_nwords - _wpos ? blocks[_wpos + _wordshift] : (_Word)0);

        if ((pos %= bpw) != 0)
        {   // 0 < pos < bpw, shift by bits
            for (std::ptrdiff_t _wpos = 0; _wpos < (std::ptrdiff_t)_nwords; ++_wpos)
                blocks[_wpos] = (_Word)((blocks[_wpos] >> pos) | (blocks[_wpos + 1] << (bpw - pos)));
            blocks[_nwords] >>= pos;
        }

        if (_s_local) { // restore control block
            __put_ctrl(_cblk);
        }
        __sanitize_bits();
    }


private:
    template<class T>
    static inline void __clearmem(T* ptr, size_t n) {
    #ifdef STDX_CMPLR_GNU
        __builtin_memset(ptr, 0, sizeof(T)*n);
    #else
        std::memset(ptr, 0, sizeof(T)*n);
    #endif
     }

    template<class T>
    static inline void __movemem(T* addr_dst, const T* addr_src, size_t n) {
    #ifdef STDX_CMPLR_GNU
        __builtin_memmove(addr_dst, addr_src, sizeof(T)*n);
    #else
         std::memmove(addr_dst, addr_src, sizeof(T)*n);
    #endif
     }



    uint8_t __get_ctrl() const {
        return (uint8_t)(buf[1] >> local_offset);
    }

    void __put_ctrl(uint8_t ctl) {
        buf[1] &= local_mask;
        buf[1] |= (uintptr_t(ctl) << local_offset);
    }


    void __reset(pointer p, size_t nbits)
    {
        if (nbits < max_local_bits) {
            buf[1] &= local_mask;
            buf[1] |= (nbits << local_offset);
            buf[1] &= ~heap_bit;
        } else {
            buf[0] = (uintptr_t)p;
            buf[1] = (nbits | heap_bit);
        }
    }

    bool __is_heap() const {
        return ((buf[1] & heap_bit) != 0);
    }

    bool __is_local() const {
        return ((buf[1] & heap_bit) == 0);
    }

    void __expand(size_t nbits, size_t n)
    {
        // at this point n is always greater than nbits
        if (n < max_local_bits) {
            __reset((pointer)buf, n);
        } else {
            size_t nwords = bit_traits<_Word>::bit_space(nbits);
            size_t new_nwords = bit_traits<_Word>::bit_space(n);
            pointer wp = this->data();
            pointer p = this->allocate(new_nwords);
            __movemem(p, wp, nwords);
            __reset(p, n);
            if (wp != (pointer)buf) {
                this->deallocate(wp, nwords);
            }
            __sanitize_bits();
        }
    }

    void __shrink(size_t nbits, size_t n)
    {
        // at this point n is always less than nbits
        size_t nwords = bit_traits<_Word>::bit_space(nbits);
        size_t new_words = bit_traits<_Word>::bit_space(n);
        pointer p = nullptr;
        pointer wp = this->data();
        if (n < max_local_bits) {
            p = (pointer)buf;
        } else {
            p = this->allocate(new_words);
        }
        __movemem(p, wp, new_words);
        __reset(p, n);

        if (wp != (pointer)buf) {
            this->deallocate(wp, nwords);
        }

        __sanitize_bits();
    }



private:
    uintptr_t buf[2];
};

#endif


/*
template<class _Word, class _Alloc>
class bitstorage<compact, _Word, _Alloc> : _Alloc
{

};*/


_STDX_END // end namespace stdx


