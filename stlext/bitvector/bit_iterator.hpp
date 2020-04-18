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
#include <cstdint>
#include <cassert>
#include <cstring>

#include <limits>
#include <memory>
#include <iterator>

#include <algorithm>
#include <functional>

#include <stdexcept>
#include <utility>
#include <type_traits>

#include "bit_traits.hpp"


_STDX_BEGIN

//internal
template<class _Word, bool _IsConst>
        class bit_iterator;

/*!
Proxy class for emulating mutable bit-reference
*/
template<class _Word>
class bit_reference
{
    template<class, bool>
    friend class bit_iterator;

    typedef _Word word_type;
public:
    /*! Ctor
        Single non-copy constructor
        */
    bit_reference(word_type& __blk, word_type __mask) :
        __m_pblock(__blk),
        __m_mask(__mask)
    {
    }

    /// copy ctor: compiler generated

    /*!
        \brief address operator overload
        \return bit iterator for this bit
        */
    inline bit_iterator<_Word, false> operator&() const {
        return bit_iterator<_Word, false>(__m_pblock, static_cast<unsigned>(__ctz(__m_mask)));
    }

    inline operator bool() const { return (__m_pblock & __m_mask) != 0; }
    inline bool operator~() const { return (__m_pblock & __m_mask) == 0; }


    /*!\brief flip bit */
    inline bit_reference& flip() { do_flip(); return *this; }
    /*!\brief assign bit value */
    inline bit_reference& operator=(bool x)                       /*noexcept*/ { do_assign(x);   return *this; }
    /*!\brief assign bit reference */
    inline bit_reference& operator=(const bit_reference& other) /*noexcept*/ { do_assign(other); return *this; }
    /*!\brief check for equality */
    inline bool operator==(const bit_reference& x) const { return bool(*this) == bool(x); }
    /*!\brief check for less */
    inline bool operator<(const bit_reference& x) const { return !bool(*this) && bool(x); }
    /*!\brief bitwise OR operator overload */
    inline bit_reference& operator|=(bool x) { if (x) do_set();   return *this; }
    /*!\brief bitwise AND operator overload */
    inline bit_reference& operator&=(bool x) { if (!x) do_reset(); return *this; }
    /*!\brief bitwise XOR operator overload */
    inline bit_reference& operator^=(bool x) { if (x) do_flip();  return *this; }
    /*!\brief bitwise AND-NOT operator overload */
    inline bit_reference& operator-=(bool x) { if (x) do_reset(); return *this; }

private:
    word_type&      __m_pblock;
    const word_type __m_mask;

    inline void do_set() { __m_pblock |= __m_mask; }
    inline void do_reset() { __m_pblock &= ~__m_mask; }
    inline void do_flip() { __m_pblock ^= __m_mask; }
    inline void do_assign(bool x) { (x ? do_set() : do_reset()); }
};

/*!
Proxy class for emulating non-mutable (constant) bit-reference
*/
template<class _Word>
class bit_const_reference
{
    template<class, bool>
    friend class bit_iterator;

    typedef _Word word_type;

public:
    /*! \brief constructor
        single non-copy constructor
        */
    bit_const_reference(word_type& blk, word_type mask) :
        __m_pblock(blk),
        __m_mask(mask)
    {
    }

    /*!
        \brief address operator overload
        \return bit iterator for this bit
        */
    bit_iterator<_Word, true> operator&() const {
        return bit_iterator<_Word, true>(&__m_pblock, static_cast<unsigned>(__ctz(__m_mask)));
    }


    /*!
        \brief Check if bit set
        \return true if bit is set, otherwise false
        */
    inline operator bool() const { return (__m_pblock & __m_mask) != 0; }

    /*!\brief check for equality */
    inline bool operator==(const bit_const_reference& x) const { return bool(*this) == bool(x); }
    /*!\brief check for less */
    inline bool operator<(const bit_const_reference& x) const { return !bool(*this) && bool(x); }

private:
    word_type& __m_pblock;
    const word_type __m_mask;
};



/*!
\brief class __bit_iterator provides bit iterator
\tparam _Word type of machine word
\tparam _IsConst mark const or non const versions of iterator

class __bit_iterator models random access bit iterator for
implementing STL compatible bit optimized algorithms.
*/
template<class _Word, bool _IsConst>
class bit_iterator
{
private:
    template<class>
    friend class bit_reference;
    template<class>
    friend class bit_const_reference;

    template <class, size_t>
    friend struct __bit_array;

    friend class bit_iterator<_Word, false>;
    friend class bit_iterator<_Word, true>;
public:
    typedef std::ptrdiff_t   difference_type;
    typedef bool             value_type;

    typedef typename std::conditional<_IsConst,
    bit_const_reference<_Word>,
    bit_reference<_Word>
    >::type   reference;

    typedef typename std::conditional<_IsConst,
    const _Word*, _Word*
    >::type   pointer;

    typedef std::random_access_iterator_tag   iterator_category;

    static const unsigned bpw = CHAR_BIT * sizeof(_Word);

    inline bit_iterator() :
        __m_blk(nullptr), __m_ctz(0) {
    }

    inline bit_iterator(pointer blk, unsigned ctz)
        : __m_blk(const_cast<_Word*>(blk)), __m_ctz(ctz) {
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
    pointer base() { return __m_blk; }


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

    // friend algorithms

    template <class W, bool C>
    friend bit_iterator<W, C> __find_bool_true(bit_iterator<W, C>, size_t);
    template <class W, bool C>
    friend bit_iterator<W, C> __find_bool_false(bit_iterator<W, C>, size_t);

    template <class W, bool C>
    friend typename bit_iterator<W, C>::difference_type __count_bool_true(bit_iterator<W, C>, size_t);
    template <class W, bool C>
    friend typename bit_iterator<W, C>::difference_type __count_bool_false(bit_iterator<W, C>, size_t);

    template <class W>
    friend void __fill_n_false(bit_iterator<W, false>, size_t);
    template <class W>
    friend void __fill_n_true(bit_iterator<W, false>, size_t);

    template <class W, bool C>
    friend bit_iterator<W, false> __copy_aligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    friend bit_iterator<W, false> __copy_unaligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    friend bit_iterator<W, false> copy(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    friend bit_iterator<W, false> copy_n(bit_iterator<W, C>, size_t, bit_iterator<W, false>);

    template <class W, bool C>
    friend bit_iterator<W, false> __copy_backward_aligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    friend bit_iterator<W, false> __copy_backward_unaligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    friend bit_iterator<W, false> copy_backward(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);

    template <class C1, class C2>
    friend bit_iterator<C2, false> __swap_ranges_aligned(bit_iterator<C1, false>, bit_iterator<C1, false>, bit_iterator<C2, false>);
    template <class C1, class C2>
    friend bit_iterator<C2, false> __swap_ranges_unaligned(bit_iterator<C1, false>, bit_iterator<C1, false>, bit_iterator<C2, false>);
    template <class C1, class C2>
    friend bit_iterator<C2, false> swap_ranges(bit_iterator<C1, false>, bit_iterator<C1, false>, bit_iterator<C2, false>);

    template <class W>
    friend bit_iterator<W, false> rotate(bit_iterator<W, false>, bit_iterator<W, false>, bit_iterator<W, false>);

    template <class W, bool C, typename Op>
    friend bit_iterator<W, false> __transform_aligned(bit_iterator<W, C>, bit_iterator<W, C>,
                                                      bit_iterator<W, C>, bit_iterator<W, false>, Op);
    template <class W, bool C, typename Op>
    friend bit_iterator<W, false> transform(bit_iterator<W, C>, bit_iterator<W, C>,
                                            bit_iterator<W, C>, bit_iterator<W, false>, Op);

    template <class W>
    friend void __flip_inplace(bit_iterator<W, false>, bit_iterator<W, false>);

    template <class W, bool C>
    friend void __flip_copy_aligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    friend void __flip_copy_unaligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);

    template <class W, bool C>
    friend void flip(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);


    template <class W, bool C1, bool C2>
    friend bool __equal_aligned(bit_iterator<W, C1>, bit_iterator<W, C2>, size_t);
    template <class W, bool C1, bool C2>
    friend bool __equal_unaligned(bit_iterator<W, C1>, bit_iterator<W, C2>, size_t);

    template <class W, bool C1, bool C2>
    friend bool equal(bit_iterator<W, C1>, bit_iterator<W, C1>,
                      bit_iterator<W, C2>, bit_iterator<W, C2>);

    template <class W, bool C1, bool C2>
    friend bit_iterator<W, C1> search(bit_iterator<W, C1>, bit_iterator<W, C1>,
                                      bit_iterator<W, C2>, bit_iterator<W, C2>);


    template <class W, bool C>
    friend void __reverse_aligned(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C>
    friend void __reverse_unaligned(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C>
    friend void __reverse_inplace(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C>
    friend void reverse(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C, class Comp>
    friend bool __next_permutation(bit_iterator<W, C>, bit_iterator<W, C>, Comp);

private:
    _Word*    __m_blk;
    unsigned  __m_ctz;
};


/// find

template <typename _Word, bool _IsConst>
bit_iterator<_Word, _IsConst> __find_bool_true(bit_iterator<_Word, _IsConst> __first, size_t __n)
{
    typedef _Word word_type;
    typedef bit_iterator<word_type, _IsConst> _It;

    static const unsigned bpw = _It::bpw;

    // do first partial word
    if (__first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - __first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        word_type __b = *(__first.__m_blk) & __m;
        if (__b)
            return _It(__first.__m_blk, static_cast<unsigned>(__ctz(__b)));
        if (__n == __dn)
            return __first + __n;
        __n -= static_cast<size_t>(__dn);
        ++__first.__m_blk;
    }
    // do middle whole words
    for (; __n >= bpw; ++__first.__m_blk, __n -= bpw)
        if (*__first.__m_blk)
            return _It(__first.__m_blk, static_cast<unsigned>(__ctz(*__first.__m_blk)));
    // do last partial word
    if (__n > 0)
    {
        word_type __m = ~word_type(0) >> (bpw - __n);
        word_type __b = *(__first.__m_blk) & __m;
        if (__b)
            return _It(__first.__m_blk, static_cast<unsigned>(__ctz(__b)));
    }
    return _It(__first.__m_blk, static_cast<unsigned>(__n));
}

template <typename _Word, bool _IsConst>
bit_iterator<_Word, _IsConst> __find_bool_false(bit_iterator<_Word, _IsConst> __first, size_t __n)
{
    typedef _Word word_type;
    typedef bit_iterator<word_type, _IsConst> _It;

    static const size_t bpw = _It::bpw;

    // do first partial word
    if (__first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - __first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        word_type __b = ~(*__first.__m_blk) & __m;
        if (__b)
            return _It(__first.__m_blk, static_cast<unsigned>(__ctz(__b)));
        if (__n == __dn)
            return __first + __n;
        __n -= static_cast<size_t>(__dn);
        ++__first.__m_blk;
    }
    // do middle whole words
    for (; __n >= bpw; ++__first.__m_blk, __n -= bpw)
    {
        word_type __b = ~(*__first.__m_blk);
        if (__b)
            return _It(__first.__m_blk, static_cast<unsigned>(__ctz(__b)));
    }
    // do last partial word
    if (__n > 0)
    {
        word_type __m = ~word_type(0) >> (bpw - __n);
        word_type __b = ~(*__first.__m_blk) & __m;
        if (__b)
            return _It(__first.__m_blk, static_cast<unsigned>(__ctz(__b)));
    }
    return _It(__first.__m_blk, static_cast<unsigned>(__n));
}

template<typename _Word, bool _IsConst, typename _Tp>
inline bit_iterator<_Word, _IsConst> find(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, const _Tp& __x)
{
    if (static_cast<bool>(!!__x))
        return __find_bool_true(__first, static_cast<size_t>(__last - __first));
    return __find_bool_false(__first, static_cast<size_t>(__last - __first));
}


/// count

template <typename _Word, bool _IsConst>
typename bit_iterator<_Word, _IsConst>::difference_type
__count_bool_true(bit_iterator<_Word, _IsConst> __first, size_t __n)
{
    typedef bit_iterator<_Word, _IsConst> _It;
    typedef _Word word_type;
    typedef typename _It::difference_type difference_type;
    static const unsigned bpw = _It::bpw;
    difference_type __r = 0;
    // do first partial word
    if (__first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - __first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        __r = __pop_count(*__first.__m_blk & __m);
        __n -= static_cast<size_t>(__dn);
        ++__first.__m_blk;
    }
    // do middle whole words
    for (; __n >= bpw; ++__first.__m_blk, __n -= bpw)
        __r += __pop_count(*__first.__m_blk);
    // do last partial word
    if (__n > 0)
    {
        word_type __m = ~word_type(0) >> (bpw - __n);
        __r += __pop_count(*__first.__m_blk & __m);
    }
    return __r;
}

template <typename _Word, bool _IsConst>
typename bit_iterator<_Word, _IsConst>::difference_type
__count_bool_false(bit_iterator<_Word, _IsConst> __first, size_t __n)
{
    typedef bit_iterator<_Word, _IsConst> _It;
    typedef _Word word_type;
    typedef typename _It::difference_type difference_type;
    static const unsigned bpw = _It::bpw;
    difference_type __r = 0;
    // do first partial word
    if (__first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - __first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        __r = __pop_count(~*__first.__m_blk & __m);
        __n -= static_cast<size_t>(__dn);
        ++__first.__m_blk;
    }
    // do middle whole words
    for (; __n >= bpw; ++__first.__m_blk, __n -= bpw)
        __r += __pop_count(~*__first.__m_blk);
    // do last partial word
    if (__n > 0)
    {
        word_type __m = ~word_type(0) >> (bpw - __n);
        __r += __pop_count(~*__first.__m_blk & __m);
    }
    return __r;
}

template <typename _Word, bool _IsConst, typename _Tp>
inline typename bit_iterator<_Word, _IsConst>::difference_type
count(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, const _Tp& __v)
{
    if (static_cast<bool>(!!__v))
        return __count_bool_true(__first, static_cast<size_t>(__last - __first));
    return __count_bool_false(__first, static_cast<size_t>(__last - __first));
}


/// fill_n

template <typename _Word>
void __fill_n_false(bit_iterator<_Word, false> __first, size_t __n)
{
    typedef bit_iterator<_Word, false> _It;
    typedef _Word word_type;
    static const unsigned bpw = _It::bpw;
    // do first partial word
    if (__first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - __first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        *__first.__m_blk &= ~__m;
        __n -= static_cast<size_t>(__dn);
        ++__first.__m_blk;
    }
    // do middle whole words
    size_t __nw = __n / bpw;
#ifdef STDX_CMPLR_GNU
    __builtin_memset(__first.__m_blk, 0, __nw * sizeof(word_type));
#else
    std::memset(__first.__m_blk, 0, __nw * sizeof(word_type));
#endif
    __n -= __nw * bpw;
    // do last partial word
    if (__n > 0)
    {
        __first.__m_blk += __nw;
        word_type __m = ~word_type(0) >> (bpw - __n);
        *__first.__m_blk &= ~__m;
    }
}

template <typename _Word>
void __fill_n_true(bit_iterator<_Word, false> __first, size_t __n)
{
    typedef bit_iterator<_Word, false> _It;
    typedef _Word word_type;
    static const unsigned bpw = _It::bpw;
    // do first partial word
    if (__first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - __first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        *__first.__m_blk |= __m;
        __n -= static_cast<size_t>(__dn);
        ++__first.__m_blk;
    }
    // do middle whole words
    size_t __nw = __n / bpw;
#ifdef STDX_CMPLR_GNU
    __builtin_memset(__first.__m_blk, -1, __nw * sizeof(word_type));
#else
    std::memset(__first.__m_blk, -1, __nw * sizeof(word_type));
#endif
    __n -= __nw * bpw;
    // do last partial word
    if (__n > 0)
    {
        __first.__m_blk += __nw;
        word_type __m = ~word_type(0) >> (bpw - __n);
        *__first.__m_blk |= __m;
    }
}

template <typename _Word>
inline void fill_n(bit_iterator<_Word, false> __first, size_t __n, bool __x) {
    if (__n > 0) {
        (__x ? __fill_n_true(__first, __n) : __fill_n_false(__first, __n));
    }
}

/// fill

template <typename _Word>
inline void fill(bit_iterator<_Word, false> __first, bit_iterator<_Word, false> __last, bool __x) {
    fill_n(__first, static_cast<size_t>(__last - __first), __x);
}


/// copy

template <typename _Word, bool _IsConst>
bit_iterator<_Word, false> __copy_aligned(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last,
                                          bit_iterator<_Word, false> __result)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__m_ctz != 0)
        {
            unsigned __clz = bpw - __first.__m_ctz;
            difference_type __dn = (std::min)(static_cast<difference_type>(__clz), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz - __dn));
            word_type __b = *__first.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b;
            __result.__m_blk += (__dn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__dn + __result.__m_ctz) % bpw);
            ++__first.__m_blk;
            // __first.__m_ctz = 0;
        }
        // __first.__m_ctz == 0;
        // do middle words
        size_t __nw = __n / bpw;
#ifdef STDX_CMPLR_GNU
        __builtin_memmove(__result.__m_blk, __first.__m_blk, __nw * sizeof(word_type));
#else
        std::memmove(__result.__m_blk, __first.__m_blk, __nw * sizeof(word_type));
#endif
        __n -= __nw * bpw;
        __result.__m_blk += __nw;
        // do last word
        if (__n > 0)
        {
            __first.__m_blk += __nw;
            word_type __m = ~word_type(0) >> (bpw - __n);
            word_type __b = *__first.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b;
            __result.__m_ctz = static_cast<unsigned>(__n);
        }
    }
    return __result;
}

template <typename _Word, bool _IsConst>
bit_iterator<_Word, false>
__copy_unaligned(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last,
                 bit_iterator<_Word, false> __result)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef  typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__m_ctz != 0)
        {
            unsigned __clz_f = bpw - __first.__m_ctz;
            difference_type __dn = (std::min)(static_cast<difference_type>(__clz_f), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
            word_type __b = *__first.__m_blk & __m;
            unsigned __clz_r = bpw - __result.__m_ctz;
            word_type __ddn = (std::min<word_type>)(__dn, __clz_r);
            __m = (~word_type(0) << __result.__m_ctz) & (~word_type(0) >> (__clz_r - __ddn));
            *__result.__m_blk &= ~__m;
            if (__result.__m_ctz > __first.__m_ctz)
                *__result.__m_blk |= __b << (__result.__m_ctz - __first.__m_ctz);
            else
                *__result.__m_blk |= __b >> (__first.__m_ctz - __result.__m_ctz);
            __result.__m_blk += (__ddn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__ddn + __result.__m_ctz) % bpw);
            __dn -= static_cast<difference_type>(__ddn);
            if (__dn > 0)
            {
                __m = ~word_type(0) >> (bpw - __dn);
                *__result.__m_blk &= ~__m;
                *__result.__m_blk |= __b >> (__first.__m_ctz + __ddn);
                __result.__m_ctz = static_cast<unsigned>(__dn);
            }
            ++__first.__m_blk;
            // __first.__m_ctz = 0;
        }
        // __first.__m_ctz == 0;
        // do middle words
        unsigned __clz_r = bpw - __result.__m_ctz;
        word_type __m = ~word_type(0) << __result.__m_ctz;
        for (; __n >= bpw; __n -= bpw, ++__first.__m_blk)
        {
            word_type __b = *__first.__m_blk;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b << __result.__m_ctz;
            ++__result.__m_blk;
            *__result.__m_blk &= __m;
            *__result.__m_blk |= __b >> __clz_r;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~word_type(0) >> (bpw - __n);
            word_type __b = *__first.__m_blk & __m;
            word_type __dn = (std::min)(__n, static_cast<difference_type>(__clz_r));
            __m = (~word_type(0) << __result.__m_ctz) & (~word_type(0) >> (__clz_r - __dn));
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b << __result.__m_ctz;
            __result.__m_blk += (__dn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__dn + __result.__m_ctz) % bpw);
            __n -= static_cast<difference_type>(__dn);
            if (__n > 0)
            {
                __m = ~word_type(0) >> (bpw - __n);
                *__result.__m_blk &= ~__m;
                *__result.__m_blk |= __b >> __dn;
                __result.__m_ctz = static_cast<unsigned>(__n);
            }
        }
    }
    return __result;
}

template <typename _Word, bool _IsConst>
inline bit_iterator<_Word, false>
copy(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    if (__first.__m_ctz == __result.__m_ctz)
        return __copy_aligned(__first, __last, __result);
    return __copy_unaligned(__first, __last, __result);
}

template <typename _Word, bool _IsConst>
inline bit_iterator<_Word, false>
copy_n(bit_iterator<_Word, _IsConst> __first, size_t __n, bit_iterator<_Word, false> __result)
{
    if (__first.__m_ctz == __result.__m_ctz)
        return __copy_aligned(__first, __first + __n, __result);
    return __copy_unaligned(__first, __first + __n, __result);
}

/// copy_backward

template <class _Word, bool _IsConst>
bit_iterator<_Word, false> __copy_backward_aligned(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last,
                                                   bit_iterator<_Word, false> __result)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__last.__m_ctz != 0)
        {
            difference_type __dn = (std::min)(static_cast<difference_type>(__last.__m_ctz), __n);
            __n -= __dn;
            unsigned __clz = bpw - __last.__m_ctz;
            word_type __m = (~word_type(0) << (__last.__m_ctz - __dn)) & (~word_type(0) >> __clz);
            word_type __b = *__last.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b;
            __result.__m_ctz = static_cast<unsigned>(((-__dn & (bpw - 1)) + __result.__m_ctz) % bpw);
            // __last.__m_ctz = 0
        }
        // __last.__m_ctz == 0 || __n == 0
        // __result.__m_ctz == 0 || __n == 0
        // do middle words
        word_type __nw = __n / bpw;
        __result.__m_blk -= __nw;
        __last.__m_blk -= __nw;
#ifdef STDX_CMPLR_GNU
        __builtin_memmove(__result.__m_blk, __last.__m_blk, static_cast<size_t>(__nw * sizeof(word_type)));
#else
        std::memmove(__result.__m_blk, __last.__m_blk, static_cast<size_t>(__nw * sizeof(word_type)));
#endif
        __n -= static_cast<difference_type>(__nw * bpw);
        // do last word
        if (__n > 0)
        {
            word_type __m = ~word_type(0) << (bpw - __n);
            word_type __b = *--__last.__m_blk & __m;
            *--__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b;
            __result.__m_ctz = static_cast<unsigned>(-__n & (bpw - 1));
        }
    }
    return __result;
}


#ifdef STDX_CMPLR_MSVC
#pragma warning(push)
#pragma warning(disable:4146)
#endif
template <class _Word, bool _IsConst>
bit_iterator<_Word, false> __copy_backward_unaligned(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last,
                                                     bit_iterator<_Word, false> __result)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__last.__m_ctz != 0)
        {
            difference_type __dn = (std::min)(static_cast<difference_type>(__last.__m_ctz), __n);
            __n -= __dn;
            unsigned __clz_l = bpw - __last.__m_ctz;
            word_type __m = (~word_type(0) << (__last.__m_ctz - __dn)) & (~word_type(0) >> __clz_l);
            word_type __b = *__last.__m_blk & __m;
            unsigned __clz_r = bpw - __result.__m_ctz;
            word_type __ddn = (std::min)(__dn, static_cast<difference_type>(__result.__m_ctz));
            if (__ddn > 0)
            {
                __m = (~word_type(0) << (__result.__m_ctz - __ddn)) & (~word_type(0) >> __clz_r);
                *__result.__m_blk &= ~__m;
                if (__result.__m_ctz > __last.__m_ctz)
                    *__result.__m_blk |= __b << (__result.__m_ctz - __last.__m_ctz);
                else
                    *__result.__m_blk |= __b >> (__last.__m_ctz - __result.__m_ctz);
                __result.__m_ctz = static_cast<unsigned>(((-__ddn & (bpw - 1)) + __result.__m_ctz) % bpw);
                __dn -= static_cast<difference_type>(__ddn);
            }
            if (__dn > 0)
            {
                // __result.__m_ctz == 0
                --__result.__m_blk;
                __result.__m_ctz = static_cast<unsigned>(-__dn & (bpw - 1));
                __m = ~word_type(0) << __result.__m_ctz;
                *__result.__m_blk &= ~__m;
                __last.__m_ctz -= static_cast<unsigned>(__dn + __ddn);
                *__result.__m_blk |= __b << (__result.__m_ctz - __last.__m_ctz);
            }
            // __last.__m_ctz = 0
        }
        // __last.__m_ctz == 0 || __n == 0
        // __result.__m_ctz != 0 || __n == 0
        // do middle words
        unsigned __clz_r = bpw - __result.__m_ctz;
        word_type __m = ~word_type(0) >> __clz_r;
        for (; __n >= bpw; __n -= bpw)
        {
            word_type __b = *--__last.__m_blk;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b >> __clz_r;
            *--__result.__m_blk &= __m;
            *__result.__m_blk |= __b << __result.__m_ctz;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~word_type(0) << (bpw - __n);
            word_type __b = *--__last.__m_blk & __m;
            __clz_r = bpw - __result.__m_ctz;
            word_type __dn = (std::min)(__n, static_cast<difference_type>(__result.__m_ctz));
            __m = (~word_type(0) << (__result.__m_ctz - __dn)) & (~word_type(0) >> __clz_r);
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b >> (bpw - __result.__m_ctz);
            __result.__m_ctz = static_cast<unsigned>(((-__dn & (bpw - 1)) +
                                                      __result.__m_ctz) % bpw);
            __n -= static_cast<difference_type>(__dn);
            if (__n > 0)
            {
                // __result.__m_ctz == 0
                --__result.__m_blk;
                __result.__m_ctz = static_cast<unsigned>(-__n & (bpw - 1));
                __m = ~word_type(0) << __result.__m_ctz;
                *__result.__m_blk &= ~__m;
                *__result.__m_blk |= __b << (__result.__m_ctz - (bpw - __n - __dn));
            }
        }
    }
    return __result;
}
#ifdef STDX_CMPLR_MSVC
#pragma warning(pop)
#endif

template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
copy_backward(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    if (__last.__m_ctz == __result.__m_ctz)
        return __copy_backward_aligned(__first, __last, __result);
    return __copy_backward_unaligned(__first, __last, __result);
}

/// move

template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
move(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    return copy(__first, __last, __result);
}

/// move_backward

template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
move_backward(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    return copy_backward(__first, __last, __result);
}



/// swap_ranges

template <class __C1, class __C2>
bit_iterator<__C2, false>
__swap_ranges_aligned(bit_iterator<__C1, false> __first, bit_iterator<__C1, false> __last,
                      bit_iterator<__C2, false> __result)
{
    typedef bit_iterator<__C1, false> _I1;
    typedef typename _I1::difference_type difference_type;
    typedef __C1 word_type;
    static const unsigned bpw = _I1::bpw;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__m_ctz != 0)
        {
            unsigned __clz = bpw - __first.__m_ctz;
            difference_type __dn = (std::min)(static_cast<difference_type>(__clz), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz - __dn));
            word_type __b1 = *__first.__m_blk & __m;
            *__first.__m_blk &= ~__m;
            word_type __b2 = *__result.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b1;
            *__first.__m_blk |= __b2;
            __result.__m_blk += (__dn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__dn + __result.__m_ctz) % bpw);
            ++__first.__m_blk;
            // __first.__m_ctz = 0;
        }
        // __first.__m_ctz == 0;
        // do middle words
        for (; __n >= bpw; __n -= bpw, ++__first.__m_blk, ++__result.__m_blk)
            std::swap(*__first.__m_blk, *__result.__m_blk);
        // do last word
        if (__n > 0)
        {
            word_type __m = ~word_type(0) >> (bpw - __n);
            word_type __b1 = *__first.__m_blk & __m;
            *__first.__m_blk &= ~__m;
            word_type __b2 = *__result.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b1;
            *__first.__m_blk |= __b2;
            __result.__m_ctz = static_cast<unsigned>(__n);
        }
    }
    return __result;
}

template <class __C1, class __C2>
bit_iterator<__C2, false>
__swap_ranges_unaligned(bit_iterator<__C1, false> __first, bit_iterator<__C1, false> __last,
                        bit_iterator<__C2, false> __result)
{
    typedef bit_iterator<__C1, false> _I1;
    typedef  typename _I1::difference_type difference_type;
    typedef __C1 word_type;
    static const unsigned bpw = _I1::bpw;
    difference_type __n = __last - __first;
    if (__n > 0)
    {
        // do first word
        if (__first.__m_ctz != 0)
        {
            unsigned __clz_f = bpw - __first.__m_ctz;
            difference_type __dn = (std::min)(static_cast<difference_type>(__clz_f), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
            word_type __b1 = *__first.__m_blk & __m;
            *__first.__m_blk &= ~__m;
            unsigned __clz_r = bpw - __result.__m_ctz;
            word_type __ddn = (std::min<word_type>)(__dn, __clz_r);
            __m = (~word_type(0) << __result.__m_ctz) & (~word_type(0) >> (__clz_r - __ddn));
            word_type __b2 = *__result.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            if (__result.__m_ctz > __first.__m_ctz)
            {
                unsigned __s = __result.__m_ctz - __first.__m_ctz;
                *__result.__m_blk |= __b1 << __s;
                *__first.__m_blk |= __b2 >> __s;
            }
            else
            {
                unsigned __s = __first.__m_ctz - __result.__m_ctz;
                *__result.__m_blk |= __b1 >> __s;
                *__first.__m_blk |= __b2 << __s;
            }
            __result.__m_blk += (__ddn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__ddn + __result.__m_ctz) % bpw);
            __dn -= static_cast<difference_type>(__ddn);
            if (__dn > 0)
            {
                __m = ~word_type(0) >> (bpw - __dn);
                __b2 = *__result.__m_blk & __m;
                *__result.__m_blk &= ~__m;
                unsigned __s = static_cast<unsigned>(__first.__m_ctz + __ddn);
                *__result.__m_blk |= __b1 >> __s;
                *__first.__m_blk |= __b2 << __s;
                __result.__m_ctz = static_cast<unsigned>(__dn);
            }
            ++__first.__m_blk;
            // __first.__m_ctz = 0;
        }
        // __first.__m_ctz == 0;
        // do middle words
        word_type __m = ~word_type(0) << __result.__m_ctz;
        unsigned __clz_r = bpw - __result.__m_ctz;
        for (; __n >= bpw; __n -= bpw, ++__first.__m_blk)
        {
            word_type __b1 = *__first.__m_blk;
            word_type __b2 = *__result.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b1 << __result.__m_ctz;
            *__first.__m_blk = __b2 >> __result.__m_ctz;
            ++__result.__m_blk;
            __b2 = *__result.__m_blk & ~__m;
            *__result.__m_blk &= __m;
            *__result.__m_blk |= __b1 >> __clz_r;
            *__first.__m_blk |= __b2 << __clz_r;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~word_type(0) >> (bpw - __n);
            word_type __b1 = *__first.__m_blk & __m;
            *__first.__m_blk &= ~__m;
            word_type __dn = (std::min<word_type>)(__n, __clz_r);
            __m = (~word_type(0) << __result.__m_ctz) & (~word_type(0) >> (__clz_r - __dn));
            word_type __b2 = *__result.__m_blk & __m;
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b1 << __result.__m_ctz;
            *__first.__m_blk |= __b2 >> __result.__m_ctz;
            __result.__m_blk += (__dn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__dn + __result.__m_ctz) % bpw);
            __n -= static_cast<difference_type>(__dn);
            if (__n > 0)
            {
                __m = ~word_type(0) >> (bpw - __n);
                __b2 = *__result.__m_blk & __m;
                *__result.__m_blk &= ~__m;
                *__result.__m_blk |= __b1 >> __dn;
                *__first.__m_blk |= __b2 << __dn;
                __result.__m_ctz = static_cast<unsigned>(__n);
            }
        }
    }
    return __result;
}

template <class __C1, class __C2>
inline bit_iterator<__C2, false>
swap_ranges(bit_iterator<__C1, false> __first1, bit_iterator<__C1, false> __last1, bit_iterator<__C2, false> __first2)
{
    if (__first1.__m_ctz == __first2.__m_ctz)
        return __swap_ranges_aligned(__first1, __last1, __first2);
    return __swap_ranges_unaligned(__first1, __last1, __first2);
}


/// rotate
/*! \internal */
template <class _Word, size_t N = 4>
struct __bit_array
{
    typedef std::ptrdiff_t difference_type;
    typedef _Word          word_type;
    typedef _Word*         word_pointer;
    typedef bit_iterator<_Word, false>   iterator;
    static const unsigned bpw = CHAR_BIT * sizeof(_Word);
    static const unsigned _Np = N;

    difference_type __size;
    word_type __word[_Np];

    inline static difference_type capacity() {
        return static_cast<difference_type>(_Np * bpw);
    }

    inline explicit __bit_array(difference_type __s) :
        __size(__s) {
    }

    inline iterator begin() {
        typedef std::pointer_traits<word_pointer> ptr_traits;
        return iterator(ptr_traits::pointer_to(__word[0]), 0);
    }

    inline iterator end() {
        typedef std::pointer_traits<word_pointer> ptr_traits;
        return iterator(ptr_traits::pointer_to(__word[0]) + __size / bpw, static_cast<unsigned>(__size % bpw));
    }
};

template <class _Word>
bit_iterator<_Word, false>
rotate(bit_iterator<_Word, false> __first, bit_iterator<_Word, false> __middle, bit_iterator<_Word, false> __last)
{
    typedef __bit_array<_Word> _Bits;
    typedef bit_iterator<_Word, false> _I1;
    typedef typename _I1::difference_type difference_type;
    difference_type __d1 = __middle - __first;
    difference_type __d2 = __last - __middle;
    _I1 __r = __first + __d2;
    while (__d1 != 0 && __d2 != 0)
    {
        if (__d1 <= __d2)
        {
            if (__d1 <= _Bits::capacity())
            {
                _Bits __b(__d1);
                copy(__first, __middle, __b.begin());
                copy(__b.begin(), __b.end(), copy(__middle, __last, __first));
                break;
            }
            else
            {
                _I1 __mp = swap_ranges(__first, __middle, __middle);
                __first = __middle;
                __middle = __mp;
                __d2 -= __d1;
            }
        }
        else
        {
            if (__d2 <= _Bits::capacity())
            {
                _Bits __b(__d2);
                copy(__middle, __last, __b.begin());
                copy_backward(__b.begin(), __b.end(), copy_backward(__first, __middle, __last));
                break;
            }
            else
            {
                _I1 __mp = __first + __d2;
                swap_ranges(__first, __mp, __middle);
                __first = __mp;
                __d1 -= __d2;
            }
        }
    }
    return __r;
}

/// transform

template <class _Word, bool _IsConst, typename _BitOp>
bit_iterator<_Word, false>
__transform_aligned(bit_iterator<_Word, _IsConst> __first1, bit_iterator<_Word, _IsConst> __last1,
                    bit_iterator<_Word, _IsConst> __first2, bit_iterator<_Word, false> __result, _BitOp op)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;
    difference_type __n = __last1 - __first1;
    if (__n > 0)
    {
        // do single partial word
        if (__first1.__m_blk == __last1.__m_blk)
        {
            unsigned lsb = __first1.__m_ctz;
            unsigned msb = __last1.__m_ctz;
            word_type mask = (uint64_t(-1) << lsb) & (uint64_t(-1) >> (bpw - msb));
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;
            *__result.__m_blk = op(x & mask, y & mask) ^ (z & ~mask);
            return (__result += msb);
        }

        // do first partial word
        if (__first1.__m_ctz != 0) {
            unsigned lsb = __first1.__m_ctz;
            word_type mask = ((uint64_t(-1) << lsb) & uint64_t(-1));
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;
            *__result.__m_blk = op(x & mask, y & mask) ^ (z & ~mask);
            ++__first1.__m_blk; __first1.__m_ctz = 0;
            ++__first2.__m_blk; __first2.__m_ctz = 0;
            ++__result.__m_blk; __result.__m_ctz = 0;
        }

        // do middle whole words
        size_t __nw = __n / bpw; // number of whole words
        word_type* __e = __first1.__m_blk + __nw;
        word_type* __x = __first1.__m_blk;
        word_type* __y = __first2.__m_blk;
        word_type* __z = __result.__m_blk;
        for (; __x != __e; ++__x, ++__y, ++__z) {
            *__z = op(*__x, *__y); // hope this will be auto-vectorized :)~
        }
        __first1.__m_blk = __x; __first1.__m_ctz = 0;
        __first2.__m_blk = __y; __first2.__m_ctz = 0;
        __result.__m_blk = __z; __result.__m_ctz = 0;

        __n -= __nw * bpw;

        // do last partial word
        if (__n > 0) {
            word_type mask = (uint64_t(-1) & uint64_t(-1) >> (bpw - __last1.__m_ctz));
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;
            *__result.__m_blk = op(x & mask, y & mask) ^ (z & ~mask);
            __result += __n;
        }
    }
    return __result;
}


template <class _Word, bool _IsConst, typename _BitOp>
inline bit_iterator<_Word, false>
transform(bit_iterator<_Word, _IsConst> __first1, bit_iterator<_Word, _IsConst> __last1,
          bit_iterator<_Word, _IsConst> __first2, bit_iterator<_Word, false> __result, _BitOp op)
{
    using namespace std;
    static_assert(
                //#ifdef STDX_CMPLR_MSVC
                is_same<bit_and<void>, _BitOp>::value ||
                is_same<bit_xor<void>, _BitOp>::value ||
                is_same<bit_or<void>, _BitOp>::value  ||
                //is_same<bit_andnot<void>, _BitOp>::value ||
                //#endif
                is_same<bit_and<_Word>, _BitOp>::value ||
                is_same<bit_xor<_Word>, _BitOp>::value ||
                is_same<bit_or<_Word>, _BitOp>::value, //||
                //is_same<bit_andnot<_Word>, _BitOp>::value,
                "type of _BitOp not supported");

    __stdx_assertx(__first1.__m_ctz == __first2.__m_ctz &&
                   __first1.__m_ctz == __result.__m_ctz, std::domain_error,
                   "unaligned transform currently not supported");
    return __transform_aligned(__first1, __last1, __first2, __result, op);
}




template <class _Word>
inline void __flip_inplace(bit_iterator<_Word, false> __first, bit_iterator<_Word, false> __last)
{
    using namespace std;
    typedef bit_iterator<_Word, false> _InIt;
    //typedef typename _InIt::difference_type difference_type;
    typedef _Word word_type;

    static constexpr unsigned bpw = _InIt::bpw;

    size_t __n = __last - __first;
    size_t __d = bpw - __first.__m_ctz;
    size_t __ddn = (__n <= __d) ? __n : __d;
    if (__n == 0)
        return;

    word_type __msb = __first.__m_ctz + __ddn - 1;
    word_type* __blk = __first.__m_blk;

    *__blk ^= (word_type(-1) << __first.__m_ctz) & (word_type(-1) >> (bpw - __msb - 1));

    __n -= __ddn;
    if (__n == 0)
        return;

    ++__blk;

    for (; ; ++__blk, __n -= bpw)
    {
        if (__n <= bpw) {
            __msb = __n - 1;
            *__blk ^= (word_type(-1) & (word_type(-1) >> (bpw - __msb - 1)));
            break;
        }
        else {
            *__blk = ~(*__blk);
        }
    }
}


template <class _Word, bool _IsConst>
inline void __flip_copy_aligned(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    using namespace std;
    typedef bit_iterator<_Word, _IsConst> _InIt;
    //typedef bit_iterator<_Word, false> _OutIt;
    //typedef typename _InIt::difference_type difference_type;
    typedef _Word word_type;

    static constexpr unsigned bpw = _InIt::bpw;

    size_t __n = __last - __first;
    size_t __d = bpw - __first.__m_ctz;
    size_t __ddn = (__n <= __d) ? __n : __d;
    if (__n == 0)
        return;

    word_type __msb = __first.__m_ctz + __ddn - 1;
    word_type* __blk = __first.__m_blk; // input pointer
    word_type* __ptr = __result.__m_blk; // output pointer

    *__ptr = (*__blk) ^ ((word_type(-1) << __first.__m_ctz) & (word_type(-1) >> (bpw - __msb - 1)));

    __n -= __ddn;
    if (__n == 0)
        return;

    ++__blk;
    ++__ptr;

    for (; ; ++__blk, ++__ptr, __n -= bpw)
    {
        if (__n <= bpw) {
            __msb = __n - 1;
            *__ptr = *__blk ^ (word_type(-1) & (word_type(-1) >> (bpw - __msb - 1)));
            break;
        }
        else {
            *__ptr = ~(*__blk);
        }
    }
}

template <class _Word, bool _IsConst>
inline void __flip_copy_unaligned(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef  typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;
    difference_type __n = __last - __first;

    if (__n > 0)
    {
        word_type __msb;
        // do first word
        if (__first.__m_ctz != 0)
        {
            unsigned __clz_f = bpw - __first.__m_ctz;
            difference_type __dn = (std::min)(static_cast<difference_type>(__clz_f), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
            word_type __b = (~(*__first.__m_blk) & __m);
            unsigned __clz_r = bpw - __result.__m_ctz;
            word_type __ddn = (std::min<word_type>)(__dn, __clz_r);
            __m = (~word_type(0) << __result.__m_ctz) & (~word_type(0) >> (__clz_r - __ddn));
            *__result.__m_blk &= ~__m;
            if (__result.__m_ctz > __first.__m_ctz)
                *__result.__m_blk |= __b << (__result.__m_ctz - __first.__m_ctz);
            else
                *__result.__m_blk |= __b >> (__first.__m_ctz - __result.__m_ctz);
            __result.__m_blk += (__ddn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__ddn + __result.__m_ctz) % bpw);
            __dn -= static_cast<difference_type>(__ddn);
            if (__dn > 0)
            {
                __m = ~word_type(0) >> (bpw - __dn);
                *__result.__m_blk &= ~__m;
                *__result.__m_blk |= __b >> (__first.__m_ctz + __ddn);
                __result.__m_ctz = static_cast<unsigned>(__dn);
            }
            ++__first.__m_blk;
            // __first.__m_ctz = 0;
        }
        // __first.__m_ctz == 0;
        // do middle words
        unsigned __clz_r = bpw - __result.__m_ctz;
        word_type __m = ~word_type(0) << __result.__m_ctz;
        for (; __n >= bpw; __n -= bpw, ++__first.__m_blk)
        {
            word_type __b = ~(*__first.__m_blk);

            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b << __result.__m_ctz;
            ++__result.__m_blk;
            *__result.__m_blk &= __m;
            *__result.__m_blk |= __b >> __clz_r;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~word_type(0) >> (bpw - __n);

            __msb = __n - 1;

            word_type __b = *__first.__m_blk & __m;
            __b ^= (word_type(-1) & (word_type(-1) >> (bpw - __msb - 1)));

            word_type __dn = (std::min)(__n, static_cast<difference_type>(__clz_r));
            __m = (~word_type(0) << __result.__m_ctz) & (~word_type(0) >> (__clz_r - __dn));
            *__result.__m_blk &= ~__m;
            *__result.__m_blk |= __b << __result.__m_ctz;
            __result.__m_blk += (__dn + __result.__m_ctz) / bpw;
            __result.__m_ctz = static_cast<unsigned>((__dn + __result.__m_ctz) % bpw);
            __n -= static_cast<difference_type>(__dn);
            if (__n > 0)
            {
                __m = ~word_type(0) >> (bpw - __n);
                *__result.__m_blk &= ~__m;
                *__result.__m_blk |= __b >> __dn;
                __result.__m_ctz = static_cast<unsigned>(__n);
            }
        }
    }
}


template <class _Word, bool _IsConst>
inline void flip(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, bit_iterator<_Word, false> __result)
{
    if (__first == __result) {
        __flip_inplace(__first, __last);
    } else {
        if (__first.__m_ctz == __result.__m_ctz)
            __flip_copy_aligned(__first, __last, __result);
        else
            __flip_copy_unaligned(__first, __last, __result);
    }
}


// equal

template <class _Word, bool _IC1, bool _IC2>
bool __equal_unaligned(bit_iterator<_Word, _IC1> __first1,
                       bit_iterator<_Word, _IC2> __first2,
                       size_t __n)
{
    typedef bit_iterator<_Word, _IC1> _It;
    typedef _Word word_type;
    static const unsigned bpw = _It::bpw;

    if (__n > 0)
    {
        // do first word
        if (__first1.__m_ctz != 0)
        {
            unsigned __clz_f = bpw - __first1.__m_ctz;
            size_t __dn = (std::min)(static_cast<size_t>(__clz_f), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first1.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
            word_type __b = *__first1.__m_blk & __m;
            unsigned __clz_r = bpw - __first2.__m_ctz;
            word_type __ddn = (std::min)((word_type)__dn, (word_type)__clz_r);
            __m = (~word_type(0) << __first2.__m_ctz) & (~word_type(0) >> (__clz_r - __ddn));
            if (__first2.__m_ctz > __first1.__m_ctz)
            {
                if ((*__first2.__m_blk & __m) != (__b << (__first2.__m_ctz - __first1.__m_ctz)))
                    return false;
            }
            else
            {
                if ((*__first2.__m_blk & __m) != (__b >> (__first1.__m_ctz - __first2.__m_ctz)))
                    return false;
            }
            __first2.__m_blk += (__ddn + __first2.__m_ctz) / bpw;
            __first2.__m_ctz = static_cast<unsigned>((__ddn + __first2.__m_ctz)  % bpw);
            __dn -= __ddn;
            if (__dn > 0)
            {
                __m = ~word_type(0) >> (bpw - __dn);
                if ((*__first2.__m_blk & __m) != (__b >> (__first1.__m_ctz + __ddn)))
                    return false;
                __first2.__m_ctz = static_cast<unsigned>(__dn);
            }
            ++__first1.__m_blk;
            // __first1.__m_ctz = 0;
        }
        // __first1.__m_ctz == 0;
        // do middle words
        unsigned __clz_r = bpw - __first2.__m_ctz;
        word_type __m = ~word_type(0) << __first2.__m_ctz;
        for (; __n >= bpw; __n -= bpw, ++__first1.__m_blk)
        {
            word_type __b = *__first1.__m_blk;
            if ((*__first2.__m_blk & __m) != (__b << __first2.__m_ctz))
                return false;
            ++__first2.__m_blk;
            if ((*__first2.__m_blk & ~__m) != (__b >> __clz_r))
                return false;
        }
        // do last word
        if (__n > 0)
        {
            __m = ~word_type(0) >> (bpw - __n);
            word_type __b = *__first1.__m_blk & __m;
            word_type __dn = (std::min)(__n, static_cast<size_t>(__clz_r));
            __m = (~word_type(0) << __first2.__m_ctz) & (~word_type(0) >> (__clz_r - __dn));
            if ((*__first2.__m_blk & __m) != (__b << __first2.__m_ctz))
                return false;
            __first2.__m_blk += (__dn + __first2.__m_ctz) / bpw;
            __first2.__m_ctz = static_cast<unsigned>((__dn + __first2.__m_ctz)  % bpw);
            __n -= __dn;
            if (__n > 0)
            {
                __m = ~word_type(0) >> (bpw - __n);
                if ((*__first2.__m_blk & __m) != (__b >> __dn))
                    return false;
            }
        }
    }
    return true;
}

template <class _Word, bool _IC1, bool _IC2>
bool __equal_aligned(bit_iterator<_Word, _IC1> __first1,
                     bit_iterator<_Word, _IC2> __first2,
                     size_t __n)
{
    typedef bit_iterator<_Word, _IC1> _It;
    typedef _Word word_type;
    static const int bpw = _It::bpw;

    if (__n > 0)
    {
        // do first word
        if (__first1.__m_ctz != 0)
        {
            unsigned __clz = bpw - __first1.__m_ctz;
            size_t __dn = (std::min)(static_cast<size_t>(__clz), __n);
            __n -= __dn;
            word_type __m = (~word_type(0) << __first1.__m_ctz) & (~word_type(0) >> (__clz - __dn));
            if ((*__first2.__m_blk & __m) != (*__first1.__m_blk & __m))
                return false;
            ++__first2.__m_blk;
            ++__first1.__m_blk;
            // __first1.__m_ctz = 0;
            // __first2.__m_ctz = 0;
        }
        // __first1.__m_ctz == 0;
        // __first2.__m_ctz == 0;
        // do middle words
        for (; __n >= bpw; __n -= bpw, ++__first1.__m_blk, ++__first2.__m_blk)
            if (*__first2.__m_blk != *__first1.__m_blk)
                return false;
        // do last word
        if (__n > 0)
        {
            word_type __m = ~word_type(0) >> (bpw - __n);
            if ((*__first2.__m_blk & __m) != (*__first1.__m_blk & __m))
                return false;
        }
    }
    return true;
}

// Determine if two bit ranges is equal
template <class _Word, bool C1, bool C2>
bool equal(bit_iterator<_Word, C1> first_begin,
           bit_iterator<_Word, C1> first_end,
           bit_iterator<_Word, C2> second_begin,
           bit_iterator<_Word, C2> second_end)
{
    typedef stdx::bit_traits<_Word> traits_type;

    if (first_begin == first_end) // nothing to compare
        return true;

    if (first_begin == second_begin &&
            first_end == second_end) // same size and same memory
        return true;

    size_t size = first_end - first_begin;
    if (size != static_cast<size_t>(second_end - second_begin)) // sizes differ
        return false;

    if (size == 1) // compare single bit
        return (traits_type::test_bit(first_begin.__m_blk, first_begin.__m_ctz) ==
                traits_type::test_bit(second_begin.__m_blk, second_begin.__m_ctz));

    /* fast heuristic based on popcount
    if (size < traits_type::bpw * 4 &&
            (__count_bool_true(first_begin, size) !=
             __count_bool_true(second_begin, size))) {
        return false;
    }*/

    return (first_begin.__m_ctz == second_begin.__m_ctz ? // evaluate alignment
                                                          __equal_aligned(first_begin, second_begin, size) : // aligned version
                                                          __equal_unaligned(first_begin, second_begin, size)); // unaligned version
}




template <class _Word, bool _IsConst>
void __reverse_unaligned(bit_iterator<_Word, _IsConst> first, bit_iterator<_Word, _IsConst> last)
{
    // Types and constants
    typedef _Word word_type;
    static const size_t bpw = bit_traits<_Word>::bpw;

    // Initialization
    size_t gap = (bpw - last.__m_ctz) * (last.__m_ctz != 0);

    const bool is_first_aligned = first.__m_ctz == 0;
    const bool is_last_aligned = last.__m_ctz == 0;

    word_type* it = first.__m_blk;

    // Save first and last element
    word_type first_value = *first.__m_blk;
    word_type last_value = *(last.__m_blk - is_last_aligned);

    // Reverse the underlying sequence
    std::reverse(first.__m_blk, last.__m_blk + !is_last_aligned);
    // Shift the underlying sequence to the left
    if (first.__m_ctz < gap) {
        it = first.__m_blk;
        gap -= first.__m_ctz;
        for (; it != last.__m_blk; ++it) {
            *it = __shld<word_type>(*it, *(it+1), gap);
        }
        *it <<= gap;
        it = first.__m_blk;
        // Shift the underlying sequence to the right
    } else if (first.__m_ctz > gap) {
        it = last.__m_blk - is_last_aligned;
        gap = first.__m_ctz - gap;
        for (; it != first.base(); --it) {
            *it = __shrd<word_type>(*it, *(it-1), gap);
        }
        *it >>= gap;
        it = first.__m_blk;
    }
    // Bitswap every element of the underlying sequence
    for (; it != (last.__m_blk + !is_last_aligned); ++it) {
        *it = __bit_swap(*it);
    }
    // Blend bits of the first element
    if (!is_first_aligned) {
        *first.__m_blk = __bit_blend<word_type>(
                    first_value,
                    *first.__m_blk,
                    first.__m_ctz,
                    bpw - first.__m_ctz
                    );
    }
    // Blend bits of the last element
    if (!is_last_aligned) {
        *last.__m_blk = __bit_blend<word_type>(
                    *last.__m_blk,
                    last_value,
                    last.__m_ctz,
                    bpw - last.__m_ctz
                    );
    }
}

template <class _Word, bool _IsConst>
void __reverse_aligned(bit_iterator<_Word, _IsConst> first, bit_iterator<_Word, _IsConst> last)
{
    // Types and constants
    typedef _Word word_type;
    word_type* it = first.__m_blk;

    // Reverse bit range
    std::reverse(first.__m_blk, last.__m_blk);
    for (; it !=  last.__m_blk; ++it) {
        *it = __bit_swap(*it);
    }
}

template <class _Word, bool _IsConst>
void __reverse_inplace(bit_iterator<_Word, _IsConst> first, bit_iterator<_Word, _IsConst> last)
{
    // Types and constants
    typedef _Word word_type;
    static const size_t bpw = bit_traits<_Word>::bpw;

    // Initialization
    word_type* it = first.__m_blk;
    size_t gap = (bpw - last.__m_ctz) * (last.__m_ctz != 0);

    // Reverse bit range
    *it = __bit_blend<word_type>(
                *it,
                __bit_swap(*it >> first.__m_ctz) >> gap,
                first.__m_ctz,
                last.__m_ctz - first.__m_ctz
                );
}


// Reverses the order of the bits in the provided range
template <class _Word, bool _IsConst>
void reverse(bit_iterator<_Word, _IsConst> first, bit_iterator<_Word, _IsConst> last)
{
    if (first.__m_blk == last.__m_blk) {
        __reverse_inplace(first, last); // Reverse when bit iterators belong to the same underlying word
    } else {
        (first.__m_ctz == 0 && last.__m_ctz == 0) ?
                    __reverse_aligned(first, last) : // Reverse when bit iterators are aligned
                    __reverse_unaligned(first, last); // Reverse when bit iterators do not belong to the same underlying word
    }
}



template <class _Word, bool _IsConst, class _Tp>
std::pair<
    bit_iterator<_Word, _IsConst>,
    bit_iterator<_Word, _IsConst>
>  equal_range(bit_iterator<_Word, _IsConst> first, bit_iterator<_Word, _IsConst> last, const _Tp& x)
{
    if (static_cast<bool>(!!x)) {
        first = __find_bool_true(first, last - first);
        return std::make_pair(first, first != last ? __find_bool_false(first, last - first) : last);
    } else {
        first = __find_bool_false(first, last - first);
        return std::make_pair(first, first != last ? __find_bool_true(first, last - first) : last);
    }
}

template <class _Word, bool _IsConst, class _Size, class _Tp>
bit_iterator<_Word, _IsConst> search_n(bit_iterator<_Word, _IsConst> first, bit_iterator<_Word, _IsConst> last, _Size n, const _Tp& x)
{
    if (n <= 0)
        return first;

    if (n == 1) {
        return (static_cast<bool>(!!x)) ?
                    __find_bool_true(first, last - first) :
                    __find_bool_false(first, last - first);
    }

    if (static_cast<bool>(!!x)) {
        first = __find_bool_true(first, last - first);
        while(first != last) {
            auto end =  __find_bool_false(first, last - first);
            if ((end - first) >= n)
                return first;
            first = __find_bool_true(end, last - end);
        }
        return last;
    } else {
        first = __find_bool_false(first, last - first);
        while(first != last) {
            auto end =  __find_bool_true(first, last - first);
            if ((end - first) >= n)
                return first;
            first = __find_bool_false(end, last - end);
        }
        return last;
    }
}


// [code-review] может лучше назвать mismatch?
// и тогда можно поднять в публичное API
/*returns the last equal word number(always 0 or more)
if two sequenses are equal returns -1*/
template <class _Word, bool C1, bool C2>
int __equal_for_search(bit_iterator<_Word, C1> first_begin,
                       bit_iterator<_Word, C2> second_begin,
                       size_t size)
{
    // [code-review] то же что и выше про bit_traits<_Word>::bpw
    size_t word_size = sizeof(_Word)*CHAR_BIT;
    int skip_word = 0;
    while (size > 0)
    {
        // [code-review] что значит магическая константа 8?
        _Word buf1[8], buf2[8];
#ifdef STDX_CMPLR_GNU
        __builtin_memset(buf1, 0, 8 * sizeof(_Word));
        __builtin_memset(buf2, 0, 8 * sizeof(_Word));
#else
        std::memset(buf1, 0, 8 * sizeof(_Word));
        std::memset(buf2, 0, 8 * sizeof(_Word));
#endif
        size_t bufsiz = (size < word_size * 8) ? size : word_size * 8;
        copy_n(first_begin, bufsiz, bit_iterator<_Word, false>(buf1, 0));
        copy_n(second_begin, bufsiz, bit_iterator<_Word, false>(buf2, 0));

        size_t n_size = (bufsiz + word_size - 1) / word_size;

        // [code-review] расставь scope guards
        for (size_t i = 0; i < n_size; i++)  //bufsiz + word_size-1
            if (buf1[i] != buf2[i]) return skip_word;
            else skip_word++;
        size -= bufsiz;
        first_begin += bufsiz;
        second_begin += bufsiz;
    }
    return -1;
}

/*this function creates table for needle, where the each index is position word in needle
and it contains number of bits? which we can skip? if  that word is the last equal */
template <class _Word>
void __generate_word_and_mask_table(_Word word_to_find, size_t n_end, _Word * w1, _Word * w2, _Word * mask1, _Word * mask2)
{
    // [code-review] то же что и выше про bit_traits<_Word>::bpw
    size_t word_size = sizeof(_Word)*CHAR_BIT;

    w1[0] = word_to_find;
    w2[0] = 0;
    mask1[0] = 1;
    mask2[0] = 0;

    for (size_t j = 1; j < n_end; j++)
        mask1[0] = (mask1[0] << 1) | 1;

    w1[0] = w1[0] & mask1[0];
    for (size_t i = 1; i < word_size; i++)
    {
        mask1[i] = mask1[i - 1] << 1;
        w1[i] = (w1[i - 1] << 1) & mask1[i];

        mask2[i] = (mask2[i - 1] << 1) | (mask1[i - 1] >> (word_size - 1));
        w2[i] = ((w2[i - 1] << 1) | (w1[i - 1] >> (word_size - 1))) & mask2[i];
    }
}

/*this function creates table for needle, where the each index is the position of the word in needle
and table cell contains number of bits, which we can skip, if that word is the last equal */
template <class _Word, bool C>
void __generate_skip_bit_tbl(bit_iterator<_Word, C> begin,
                             bit_iterator<_Word, C> end,
                             size_t* tbl)
{
    // [code-review] то же что и выше про bit_traits<_Word>::bpw
    size_t word_size = sizeof(_Word)*CHAR_BIT;
    size_t size = (size_t)(end - begin);
    size_t tbl_size = (size + word_size - 1) / word_size - 1;

    tbl[0] = word_size;

    for (size_t j = 1; j < word_size; j++)
    {
        size_t n_end = 0;
        if ((word_size) > size) n_end = word_size - size;
        if (equal(begin + j, begin + word_size - n_end, begin, begin - j + word_size - n_end)) {
            tbl[0] = j;
            break;
        }
    }

    for (int i = 1; i < tbl_size; i++)
    {
        size_t tail_size = word_size * (i + 1);
        tbl[i] = tail_size;
        for (size_t j = tbl[i - 1]; j < tail_size; j++)
        {
            size_t n_end = 0;
            if ((tail_size) > size) n_end = tail_size - size;
            if (equal(begin + j, begin + tail_size - n_end, begin, begin - j + tail_size - n_end)) {
                tbl[i] = j;
                break;
            }
        }
        /*это если строка слишком большая и однородная, по сути ограничитель для времени генерации таблицы  */
        if ((i - (tbl[i] / word_size)) > 1024)
            for (int k = i + 1; k < tbl_size; k++)
                tbl[k] = tbl[k - 1];
        /*//////////////////*/
    }
}

template <class _Word, bool C1, bool C2>
bit_iterator<_Word, C1>
search(bit_iterator<_Word, C1> stack_begin, bit_iterator<_Word, C1> stack_end,
       bit_iterator<_Word, C2> needle_begin, bit_iterator<_Word, C2> needle_end)
{

    size_t stack_size = (size_t)(stack_end - stack_begin); //size это число битов в строке
    size_t needle_size = (size_t)(needle_end - needle_begin);
    if (needle_size > stack_size) return stack_end;
    if (needle_size == 0) return stack_end;

    // [code-review] то же что и выше про bit_traits<_Word>::bpw
    size_t word_size = sizeof(_Word)*CHAR_BIT;
    _Word word_buf[2]; //в этот буфер мы скопируем сначало первое слово из needle, а затем будем копировать слова из stack
#ifdef STDX_CMPLR_GNU
    __builtin_memset(word_buf, 0, 2 * sizeof(_Word));
#else
    std::memset(word_buf, 0, 2 * sizeof(_Word));
#endif
    size_t needle_first_word_size = (needle_size < word_size) ? needle_size : word_size; //копируем первое слово из needle дла подготовки к поиску
    copy_n(needle_begin, needle_first_word_size, bit_iterator<_Word, false>(word_buf, 0));// первое слово needle это word_buf[0]
    if (needle_size == 1) {
        // в этом блоке обрабатываем случай, когда нужно найти всего один бит
        // [code-review] слишком сложно: может просто
        // return find(...);
        bit_iterator<_Word, C1> candidate = find(stack_begin, stack_end, word_buf[0]);
        return candidate;
    } // end if (needle_size == 1)


    if ((__count_bool_true(needle_begin, needle_size) == 0) | (__count_bool_true(needle_begin, needle_size) == needle_size)) {
        //в этом блоке обрабатываем случай, когда needle это последовательность только из нулей или только из единиц
        /*сейчас в word_buf[0] лежит или нолик - значит все нули, или что-то ещё - значит единицы
                допустим ищем 1 находим первую единичку, с этого места ищем первый нолик, если расстояние меньше размера иголки, то с места
                нолика снова повторяем поиск единички и тд*/
        bool needle = (bool)word_buf[0];
        bool not_needle = (!needle);
        bit_iterator<_Word, C1> candidate_begin = find(stack_begin, stack_end, needle);
        bit_iterator<_Word, C1> candidate_end;
        while ((candidate_begin + needle_size) <= stack_end) {
            candidate_end = find(candidate_begin, stack_end, not_needle);
            if ((candidate_end - candidate_begin) >= needle_size) return candidate_begin;
            candidate_begin = find(candidate_end, stack_end, needle);
        }
        return stack_end;
    } // end for "11...1" or "00...0"


    bool flag_needle_size = 0;                         //(flag_needle_size == 1) означает, что
    if (needle_size < word_size) flag_needle_size = 1; //размер needle меньше слова
    // [code-review] почему тип таблицы size_t? может имелось в виду _Word?
    size_t *skip_bit_tbl = NULL;
    if (!flag_needle_size) { //если размер needle больше слова создаём таблицу префиксов дли каждого слова, исключая последнее(в т.ч. неполное)
        skip_bit_tbl = (size_t*)calloc(((needle_size + word_size - 1) / word_size) - 1, sizeof(size_t));
        __generate_skip_bit_tbl(needle_begin, needle_end, skip_bit_tbl);
    }

    // [code-review] а почему sizeof(_Word)*CHAR_BIT? может задать через constexpr?
    _Word w1[sizeof(_Word)*CHAR_BIT], w2[sizeof(_Word)*CHAR_BIT], mask1[sizeof(_Word)*CHAR_BIT], mask2[sizeof(_Word)*CHAR_BIT];
    __generate_word_and_mask_table(word_buf[0], needle_first_word_size, w1, w2, mask1, mask2); //первое слово из needle это w1[0]
    //создоём таблицу возможного расположения в двух словах первого слова needle, а токже набор соответствующих масок

iter_beginning: // метка перехода на начало поиска
    size_t block_beginning = stack_begin.__m_ctz; // первые block_beginning в таблице смещений проверять нельзя
    copy_n(stack_begin - block_beginning, word_size, bit_iterator<_Word, false>(word_buf, 0));//копируем первое слово из stack word_buf[0]

    while ((stack_begin.__m_blk < stack_end.__m_blk) && (stack_end.__m_ctz) ||
           (stack_begin.__m_blk < (stack_end.__m_blk - 1)) && (!stack_end.__m_ctz))
    {// проверяем последовательно по паре блков, поэтому последняя пара - это предпоследний и  последний
        stack_size = (size_t)(stack_end - stack_begin);
        if (needle_size > stack_size) {
            if (!flag_needle_size) free(skip_bit_tbl);
            return stack_end;
        }//в оставшиеся биты stack не влезает needle
        copy_n((stack_begin + word_size - block_beginning), word_size, bit_iterator<_Word, false>(&word_buf[1], 0));
        //копируем второе слово stack в word_buf[1]
        for (size_t j = block_beginning; j < word_size; j++)
        {
            if (((word_buf[0] & mask1[j]) == w1[j]) && ((word_buf[1] & mask2[j]) == w2[j]))//накладываем маски, ищем первое слово
            {	//что делаем, если первое слово совпало
                if (stack_size - j < needle_size) {
                    if (!flag_needle_size) free(skip_bit_tbl);
                    return stack_end;
                }//в оставшиеся биты stack не влезает needle
                ////bit_iterator<_Word, C1> candidate(stack_begin + j);
                if (flag_needle_size) return stack_begin + j; //в этом случае мы нашли needle (потому как needle_size < word_size)
                int last_equal_word_num = 0; // первое слово совпало, сл-но, оно имеет 0 индекс
                //if (__count_bool_true(stack_begin + j + word_size, needle_size - word_size) == __count_bool_true(needle_begin + j + word_size, needle_size - word_size))
                //{
                last_equal_word_num = __equal_for_search(stack_begin + j + word_size, needle_begin + word_size, needle_size - word_size);
                if (last_equal_word_num == -1) { //если эта функция вернула -1, значит needle найдена
                    free(skip_bit_tbl);
                    return stack_begin + j;
                }

                //иначе по таблице префиксов для последнего совпавшего слова
                //смотрим максимально возможное количество битов, которые можно пропустить
                if ((skip_bit_tbl[last_equal_word_num] + j) < word_size) j += (skip_bit_tbl[last_equal_word_num] - 1);
                else {
                    stack_begin += (skip_bit_tbl[last_equal_word_num] + j - block_beginning);// если выходит за границы рассматриваемой
                    goto iter_beginning;  //пары из stack, то переходим на начало поиска, предварительно сместив начало stack
                }
            }
        }
        word_buf[0] = word_buf[1]; //если в паре ничего не нашли, подготавливаемся к рассмотрению следующей пары
        stack_begin += (word_size - block_beginning);
        if (block_beginning) block_beginning = 0; //теперь вектор stack выровнен по началу
    }
    //здесли рассматриваем случай, когда от stack остался только один блок(в т.ч. и неполный) и ранее совпадений не найдено
    stack_size = (size_t)(stack_end - stack_begin);
    if (!flag_needle_size) free(skip_bit_tbl);
    if (needle_size > stack_size) return stack_end; //в оставшиеся биты stack не влезает needle
    //здесь ситуация, когда весь stack лежит в word_buf[0], значащих битов stack_size штук, выровнено по началу;
    //а строка, которую мы ищем, в w1, длиной needle_size  (stack_size >= needle_size гарантированно);
    for (size_t i = 0; i <= stack_size - needle_size; i++) {
        if ((word_buf[0] & mask1[i]) == w1[i]) 	return (stack_begin + i);
    }
    return stack_end; //ничего не нашли =(
}








/// reference swapping
/*template<typename _Word>
void swap(stdx::detail::bit_const_reference<_Word> __x, stdx::detail::bit_const_reference<_Word> __y)
{
bool __t = __x;
__x = __y;
__y = __t;
}*/

/// reference swapping
template<class _Word>
void swap(bit_reference<_Word> __x, bit_reference<_Word> __y)
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template<class _Word>
void __swap(bit_reference<_Word> __x, bit_reference<_Word> __y)
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template<class _W1, class _W2>
inline void swap(bit_reference<_W1> __x, bit_reference<_W2> __y)
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template<class _Word>
inline void swap(bit_reference<_Word> __x, bool& __y)
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}

template<
        class _Word,
        class _Alloc
        >
inline void swap(bool& __x, bit_reference<_Word> __y)
{
    bool __t = __x;
    __x = __y;
    __y = __t;
}



// next permutation
template <class _Word, bool _IsConst, class _Compare>
bool __next_permutation(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, _Compare __comp)
{
    typedef bit_iterator<_Word, _IsConst> iter_type;

    iter_type __i = __last;
    if (__first == __last || __first == --__i)
        return false;

    // ################# EXPERIMENTAL ##########

    if ((__last - __first) <= iter_type::bpw) {
        _Word v = *__first.__m_blk;
        _Word t = v | (v - 1); // t gets v's least significant 0 bits set to 1
        // Next set to 1 the most significant bit to change,
        // set to 0 the least significant ones, and add the necessary 1 bits.
        _Word w = (t + 1) | (((~t & -~t) - 1) >> (__ctz(v) + 1));
        __first.__m_ctz = __ctz(w);
        *__first.__m_blk = w;
        return true;
    }
    // ################# EXPERIMENTAL ##########

    while (true)
    {
        iter_type __ip1 = __i;
        if (__comp(*--__i, *__ip1))
        {
            iter_type __j = __last;
            while (!__comp(*__i, *--__j))
                ;
            __swap<_Word>(*__i, *__j);
            stdx::reverse(__ip1, __last);
            return true;
        }
        if (__i == __first)
        {
            stdx::reverse(__first, __last);
            return false;
        }
    }
}

template <class _Word, bool _IsConst, class _Compare>
inline bool next_permutation(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, _Compare __comp)
{
    typedef typename std::add_lvalue_reference<_Compare>::type _Comp_ref;
    return __next_permutation<_Word, _IsConst, _Comp_ref>(__first, __last, __comp);
}

template <class _Word, bool _IsConst>
inline bool next_permutation(bit_iterator<_Word, _IsConst>  __first, bit_iterator<_Word, _IsConst>  __last)
{
    typedef bit_iterator<_Word, _IsConst> iter_type;
    typedef typename std::iterator_traits<iter_type>::value_type value_type;
    return next_permutation(__first, __last, std::less<value_type>());
}




// previous permutation
template <class _Word, bool _IsConst, class _Compare>
bool __prev_permutation(bit_iterator<_Word, _IsConst>  __first, bit_iterator<_Word, _IsConst>  __last, _Compare __comp)
{
    typedef bit_iterator<_Word, _IsConst> iter_type;
    iter_type __i = __last;
    if (__first == __last || __first == --__i)
        return false;
    while (true)
    {
        iter_type __ip1 = __i;
        if (__comp(*__ip1, *--__i))
        {
            iter_type __j = __last;
            while (!__comp(*--__j, *__i))
                ;
            swap(*__i, *__j);
            stdx::reverse(__ip1, __last);
            return true;
        }
        if (__i == __first)
        {
            stdx::reverse(__first, __last);
            return false;
        }
    }
}

template <class _Word, bool _IsConst, class _Compare>
inline bool prev_permutation(bit_iterator<_Word, _IsConst> __first, bit_iterator<_Word, _IsConst> __last, _Compare __comp)
{
    typedef typename std::add_lvalue_reference<_Compare>::type _Comp_ref;
    return __prev_permutation<_Word, _IsConst, _Comp_ref>(__first, __last, __comp);
}

template <class _Word, bool _IsConst>
inline bool prev_permutation(bit_iterator<_Word, _IsConst>  __first, bit_iterator<_Word, _IsConst>  __last)
{
    typedef bit_iterator<_Word, _IsConst> iter_type;
    typedef typename std::iterator_traits<iter_type>::value_type value_type;
    return prev_permutation(__first, __last, std::less<value_type>());
}


_STDX_END
