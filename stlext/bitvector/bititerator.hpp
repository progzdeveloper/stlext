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

#include <climits>
#include <cstdint>
#include <iterator>
#include <type_traits>

#include "../platform/common.h"


_STDX_BEGIN

//internal
template<class _Word, bool _IsConst>
class bit_iterator;

/*!
 * \brief class bit_reference
 *
 * Proxy class for emulating mutable bit-reference
 */
template<class _Word>
class bit_reference
{
    template<class, bool>
    friend class bit_iterator;

    typedef _Word word_type;
public:
    /*!
     * \brief Ctor
     *
     *  Single non-copy constructor
     */
    bit_reference(word_type& __blk, word_type __mask) :
        __m_pblock(__blk),
        __m_mask(__mask)
    {
    }

    /// copy ctor: compiler generated

    /*!
     * \brief address operator overload
     * \return bit iterator for this bit
     */
    inline bit_iterator<_Word, false> operator&() const {
        return bit_iterator<_Word, false>(__m_pblock, static_cast<unsigned>(__ctz(__m_mask)));
    }

    inline operator bool() const { return (__m_pblock & __m_mask) != 0; }
    inline bool operator~() const { return (__m_pblock & __m_mask) == 0; }

    /*! \brief flip bit */
    inline bit_reference& flip() { do_flip(); return *this; }
    /*! \brief assign bit value */
    inline bit_reference& operator=(bool x) noexcept                     { do_assign(x);   return *this; }
    /*! \brief assign bit reference */
    inline bit_reference& operator=(const bit_reference& other) noexcept { do_assign(other); return *this; }
    /*! \brief check for equality */
    inline bool operator==(const bit_reference& x) const { return bool(*this) == bool(x); }
    /*! \brief check for less */
    inline bool operator<(const bit_reference& x) const { return !bool(*this) && bool(x); }
    /*! \brief bitwise OR operator overload */
    inline bit_reference& operator|=(bool x) { if (x) do_set();    return (*this); }
    /*! \brief bitwise AND operator overload */
    inline bit_reference& operator&=(bool x) { if (!x) do_reset(); return (*this); }
    /*! \brief bitwise XOR operator overload */
    inline bit_reference& operator^=(bool x) { if (x) do_flip();   return (*this); }
    /*! \brief bitwise AND-NOT operator overload */
    inline bit_reference& operator-=(bool x) { if (x) do_reset();  return (*this); }

private:
    word_type&      __m_pblock;
    const word_type __m_mask;

    inline void do_set()   { __m_pblock |= __m_mask; }
    inline void do_reset() { __m_pblock &= ~__m_mask; }
    inline void do_flip()  { __m_pblock ^= __m_mask; }
    inline void do_assign(bool x) { (x ? do_set() : do_reset()); }
};


/*!
 * \brief class bit_reference
 *
 * Proxy class for emulating non-mutable (constant) bit-reference
 */
template<class _Word>
class bit_const_reference
{
    template<class, bool>
    friend class bit_iterator;

    typedef _Word word_type;

public:
    /*! \brief constructor
     *   single non-copy constructor
     */
    bit_const_reference(word_type& blk, word_type mask) :
        __m_pblock(blk),
        __m_mask(mask)
    {
    }

    /*!
     * \brief address operator overload
     * \return bit iterator for this bit
     */
    bit_iterator<_Word, true> operator&() const {
        return bit_iterator<_Word, true>(&__m_pblock, static_cast<unsigned>(__ctz(__m_mask)));
    }

    /*!
     * \brief Check if bit set
     * \return true if bit is set, otherwise false
     */
    inline operator bool() const { return (__m_pblock & __m_mask) != 0; }

    /*! \brief check for equality */
    inline bool operator==(const bit_const_reference& x) const { return bool(*this) == bool(x); }
    /*! \brief check for less */
    inline bool operator<(const bit_const_reference& x) const { return !bool(*this) && bool(x); }

private:
    word_type& __m_pblock;
    const word_type __m_mask;
};



/*!
 * \brief class bit_iterator provides bit iterator
 * \tparam _Word type of machine word
 * \tparam _IsConst mark const or non const versions of iterator
 *
 * class bit_iterator models random access bit iterator for
 * implementing STL compatible bit optimized algorithms.
 */
template<class _Word, bool _IsConst>
class bit_iterator
{
private:
    template<class>
    friend class bit_reference;
    template<class>
    friend class bit_const_reference;

    friend class bit_iterator<_Word, false>;
    friend class bit_iterator<_Word, true>;

public:
    typedef std::ptrdiff_t   difference_type;
    typedef bool             value_type;

    typedef typename std::conditional<
        _IsConst,
        bit_const_reference<_Word>,
        bit_reference<_Word>
    >::type   reference;

    typedef typename std::conditional<_IsConst,
        const _Word*,
        _Word*
    >::type   pointer;
    typedef const _Word* const_pointer;

    typedef std::random_access_iterator_tag   iterator_category;

    static const unsigned bpw = CHAR_BIT * sizeof(_Word);

    inline bit_iterator() :
        __m_blk(nullptr), __m_ctz(0) {
    }

    inline bit_iterator(pointer blk, unsigned ctz) :
        __m_blk(const_cast<_Word*>(blk)), __m_ctz(ctz) {
    }

    inline bit_iterator(const bit_iterator& other) :
        __m_blk(other.__m_blk), __m_ctz(other.__m_ctz) {
    }

    template<bool _C>
    inline bit_iterator(const bit_iterator<_Word, _C>& other) :
        __m_blk(other.__m_blk), __m_ctz(other.__m_ctz) {
    }


    // operator overloads

    inline bit_iterator& operator=(const bit_iterator& other) {
        __m_blk = other.__m_blk;
        __m_ctz = other.__m_ctz;
        return (*this);
    }

    template<bool _C>
    inline bit_iterator& operator=(const bit_iterator<_Word, _C>& other) {
        __m_blk = other.__m_blk;
        __m_ctz = other.__m_ctz;
        return (*this);
    }

    // prevent from address taking
    bit_iterator* operator&() { return (this); }

    reference operator*() const noexcept {
        return reference(*__m_blk, _Word(1) << __m_ctz);
    }

    inline reference operator[](difference_type n) const {
        return *(*this + n);
    }

    // return pointer to current word
    inline pointer base() { return __m_blk; }

    // return const-pointer to current word
    inline const_pointer base() const { return __m_blk; }

    // return bit position
    inline unsigned pos() const { return __m_ctz; }

    inline bit_iterator& operator++()
    {
        if (__m_ctz != bpw - 1)
            ++__m_ctz;
        else
        {
            __m_ctz = 0;
            ++__m_blk;
        }
        return *this;
    }

    inline bit_iterator operator++(int)
    {
        bit_iterator __tmp = *this;
        ++(*this);
        return __tmp;
    }

    inline bit_iterator& operator--()
    {
        if (__m_ctz != 0)
            --__m_ctz;
        else
        {
            __m_ctz = bpw - 1;
            --__m_blk;
        }
        return *this;
    }

    inline bit_iterator operator--(int)
    {
        bit_iterator __tmp = *this;
        --(*this);
        return __tmp;
    }

    inline bit_iterator& operator+=(difference_type n)
    {
        if (n >= 0)
            __m_blk += (n + __m_ctz) / bpw;
        else
            __m_blk += static_cast<difference_type>(n - bpw + __m_ctz + 1) / static_cast<difference_type>(bpw);
        n &= (bpw - 1);
        __m_ctz = static_cast<unsigned>((n + __m_ctz) % bpw);
        return *this;
    }

    inline bit_iterator& operator-=(difference_type n)
    {
        return *this += -n;
    }

    inline bit_iterator operator+(difference_type n) const
    {
        bit_iterator __t(*this);
        __t += n;
        return __t;
    }

    inline bit_iterator operator-(difference_type n) const
    {
        bit_iterator __t(*this);
        __t -= n;
        return __t;
    }

    // friend operator overloads

    inline friend bit_iterator operator+(difference_type n, const bit_iterator& it) {
        return (it + n);
    }

    inline friend difference_type operator-(const bit_iterator& x, const bit_iterator& y) {
        return (x.__m_blk - y.__m_blk) * bpw + x.__m_ctz - y.__m_ctz;
    }

    inline friend bool operator==(const bit_iterator& x, const bit_iterator& y) {
        return x.__m_blk == y.__m_blk && x.__m_ctz == y.__m_ctz;
    }

    inline friend bool operator!=(const bit_iterator& x, const bit_iterator& y) {
        return !(x == y);
    }

    inline friend bool operator<(const bit_iterator& x, const bit_iterator& y) {
        return x.__m_blk < y.__m_blk || (x.__m_blk == y.__m_blk && x.__m_ctz < y.__m_ctz);
    }

    inline friend bool operator>(const bit_iterator& x, const bit_iterator& y) {
        return y < x;
    }

    inline friend bool operator<=(const bit_iterator& x, const bit_iterator& y) {
        return !(y < x);
    }

    inline friend bool operator>=(const bit_iterator& x, const bit_iterator& y) {
        return !(x < y);
    }

public:
    _Word*    __m_blk;
    unsigned  __m_ctz;
};



_STDX_END // end namespace stdx


