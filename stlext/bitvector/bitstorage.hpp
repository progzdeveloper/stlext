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
    _Word __buf[2];
    static constexpr size_t  __local_offset = 1;
    static constexpr _Word   __local_mask   = _Word(~uint8_t(0)) << (sizeof(_Word) * CHAR_BIT - CHAR_BIT);
public:
    typedef _Alloc            allocator_type;
    typedef size_t            size_type;
    typedef _Word             word_type;
    typedef _Word*            pointer;
    typedef const _Word*      const_pointer;
    typedef bit_traits<_Word> traits_type;

    static constexpr size_t max_bits = size_t(1) << (sizeof(size_t) * CHAR_BIT - 1);
    static constexpr size_t max_local_bits = ( (sizeof(__buf) - 1) * CHAR_BIT );
    static constexpr size_t bpw = traits_type::bpw;


    bitstorage()
    {
        __clearmem(__buf, sizeof(__buf)/sizeof(__buf[0]));
    }

    bitstorage(const _Alloc& al)
        : _Alloc(al)
    {
        __clearmem(__buf, sizeof(__buf)/sizeof(__buf[0]));
    }

    bitstorage(const bitstorage& other) :
        _Alloc(other)
    {
        bool _s_local = other.__is_local();
        size_t nbits = other.__get_nbits(_s_local);
        if (!_s_local) {
            pointer addr = __allocate_blocks(nbits);
            __movemem(addr, other.__get_heap(), traits_type::bit_space(nbits));
            __put_heap(addr, nbits);
        } else {
            __put_local(other.__get_local(), nbits);
        }
    }

    bitstorage operator=(const bitstorage& other) = delete;

    ~bitstorage() {
        clear();
    }

    allocator_type get_allocator() const { return (*this); }


    pointer data() {
        return __get_ptr(__is_local());
    }

    const_pointer data() const {
        return __get_ptr(__is_local());
    }

    size_t size() const {
        return __get_nbits(__is_local());
    }

    void clear()
    {
        if (__is_heap()) {
            this->__deallocate_blocks(__get_heap(),
                                      __get_nbits(false));
        }
        __clearmem(__buf, sizeof(__buf)/sizeof(__buf[0]));
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

        size_t nbits = size();
        if (n == nbits)
            return;

        if (n < nbits) {
            __sqeeze(nbits, nbits - n);
        } else {
            __expand(nbits, n - nbits);
        }
    }

    void __sanitize_bits()
    {
        // FIXME: test me carefully!!!
        // FIXME: fix bugs!!!
        // get storage type
        bool _s_local = __is_local();
        // get actual number of bits
        size_t n = __get_nbits(_s_local);
        // get simple mask
        _Word mask = (n % bpw != 0) ? ((static_cast<_Word>(1) << (n % bpw)) - 1) : (~_Word(0));
        // conditionally set sbo_mask bits without branching
        mask |= (mask & ~__local_mask) | (-_s_local & __local_mask);
        // mask higest bits of higest word
        __buf[traits_type::block_index(n)] &= mask;
    }

    void __bitwise_shl(size_t pos)
    {
        bool _s_local = __is_local();
        size_t _s_bits = __get_nbits(_s_local);
        pointer blocks = __get_ptr(_s_local);

        // shift left by pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = traits_type::bit_space(_s_bits) - 1;
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
            __put_local(blocks, _s_bits);
        }
        __sanitize_bits();
    }

    void __bitwise_shr(size_t pos)
    {
        bool _s_local = __is_local();
        size_t _s_bits = __get_nbits(_s_local);
        pointer blocks = __get_ptr(_s_local);

        // shift right by pos, first by words then by bits
        const std::ptrdiff_t _wordshift = (std::ptrdiff_t)traits_type::block_index(pos);
        const size_t _nwords = traits_type::bit_space(_s_bits) - 1;
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
            __put_local(blocks, _s_bits);
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
    static inline void __movemem(T* addr_dst, T* addr_src, size_t n) {
#ifdef STDX_CMPLR_GNU
        __builtin_memmove(addr_dst, addr_src, sizeof(T)*n);
#else
        std::memmove(addr_dst, addr_src, sizeof(T)*n);
#endif
    }

    inline void __put_heap(_Word* addr, size_t nbits) {
        if (addr != __buf)
            __buf[0] = reinterpret_cast<uintptr_t>(addr);
        __buf[1] = nbits;
        __buf[1] |= _Word(1) << (bpw - 1);
    }

    inline pointer __get_heap() const {
        return reinterpret_cast<pointer>(__buf[0]);
    }

    inline void __put_local(_Word* addr, size_t nbits)
    {
        uint8_t* p = reinterpret_cast<uint8_t*>(__buf);
        if (addr != __buf) {
            __movemem(p, reinterpret_cast<uint8_t*>(addr), (nbits + CHAR_BIT-1) / CHAR_BIT);
        }
        p[sizeof(__buf)-1] = nbits;
    }

    inline pointer __get_local() const {
        return const_cast<pointer>(__buf);
    }


    inline pointer __get_ptr(bool __local) const {
        return ( __local ? __get_local() : __get_heap()) ;
    }

    inline size_t __get_nbits(bool __local) const {
        return ( __local ?
                     static_cast<size_t>(__buf[__local_offset] >> (bpw - CHAR_BIT)) :
                     static_cast<size_t>(__buf[__local_offset] & ~(_Word(1) << (bpw - 1))) );
    }



    inline bool __is_local() const {
        return !__is_heap();
    }

    inline bool __is_heap() const {
        return ((__buf[__local_offset] >> (bpw-1)) == 0x1);
    }



    inline pointer __allocate_blocks(size_t nbits) {
        return static_cast<_Alloc*>(this)->allocate(traits_type::bit_space(nbits));
    }

    inline void __deallocate_blocks(pointer p, size_t nbits) {
        if (nbits > 0)
            static_cast<_Alloc*>(this)->deallocate(p, traits_type::bit_space(nbits));
    }


    void __expand(size_t nbits, size_t amount)
    {
        if ((nbits % bpw) > amount) {
            if (__is_local()) {
                __put_local(__buf, nbits + amount);
            } else {
                __put_heap(__buf, nbits + amount);
            }
            return;
        }

        pointer ptr = nullptr;
        if (__is_local()) {
            if ((amount + nbits) < max_local_bits) {
                __put_local(__buf, nbits + amount);
                return;
            }
            ptr = __get_local();
        } else {
            ptr = __get_heap();
        }

        size_t new_size = nbits + amount;
        pointer addr = this->__allocate_blocks(new_size);
        if (ptr != nullptr)
            __movemem((uint8_t*)addr, (uint8_t*)ptr, bit_traits<uint8_t>::bit_space(nbits));

        if (__is_heap())
            this->__deallocate_blocks(ptr, nbits);

        __put_heap(addr, new_size);
    }

    void __sqeeze(size_t nbits, size_t amount)
    {
        // FIXME: bugs!
        if ((nbits % bpw) > amount) {
            if (__is_local()) {
                __put_local(__buf, nbits - amount);
            } else {
                __put_heap(__buf, nbits - amount);
            }
            __sanitize_bits();
            return;
        }

        if (__is_local()) {
            __put_local(__buf, nbits - amount);
            __sanitize_bits();
            return;
        }

        size_t new_size = nbits - amount;
        pointer ptr = __get_heap();
        if (new_size < max_local_bits) {
            __put_local(ptr, new_size);
        } else {
            pointer addr = this->__allocate_blocks(new_size);
            __movemem(addr, ptr, traits_type::bit_space(new_size));
            __put_heap(addr, new_size);
        }

        // deallocate old pointer
        this->__deallocate_blocks(ptr, nbits);

        // clear unused bits
        __sanitize_bits();
    }

};

#endif


/*
template<class _Word, class _Alloc>
class bitstorage<compact, _Word, _Alloc> : _Alloc
{

};*/


_STDX_END // end namespace stdx


