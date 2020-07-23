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

#include <algorithm>
#include <functional>
#include "bititerator.hpp"

#include "../platform/bits.h"

namespace stdx {

namespace detail {
    template <class W, bool C>
    bit_iterator<W, C> __find_bool_true(bit_iterator<W, C>, size_t);
    template <class W, bool C>
    bit_iterator<W, C> __find_bool_false(bit_iterator<W, C>, size_t);

    template <class W, bool C>
    typename bit_iterator<W, C>::difference_type __count_bool_true(bit_iterator<W, C>, size_t);
    template <class W, bool C>
    typename bit_iterator<W, C>::difference_type __count_bool_false(bit_iterator<W, C>, size_t);

    template <class W>
    void __fill_n_false(bit_iterator<W, false>, size_t);
    template <class W>
    void __fill_n_true(bit_iterator<W, false>, size_t);

    template <class W, class S, class R>
    void __generate_n(bit_iterator<W, false>, S, R);

    template <class W, bool C>
    bit_iterator<W, false> __copy_aligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    bit_iterator<W, false> __copy_unaligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);

    template <class W, bool C>
    bit_iterator<W, false> __copy_backward_aligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    bit_iterator<W, false> __copy_backward_unaligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);

    template <class C1, class C2>
    bit_iterator<C2, false> __swap_ranges_aligned(bit_iterator<C1, false>, bit_iterator<C1, false>, bit_iterator<C2, false>);
    template <class C1, class C2>
    bit_iterator<C2, false> __swap_ranges_unaligned(bit_iterator<C1, false>, bit_iterator<C1, false>, bit_iterator<C2, false>);


    template <class W, bool C, typename Op>
    bit_iterator<W, false> __transform_aligned(bit_iterator<W, C>, bit_iterator<W, C>,
                                               bit_iterator<W, C>, bit_iterator<W, false>, Op);
    template <class W, bool C, typename Op>
    bit_iterator<W, false> __transform_unaligned(bit_iterator<W, C>, bit_iterator<W, C>,
                                                 bit_iterator<W, C>, bit_iterator<W, false>, Op);

    template <class W>
    void __flip_inplace(bit_iterator<W, false>, bit_iterator<W, false>);

    template <class W, bool C>
    void __flip_copy_aligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);
    template <class W, bool C>
    void __flip_copy_unaligned(bit_iterator<W, C>, bit_iterator<W, C>, bit_iterator<W, false>);


    template <class W, bool C1, bool C2>
    bool __equal_aligned(bit_iterator<W, C1>, bit_iterator<W, C2>, size_t);
    template <class W, bool C1, bool C2>
    bool __equal_unaligned(bit_iterator<W, C1>, bit_iterator<W, C2>, size_t);

    template <class W>
    bit_iterator<W, false> __rotate(bit_iterator<W, false> first, bit_iterator<W, false> middle, bit_iterator<W, false> last);

    template <class W, bool C>
    void __reverse_aligned(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C>
    void __reverse_unaligned(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C>
    void __reverse_inplace(bit_iterator<W, C> first, bit_iterator<W, C> last);

    template <class W, bool C, class Comp>
    bool __next_permutation(bit_iterator<W, C>, bit_iterator<W, C>, Comp);

    template <class W, bool C, class Comp>
    bool __prev_permutation(bit_iterator<W, C>, bit_iterator<W, C>, Comp);


    template <class W, bool C, class _Ch, class _Tr, class _Al>
    std::basic_string<_Ch, _Tr, _Al>& __to_string(bit_iterator<W, C> first,
                                                  bit_iterator<W, C> last,
                                                  std::basic_string<_Ch, _Tr, _Al>& s,
                                                  _Ch on = _Ch('1'), _Ch off = _Ch('0'));

    template <class W, bool C, class _Ch, class _Tr>
    std::basic_ostream<_Ch, _Tr>& __to_stream(bit_iterator<W, C> first,
                                              bit_iterator<W, C> last,
                                              std::basic_ostream<_Ch, _Tr>& s);

} // end namespace detail




template<typename _Word, bool _IsConst, class _Tp>
inline bit_iterator<_Word, _IsConst>
    find(bit_iterator<_Word, _IsConst> first,
         bit_iterator<_Word, _IsConst> last,
         const _Tp& x)
{
    if (static_cast<bool>(!!x))
        return detail::__find_bool_true(first, static_cast<size_t>(last - first));
    return detail::__find_bool_false(first, static_cast<size_t>(last - first));
}


template <class _Word, bool _IsConst, class _Tp>
inline typename bit_iterator<_Word, _IsConst>::difference_type
    count(bit_iterator<_Word, _IsConst> first,
          bit_iterator<_Word, _IsConst> last,
          const _Tp& x)
{
    if (static_cast<bool>(!!x))
        return detail::__count_bool_true(first, static_cast<size_t>(last - first));
    return detail::__count_bool_false(first, static_cast<size_t>(last - first));
}




template <class _Word>
inline void fill_n(bit_iterator<_Word, false> first, size_t n, bool x)
{
    if (n > 0) {
        (x ? detail::__fill_n_true(first, n) :
             detail::__fill_n_false(first, n));
    }
}

template <class _Word>
inline void fill(bit_iterator<_Word, false> first,
                 bit_iterator<_Word, false> last, bool x)
{
    fill_n(first, static_cast<size_t>(last - first), x);
}



template <class _Word, class _SizeT, class _Rng>
inline void generate_n(bit_iterator<_Word, false> first, _SizeT n, _Rng rng)
{
    detail::__generate_n(first, n, rng);
}

template <class _Word, class _Rng>
inline void generate(bit_iterator<_Word, false> first,
                     bit_iterator<_Word, false> last, _Rng rng)
{
    return detail::__generate_n(first, (last - first), rng);
}


template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
    copy(bit_iterator<_Word, _IsConst> first,
         bit_iterator<_Word, _IsConst> last,
         bit_iterator<_Word, false> result)
{
    if (first.__m_ctz == result.__m_ctz)
        return detail::__copy_aligned(first, last, result);
    return detail::__copy_unaligned(first, last, result);
}

template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
    copy_n(bit_iterator<_Word, _IsConst> first, size_t n, bit_iterator<_Word, false> result)
{
    if (first.__m_ctz == result.__m_ctz)
        return detail::__copy_aligned(first, first + n, result);
    return detail::__copy_unaligned(first, first + n, result);
}


template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
    copy_backward(bit_iterator<_Word, _IsConst> first,
                  bit_iterator<_Word, _IsConst> last,
                  bit_iterator<_Word, false> result)
{
    if (last.__m_ctz == result.__m_ctz)
        return detail::__copy_backward_aligned(first, last, result);
    return detail::__copy_backward_unaligned(first, last, result);
}



template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
    move(bit_iterator<_Word, _IsConst> first,
         bit_iterator<_Word, _IsConst> last,
         bit_iterator<_Word, false> result)
{
    return copy(first, last, result);
}

template <class _Word, bool _IsConst>
inline bit_iterator<_Word, false>
    move_backward(bit_iterator<_Word, _IsConst> first,
                  bit_iterator<_Word, _IsConst> last,
                  bit_iterator<_Word, false> result)
{
    return copy_backward(first, last, result);
}



template <class __C1, class __C2>
inline bit_iterator<__C2, false>
    swap_ranges(bit_iterator<__C1, false> first1,
                bit_iterator<__C1, false> last1,
                bit_iterator<__C2, false> first2)
{
    if (first1.__m_ctz == first2.__m_ctz)
        return detail::__swap_ranges_aligned(first1, last1, first2);
    return detail::__swap_ranges_unaligned(first1, last1, first2);
}


template <class _Word, bool _IsConst, typename _BitOp>
inline bit_iterator<_Word, false>
    transform(bit_iterator<_Word, _IsConst> first1, bit_iterator<_Word, _IsConst> last1,
              bit_iterator<_Word, _IsConst> first2, bit_iterator<_Word, false> result, _BitOp op)
{
    using namespace std;
    typedef bit_iterator<_Word, _IsConst> _In;
    static constexpr size_t bpw = _In::bpw;
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
        "type of _BitOp not supported"
    );

    if (first1 == last1) { // empty range
        return result;
    }

    size_t __n = last1 - first1;
    if (__n == 1) { // simplest case: single bit
        *result = op(*first1, *first2);
        return (++result);
    }

    if (first1.__m_ctz == first2.__m_ctz &&
        first1.__m_ctz == result.__m_ctz) // all aligned
    {
        if (first1.__m_ctz == 0 && ((__n % bpw) == 0)) { // only aligned full words
            // use simple word-by-word transform from STL
            result.__m_blk = std::transform(first1.__m_blk, last1.__m_blk, first2.__m_blk, result.__m_blk, op);
            return result;
        }
        return detail::__transform_aligned(first1, last1, first2, result, op);
    }

    return detail::__transform_unaligned(first1, last1, first2, result, op);
}

// more secure version of `transform` algorithm
template <class _Word, bool _IsConst, typename _BitOp>
inline bit_iterator<_Word, false>
    transform(bit_iterator<_Word, _IsConst> first1, bit_iterator<_Word, _IsConst> last1,
              bit_iterator<_Word, _IsConst> first2, bit_iterator<_Word, _IsConst> last2,
              bit_iterator<_Word, false>    dfirst, bit_iterator<_Word, false>    dlast,
              _BitOp op)
{
    size_t __d1 = last1 - first1;
    size_t __d2 = last2 - first2;
    size_t __d3 = dlast - dfirst;
    if (__d1 != __d2 || __d1 != __d3) {
        throw std::out_of_range("one or more iterators are out of valid range");
    }
    return transform(first1, last1, first2, dfirst, op);
}



template <class _Word, bool _IsConst>
inline void flip_range(bit_iterator<_Word, _IsConst> first,
                       bit_iterator<_Word, _IsConst> last,
                       bit_iterator<_Word, false> result)
{
    if (first == result) {
        detail::__flip_inplace(first, last);
    } else {
        if (first.__m_ctz == result.__m_ctz)
            detail::__flip_copy_aligned(first, last, result);
        else
            detail::__flip_copy_unaligned(first, last, result);
    }
}



template <class _Word, bool _IsConst>
void reverse(bit_iterator<_Word, _IsConst> first,
             bit_iterator<_Word, _IsConst> last)
{
    if (first.__m_blk == last.__m_blk) {
        detail::__reverse_inplace(first, last); // Reverse when bit iterators belong to the same underlying word
    } else {
        if (first.__m_ctz == 0 && last.__m_ctz == 0)
            detail::__reverse_aligned(first, last); // Reverse when bit iterators are aligned
        else
            detail::__reverse_unaligned(first, last); // Reverse when bit iterators do not belong to the same underlying word
    }
}




template <class _Word, bool C1, bool C2>
inline bool equal(bit_iterator<_Word, C1> first1, bit_iterator<_Word, C1> last1,
                  bit_iterator<_Word, C2> first2, bit_iterator<_Word, C2> last2)
{
    typedef stdx::bit_traits<_Word> traits_type;

    if (first1 == last1) // nothing to compare
        return true;

    if (first1 == first2 && last1 == last2) // same size and same memory
        return true;

    size_t size = last1 - first1;
    if (size != static_cast<size_t>(last2 - first2)) // sizes differ
        return false;

    if (size == 1) // compare single bit
        return (traits_type::test_bit_unchecked(first1.__m_blk, first1.__m_ctz) ==
                traits_type::test_bit_unchecked(first2.__m_blk, first2.__m_ctz));

    /* fast heuristic based on popcount
    if (size < traits_type::bpw * 4 &&
            (__count_bool_true(first1, size) !=
             __count_bool_true(first2, size))) {
        return false;
    }*/

   return (first1.__m_ctz == first2.__m_ctz ? // evaluate alignment
           detail::__equal_aligned(first1, first2, size) : // aligned version
           detail::__equal_unaligned(first1, first2, size)); // unaligned version
}



template <class _Word, bool _IsConst, class _Tp>
std::pair<
    bit_iterator<_Word, _IsConst>,
    bit_iterator<_Word, _IsConst>
>
    equal_range(bit_iterator<_Word, _IsConst> first,
                bit_iterator<_Word, _IsConst> last,
                const _Tp& x)
{
    using detail::__find_bool_true;
    using detail::__find_bool_false;

    if (static_cast<bool>(!!x)) {
        first = __find_bool_true(first, last - first);
        return std::make_pair(first, first != last ? __find_bool_false(first, last - first) : last);
    } else {
        first = __find_bool_false(first, last - first);
        return std::make_pair(first, first != last ? __find_bool_true(first, last - first) : last);
    }
}



template <class _Word, bool _IsConst, class _Size, class _Tp>
bit_iterator<_Word, _IsConst>
    search_n(bit_iterator<_Word, _IsConst> first,
             bit_iterator<_Word, _IsConst> last,
             _Size n, const _Tp& x)
{
    using detail::__find_bool_false;
    using detail::__find_bool_true;

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
            auto end = __find_bool_false(first, last - first);
            if ((end - first) >= n)
                return first;
            first = __find_bool_true(end, last - end);
        }
        return last;
    } else {
        first = __find_bool_false(first, last - first);
        while(first != last) {
            auto end = __find_bool_true(first, last - first);
            if ((end - first) >= n)
                return first;
            first = __find_bool_false(end, last - end);
        }
        return last;
    }
}


template <class _Word>
inline bit_iterator<_Word, false>
    rotate(bit_iterator<_Word, false> first,
           bit_iterator<_Word, false> middle,
           bit_iterator<_Word, false> last)
{
    return detail::__rotate(first, middle, last);
}



template <class _Word, bool _IsConst, class _Compare>
inline bool next_permutation(bit_iterator<_Word, _IsConst> first,
                             bit_iterator<_Word, _IsConst> last,
                             _Compare comp)
{
    //typedef typename std::add_lvalue_reference<_Compare>::type _Comp_ref;
    return detail::__next_permutation(first, last, comp);
}

template <class _Word, bool _IsConst>
inline bool next_permutation(bit_iterator<_Word, _IsConst>  first,
                             bit_iterator<_Word, _IsConst>  last)
{
    typedef bit_iterator<_Word, _IsConst> iter_type;
    typedef typename std::iterator_traits<iter_type>::value_type value_type;
    return next_permutation(first, last, std::less<value_type>());
}





template <class _Word, bool _IsConst, class _Compare>
inline bool prev_permutation(bit_iterator<_Word, _IsConst> first,
                             bit_iterator<_Word, _IsConst> last,
                             _Compare comp)
{
    //typedef typename std::add_lvalue_reference<_Compare>::type _Comp_ref;
    return detail::__prev_permutation(first, last, comp);
}

template <class _Word, bool _IsConst>
inline bool prev_permutation(bit_iterator<_Word, _IsConst>  first,
                             bit_iterator<_Word, _IsConst>  last)
{
    typedef bit_iterator<_Word, _IsConst> iter_type;
    typedef typename std::iterator_traits<iter_type>::value_type value_type;
    return prev_permutation(first, last, std::less<value_type>());
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





template<class W, bool _What>
class bitsearch_iterator
{
    typedef stdx::bit_iterator<W, true> base_type;

public:
    typedef std::forward_iterator_tag iterator_category;

    typedef typename base_type::value_type      value_type;

    typedef typename base_type::reference       reference;
    typedef const reference                     const_reference;

    typedef typename base_type::pointer         pointer;
    typedef typename base_type::const_pointer   const_pointer;

    typedef typename base_type::difference_type difference_type;


    bitsearch_iterator(base_type last) :
        __m_it(last), __m_last(last) {
    }

    bitsearch_iterator(base_type first, base_type last) :
        __m_it(first), __m_last(last)
    {
        __m_it = stdx::find(__m_it, __m_last, _What);
    }

    bitsearch_iterator(const bitsearch_iterator& other) :
        __m_it(other.__m_it), __m_last(other.__m_last) {
    }

    bitsearch_iterator& operator=(const base_type& other) {
        __m_it = other.__m_it;
        return (*this);
    }

    bitsearch_iterator& operator=(const bitsearch_iterator& other) {
        __m_it = other.__m_it;
        __m_last = other.__m_last;
        return (*this);
    }

    base_type base() const { return __m_it; }

    bitsearch_iterator& operator++() {
        if (__m_it == __m_last)
            return (*this);
        __m_it = stdx::find(++__m_it, __m_last, _What);
        return (*this);
    }

    bitsearch_iterator operator++(int) {
        bitsearch_iterator t(*this);
        ++(*this);
        return t;
    }

    // prevent from address taking
    bitsearch_iterator* operator&() { return (this); }

    const_reference operator*() const noexcept {
        return (*__m_it);
    }

    inline operator base_type() const {
        return __m_it;
    }

    friend inline difference_type operator-(const bitsearch_iterator& x, const bitsearch_iterator& y) {
        return (x.__m_it - y.__m_it);
    }

    friend inline difference_type operator-(const bitsearch_iterator& x, const base_type& y) {
        return (x.__m_it - y);
    }

    friend inline difference_type operator-(const base_type& x, const bitsearch_iterator& y) {
        return (x - y.__m_it);
    }

    friend inline bool operator==(const bitsearch_iterator& lhs, const bitsearch_iterator& rhs) {
        return (lhs.__m_it == rhs.__m_it);
    }

    friend inline bool operator!=(const bitsearch_iterator& lhs, const bitsearch_iterator& rhs) {
        return !(lhs == rhs);
    }


    friend inline bool operator==(const bitsearch_iterator& lhs, const base_type& rhs) {
        return (lhs.__m_it == rhs);
    }

    friend inline bool operator==(const base_type& lhs, const bitsearch_iterator& rhs) {
        return (lhs == rhs.__m_it);
    }

    friend inline bool operator!=(const base_type& lhs, const bitsearch_iterator& rhs) {
        return !(lhs == rhs);
    }

    friend inline bool operator!=(const bitsearch_iterator& lhs, const base_type& rhs) {
        return !(lhs == rhs);
    }

private:
    base_type __m_it, __m_last;
};


} // end namespace stdx


#include "bitalgo.hxx"

