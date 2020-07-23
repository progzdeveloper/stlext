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

//
// WARNING: This file is internal, and not intended for direct include!
//

#include "bititerator.hpp"
#include <type_traits>

namespace stdx {


namespace detail {


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




///
/// find
///

template <class _Word, bool _IsConst>
bit_iterator<_Word, _IsConst> __find_bool_true(bit_iterator<_Word, _IsConst> first, size_t __n)
{
    typedef _Word word_type;
    typedef bit_iterator<word_type, _IsConst> _It;

    static const unsigned bpw = _It::bpw;

    // do first partial word
    if (first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        word_type __b = *(first.__m_blk) & __m;
        if (__b)
            return _It(first.__m_blk, static_cast<unsigned>(__ctz(__b)));
        if (__n == __dn)
            return first + __n;
        __n -= static_cast<size_t>(__dn);
        ++first.__m_blk;
    }
#ifndef __STDX_DISABLE_SIMD_OPTIMIZATION__
#ifdef __AVX2__ // AVX2 optimization
    static constexpr size_t __avx2_bpw = sizeof(__m256i)*CHAR_BIT;
    static constexpr size_t __avx2_nw = __avx2_bpw / bpw;
    for (; __n >= 8*__avx2_bpw; )
    {
        __m256i xmm1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 0*__avx2_nw));
        __m256i xmm2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 1*__avx2_nw));
        __m256i xmm3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 2*__avx2_nw));
        __m256i xmm4 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 3*__avx2_nw));

        __m256i xmm5 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 4*__avx2_nw));
        __m256i xmm6 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 5*__avx2_nw));
        __m256i xmm7 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 6*__avx2_nw));
        __m256i xmm8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first.__m_blk + 7*__avx2_nw));

        if (_mm256_testz_si256(xmm1, xmm1) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm2, xmm2) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm3, xmm3) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm4, xmm4) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm5, xmm5) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm6, xmm6) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm7, xmm7) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;

        if (_mm256_testz_si256(xmm8, xmm8) == 0) break;
        first.__m_blk += __avx2_nw, __n -= __avx2_bpw;
    }
#endif

#ifdef __SSE2__ // SSE2/SSE4.1 optimization
    static constexpr size_t __sse_bpw = sizeof(__m128i)*CHAR_BIT;
    static constexpr size_t __sse_nw = __sse_bpw / bpw;
    for (; __n >= 4*__sse_bpw; )
    {
        __m128i xmm1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 0*__sse_nw));
        __m128i xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 1*__sse_nw));
        __m128i xmm3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 2*__sse_nw));
        __m128i xmm4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 3*__sse_nw));

        __m128i xmm5 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 4*__sse_nw));
        __m128i xmm6 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 5*__sse_nw));
        __m128i xmm7 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 6*__sse_nw));
        __m128i xmm8 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first.__m_blk + 7*__sse_nw));
#ifdef __SSE4_1__
        if (_mm_testz_si128(xmm1, xmm1) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm2, xmm2) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm3, xmm3) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm4, xmm4) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm5, xmm5) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm6, xmm6) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm7, xmm7) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_testz_si128(xmm8, xmm8) == 0) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;
#else // no SSE4.1 but we have SSE2
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm1, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm2, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm3, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm4, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm5, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm6, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm7, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;

        if (_mm_movemask_epi8(_mm_cmpeq_epi8(xmm8, _mm_setzero_si128())) == 0xFFFF) break;
        first.__m_blk += __sse_nw, __n -= __sse_bpw;
#endif
    }
#endif
#endif

    // do middle whole words
    for (; __n >= bpw; ++first.__m_blk, __n -= bpw)
        if (*first.__m_blk)
            return _It(first.__m_blk, static_cast<unsigned>(__ctz(*first.__m_blk)));

    // do last partial word
    if (__n > 0)
    {
        word_type __m = ~word_type(0) >> (bpw - __n);
        word_type __b = *(first.__m_blk) & __m;
        if (__b)
            return _It(first.__m_blk, static_cast<unsigned>(__ctz(__b)));
    }
    return _It(first.__m_blk, static_cast<unsigned>(__n));
}





template <class _Word, bool _IsConst>
bit_iterator<_Word, _IsConst> __find_bool_false(bit_iterator<_Word, _IsConst> first, size_t __n)
{
    typedef _Word word_type;
    typedef bit_iterator<word_type, _IsConst> _It;

    static const size_t bpw = _It::bpw;

    // do first partial word
    if (first.__m_ctz != 0)
    {
        word_type __clz_f = static_cast<word_type>(bpw - first.__m_ctz);
        word_type __dn = (std::min)(__clz_f, static_cast<word_type>(__n));
        word_type __m = (~word_type(0) << first.__m_ctz) & (~word_type(0) >> (__clz_f - __dn));
        word_type __b = ~(*first.__m_blk) & __m;
        if (__b)
            return _It(first.__m_blk, static_cast<unsigned>(__ctz(__b)));
        if (__n == __dn)
            return first + __n;
        __n -= static_cast<size_t>(__dn);
        ++first.__m_blk;
    }
    // do middle whole words
    for (; __n >= bpw; ++first.__m_blk, __n -= bpw)
    {
        word_type __b = ~(*first.__m_blk);
        if (__b)
            return _It(first.__m_blk, static_cast<unsigned>(__ctz(__b)));
    }
    // do last partial word
    if (__n > 0)
    {
        word_type __m = ~word_type(0) >> (bpw - __n);
        word_type __b = ~(*first.__m_blk) & __m;
        if (__b)
            return _It(first.__m_blk, static_cast<unsigned>(__ctz(__b)));
    }
    return _It(first.__m_blk, static_cast<unsigned>(__n));
}






///
/// count
///

template <class _Word, bool _IsConst>
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
#ifndef __STDX_DISABLE_SIMD_OPTIMIZATION__
    // agressive unroll 8 times
    for (; __n >= 8*bpw; __first.__m_blk += 8, __n -= 8*bpw)
    {
        __r += __pop_count(*(__first.__m_blk + 0));
        __r += __pop_count(*(__first.__m_blk + 1));
        __r += __pop_count(*(__first.__m_blk + 2));
        __r += __pop_count(*(__first.__m_blk + 3));

        __r += __pop_count(*(__first.__m_blk + 4));
        __r += __pop_count(*(__first.__m_blk + 5));
        __r += __pop_count(*(__first.__m_blk + 6));
        __r += __pop_count(*(__first.__m_blk + 7));
    }
#endif
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

template <class _Word, bool _IsConst>
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
#ifndef __STDX_DISABLE_SIMD_OPTIMIZATION__
    // agressive unroll 8 times
    for (; __n >= 8*bpw; __first.__m_blk += 8, __n -= 8*bpw)
    {
        __r += __pop_count(~*(__first.__m_blk + 0));
        __r += __pop_count(~*(__first.__m_blk + 1));
        __r += __pop_count(~*(__first.__m_blk + 2));
        __r += __pop_count(~*(__first.__m_blk + 3));

        __r += __pop_count(~*(__first.__m_blk + 4));
        __r += __pop_count(~*(__first.__m_blk + 5));
        __r += __pop_count(~*(__first.__m_blk + 6));
        __r += __pop_count(~*(__first.__m_blk + 7));
    }
#endif
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



///
/// fill_n
///

template <class _Word>
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

template <class _Word>
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




template <class _Word, class _SizeT, class _Rng>
void __generate_n(bit_iterator<_Word, false> __first, _SizeT __n, _Rng __rng)
{
    // FIXME: static assert me!
    /*static_assert(std::is_same<_Word, typename std::result_of<_Rng>::type>::value,
                  "result type of random number generator differs from word type of iterator");*/

    typedef bit_iterator<_Word, false> _It;
    static const unsigned bpw = _It::bpw;

    if (__n > 0) {
        // do first partial word
        if (__first.__m_ctz != 0)
        {
            *__first.__m_blk |= (__rng() >> __first.__m_ctz);
            __n -= __first.__m_ctz;
            ++__first.__m_blk;
        }

        // do middle whole words
        for (; __n > 0; __n -= bpw, ++__first.__m_blk) {
            *__first.__m_blk = __rng();
        }

        // do last partial word
        if (__n > 0) {
            *__first.__m_blk = (__rng() >> (bpw - __n));
        }
    }
}




///
/// copy
///

template <class _Word, bool _IsConst>
bit_iterator<_Word, false> __copy_aligned(bit_iterator<_Word, _IsConst> __first,
                                          bit_iterator<_Word, _IsConst> __last,
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

template <class _Word, bool _IsConst>
bit_iterator<_Word, false> __copy_unaligned(bit_iterator<_Word, _IsConst> __first,
                                            bit_iterator<_Word, _IsConst> __last,
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







///
/// copy_backward
///

template <class _Word, bool _IsConst>
bit_iterator<_Word, false> __copy_backward_aligned(bit_iterator<_Word, _IsConst> __first,
                                                   bit_iterator<_Word, _IsConst> __last,
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
bit_iterator<_Word, false> __copy_backward_unaligned(bit_iterator<_Word, _IsConst> __first,
                                                     bit_iterator<_Word, _IsConst> __last,
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



///
/// swap_ranges
///

template <class __C1, class __C2>
bit_iterator<__C2, false>
__swap_ranges_aligned(bit_iterator<__C1, false> __first,
                      bit_iterator<__C1, false> __last,
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
__swap_ranges_unaligned(bit_iterator<__C1, false> __first,
                        bit_iterator<__C1, false> __last,
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






///
/// rotate
///

template <class _Word>
bit_iterator<_Word, false>
__rotate(bit_iterator<_Word, false> __first,
         bit_iterator<_Word, false> __middle,
         bit_iterator<_Word, false> __last)
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





///
/// transform
///

template <class _Word, bool _IsConst, class _BitOp>
bit_iterator<_Word, false>
__transform_aligned(bit_iterator<_Word, _IsConst> __first1,
                    bit_iterator<_Word, _IsConst> __last1,
                    bit_iterator<_Word, _IsConst> __first2,
                    bit_iterator<_Word, false>    __result,
                    _BitOp op)
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
            word_type mask = ((~(_Word(0))) << lsb) & ((~(_Word(0))) >> (bpw - msb));
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;
            *__result.__m_blk = op(x & mask, y & mask) ^ (z & ~mask);
            return (__result += msb);
        }

        // do first partial word
        if (__first1.__m_ctz != 0) {
            unsigned lsb = __first1.__m_ctz;
            word_type mask = (((~(_Word(0))) << lsb) & (~(_Word(0))));
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;
            *__result.__m_blk = op(x & mask, y & mask) ^ (z & ~mask);
            ++__first1.__m_blk; __first1.__m_ctz = 0;
            ++__first2.__m_blk; __first2.__m_ctz = 0;
            ++__result.__m_blk; __result.__m_ctz = 0;
            __n -= (bpw - lsb);
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
            word_type mask = ((~(_Word(0))) & (~(_Word(0))) >> (bpw - __last1.__m_ctz));
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;
            *__result.__m_blk = op(x & mask, y & mask) ^ (z & ~mask);
            __result += __n;
        }
    }
    return __result;
}

template <class _Word, bool _IsConst, class _BitOp>
bit_iterator<_Word, false>
__transform_unaligned(bit_iterator<_Word, _IsConst> __first1,
                    bit_iterator<_Word, _IsConst>   __last1,
                    bit_iterator<_Word, _IsConst>   __first2,
                    bit_iterator<_Word, false>      __result,
                    _BitOp op)
{
    typedef bit_iterator<_Word, _IsConst> _In;
    typedef typename _In::difference_type difference_type;
    typedef _Word word_type;
    static const unsigned bpw = _In::bpw;

    difference_type __n = __last1 - __first1;
    if (__n > 0)
    {
        unsigned lsb1 = __first1.__m_ctz;
        unsigned msb1 = __last1.__m_ctz;

        unsigned lsb2 = __first2.__m_ctz;
        unsigned msb2 = (lsb2 + __n) % bpw;

        unsigned lsbr = __result.__m_ctz;
        unsigned msbr = (lsbr + __n) % bpw;

        word_type __maskr_lo = (~_Word(0)) << lsbr;
        word_type __maskr_hi = ((~(_Word(0))) >> (bpw - msbr));

        word_type __w; // temporary whole word

        // do single word
        if ((__n + lsb1) < bpw && (__n + lsb2) < bpw && (__n + lsbr) < bpw)
        {
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;

            word_type __mask1_hi = ((~(_Word(0))) >> (bpw - msb1));
            word_type __mask2_hi = ((~(_Word(0))) >> (bpw - msb2));
            word_type __maskr = __maskr_lo ^ __maskr_hi; // combine mask for result
            x &= __mask1_hi;
            x >>= lsb1; // shift x to left
            y &= __mask2_hi;
            y >>= lsb2; // shift y to left
            __w = (op(x, y) << lsbr);
            *__result.__m_blk = __w | (z & __maskr);
            return (__result += __n);
        }

        // less than single word, but can span adjacent words
        if (__n < bpw)
        {
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;

            unsigned __n1 = (bpw - lsb1);
            unsigned __n2 = (bpw - lsb2);

            x >>= lsb1;
            if (lsb1 > 0) {
                ++__first1.__m_blk; // move to next word
                x |= (*__first1.__m_blk) << __n1;
            }

            y >>= lsb2;
            if (lsb2 > 0) {
                ++__first2.__m_blk; // move to next word
                y |= (*__first2.__m_blk) << __n2;
            }

            __w = op(x, y); // result is in lower bits of __w

            if ((__n + lsbr) < bpw) { // result located in single word
                __w &= (~(_Word(0))) >> (bpw - __n);
                *__result.__m_blk = (__w << lsbr) | (z & (__maskr_lo ^ __maskr_hi));
            } else { // result span adjacent words
                __w &= (~(_Word(0))) >> (bpw - __n);
                *__result.__m_blk = (__w << lsbr) | (z & ~__maskr_lo);
                ++__result.__m_blk;
                z = *__result.__m_blk;
                if (msbr != 0)
                    z = (z & ~__maskr_hi);
                else
                    z &= __maskr_hi;
                *__result.__m_blk = (__w >> (bpw - lsbr)) | z;
            }
            __result.__m_ctz = msbr;
            return (__result);
        }

        // FIXME: fix bugs!!!
        // do middle whole words
        for (; __n >= bpw; __n -= bpw)
        {
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;

            unsigned __n1 = (bpw - lsb1);
            unsigned __n2 = (bpw - lsb2);

            x >>= lsb1;
            ++__first1.__m_blk; // move to next word
            if (lsb1 > 0) {
                x |= (*__first1.__m_blk) << __n1; // take a part of word
            }

            y >>= lsb2;
            ++__first2.__m_blk; // move to next word
            if (lsb2 > 0) {
                y |= (*__first2.__m_blk) << __n2; // take a part of word
            }

            __w = op(x, y); // result is in lower bits of __w
            if (lsbr == 0) { // result located in single word
                *__result.__m_blk = __w;
                ++__result.__m_blk;
            } else { // result span adjacent words
                *__result.__m_blk = (__w << lsbr) | (z & ~__maskr_lo);
                ++__result.__m_blk;
                z = *__result.__m_blk;
                *__result.__m_blk = (__w >> (bpw - lsbr)) | (z & ~((~(_Word(0))) >> (bpw - lsbr)));
            }
        }

        // do last word
        if (__n > 0)
        {
            word_type x = *__first1.__m_blk;
            word_type y = *__first2.__m_blk;
            word_type z = *__result.__m_blk;

            unsigned __n1 = (bpw - lsb1);
            unsigned __n2 = (bpw - lsb2);

            x >>= lsb1;
            if (lsb1 > 0) {
                ++__first1.__m_blk; // move to next word
                x |= (*__first1.__m_blk) << __n1;
            }

            y >>= lsb2;
            if (lsb2 > 0) {
                ++__first2.__m_blk; // move to next word
                y |= (*__first2.__m_blk) << __n2;
            }

            __w = op(x, y); // result is in lower bits of __w
            __w &= (~(_Word(0))) >> (bpw - __n);

            if ((__n + lsbr) < bpw) { // result located in single word
                *__result.__m_blk = (__w << lsbr) | (z & (__maskr_lo ^ __maskr_hi));
            } else { // result span adjacent words
                *__result.__m_blk = (__w << lsbr) | (z & ~__maskr_lo);
                ++__result.__m_blk;
                if (msbr != 0)
                    z = (z & ~__maskr_hi);
                else
                    z &= __maskr_hi;
                *__result.__m_blk = (__w >> (bpw - lsbr)) | z;
            }
        }

        __result.__m_ctz = msbr;
    }

    return __result;
}

///
/// flip
///


template <class _Word>
inline void __flip_inplace(bit_iterator<_Word, false> __first,
                           bit_iterator<_Word, false> __last)
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
inline void __flip_copy_aligned(bit_iterator<_Word, _IsConst> __first,
                                bit_iterator<_Word, _IsConst> __last,
                                bit_iterator<_Word, false> __result)
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

    for (; __n > 0; ++__blk, ++__ptr, __n -= bpw) {
        *__ptr = ~(*__blk);
    }
    if (__n > 0) {
        __msb = __n - 1;
        *__ptr = *__blk ^ (word_type(-1) >> (bpw - __msb - 1));
    }
}

template <class _Word, bool _IsConst>
inline void __flip_copy_unaligned(bit_iterator<_Word, _IsConst> __first,
                                  bit_iterator<_Word, _IsConst> __last,
                                  bit_iterator<_Word, false> __result)
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






///
/// equal
///

template <class _Word, bool _IC1, bool _IC2>
bool __equal_unaligned(bit_iterator<_Word, _IC1> __first1,
                       bit_iterator<_Word, _IC2> __first2,
                       size_t __n)
{
    typedef bit_iterator<_Word, _IC1> _It;
    typedef typename std::remove_const<_Word>::type word_type;
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



///
/// reverse
///

template <class _Word, bool _IsConst>
void __reverse_unaligned(bit_iterator<_Word, _IsConst> first,
                         bit_iterator<_Word, _IsConst> last)
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
void __reverse_aligned(bit_iterator<_Word, _IsConst> first,
                       bit_iterator<_Word, _IsConst> last)
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
void __reverse_inplace(bit_iterator<_Word, _IsConst> first,
                       bit_iterator<_Word, _IsConst> last)
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





///
/// next permutation
///

template <class _Word, bool _IsConst, class _Compare>
bool __next_permutation(bit_iterator<_Word, _IsConst> __first,
                        bit_iterator<_Word, _IsConst> __last,
                        _Compare __comp)
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
            reverse(__ip1, __last);
            return true;
        }
        if (__i == __first)
        {
            reverse(__first, __last);
            return false;
        }
    }
}


///
/// previous permutation
///

template <class _Word, bool _IsConst, class _Compare>
bool __prev_permutation(bit_iterator<_Word, _IsConst>  __first,
                            bit_iterator<_Word, _IsConst>  __last,
                            _Compare __comp)
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
            reverse(__ip1, __last);
            return true;
        }
        if (__i == __first)
        {
            reverse(__first, __last);
            return false;
        }
    }
}




template <
    class _Word,
    bool _IsConst,
    class _Char,
    class _Traits,
    class _Alloc
>
std::basic_string<_Char, _Traits, _Alloc>& __to_string(bit_iterator<_Word, _IsConst> first,
                                                       bit_iterator<_Word, _IsConst> last,
                                                       std::basic_string<_Char, _Traits, _Alloc>& s,
                                                       _Char on, _Char off)
{
    //static constexpr const size_t bpw = sizeof(_Word)*CHAR_BIT;
    size_t n = (last - first);
    if (n == 0)
        return s;

    s.resize(n);
    auto out = s.rbegin();
    for (; first != last; ++first, ++out) {
        *out = (((*first.__m_blk) & (_Word(1) << first.__m_ctz)) != 0) ? on : off;
    }
    return s;
}


template <class _Word, bool _IsConst, class _Char, class _Traits>
std::basic_ostream<_Char, _Traits>& __to_stream(bit_iterator<_Word, _IsConst> first,
                                               bit_iterator<_Word, _IsConst> last,
                                               std::basic_ostream<_Char, _Traits>& stream)
{
    for (--last; last >= first; --last) {
        stream.put(_Char('0') + ((*last.__m_blk & (_Word(1) << last.__m_ctz)) != 0));
    }
    return stream;
}

/*#if (CHAR_BIT == 8)
template <class _Word, bool _IsConst, class _Traits>
std::basic_ostream<char, _Traits>& __to_stream(bit_iterator<_Word, _IsConst> first,
                                               bit_iterator<_Word, _IsConst> last,
                                               std::basic_ostream<char, _Traits>& stream)
{
    static constexpr const size_t bpw = sizeof(_Word)*CHAR_BIT;
    // pre-builded table for 8 bit words of human-readable string representation
    static constexpr const char* const bin_str[] = {
        "00000000","00000001","00000010","00000011","00000100","00000101","00000110","00000111",
        "00001000","00001001","00001010","00001011","00001100","00001101","00001110","00001111",
        "00010000","00010001","00010010","00010011","00010100","00010101","00010110","00010111",
        "00011000","00011001","00011010","00011011","00011100","00011101","00011110","00011111",
        "00100000","00100001","00100010","00100011","00100100","00100101","00100110","00100111",
        "00101000","00101001","00101010","00101011","00101100","00101101","00101110","00101111",
        "00110000","00110001","00110010","00110011","00110100","00110101","00110110","00110111",
        "00111000","00111001","00111010","00111011","00111100","00111101","00111110","00111111",
        "01000000","01000001","01000010","01000011","01000100","01000101","01000110","01000111",
        "01001000","01001001","01001010","01001011","01001100","01001101","01001110","01001111",
        "01010000","01010001","01010010","01010011","01010100","01010101","01010110","01010111",
        "01011000","01011001","01011010","01011011","01011100","01011101","01011110","01011111",
        "01100000","01100001","01100010","01100011","01100100","01100101","01100110","01100111",
        "01101000","01101001","01101010","01101011","01101100","01101101","01101110","01101111",
        "01110000","01110001","01110010","01110011","01110100","01110101","01110110","01110111",
        "01111000","01111001","01111010","01111011","01111100","01111101","01111110","01111111",
        "10000000","10000001","10000010","10000011","10000100","10000101","10000110","10000111",
        "10001000","10001001","10001010","10001011","10001100","10001101","10001110","10001111",
        "10010000","10010001","10010010","10010011","10010100","10010101","10010110","10010111",
        "10011000","10011001","10011010","10011011","10011100","10011101","10011110","10011111",
        "10100000","10100001","10100010","10100011","10100100","10100101","10100110","10100111",
        "10101000","10101001","10101010","10101011","10101100","10101101","10101110","10101111",
        "10110000","10110001","10110010","10110011","10110100","10110101","10110110","10110111",
        "10111000","10111001","10111010","10111011","10111100","10111101","10111110","10111111",
        "11000000","11000001","11000010","11000011","11000100","11000101","11000110","11000111",
        "11001000","11001001","11001010","11001011","11001100","11001101","11001110","11001111",
        "11010000","11010001","11010010","11010011","11010100","11010101","11010110","11010111",
        "11011000","11011001","11011010","11011011","11011100","11011101","11011110","11011111",
        "11100000","11100001","11100010","11100011","11100100","11100101","11100110","11100111",
        "11101000","11101001","11101010","11101011","11101100","11101101","11101110","11101111",
        "11110000","11110001","11110010","11110011","11110100","11110101","11110110","11110111",
        "11111000","11111001","11111010","11111011","11111100","11111101","11111110","11111111"
    };

    // TODO: test me carefully!
    // FIXME: fix bugs!

    if (first == last)
        return stream;

    typedef typename std::remove_const<_Word>::type word_t;

    --last;
    const word_t* ptr = last.__m_blk;
    word_t w = *ptr;

    // process the first word
    unsigned ctz0 = (first.base() == last.base() ? first.__m_ctz : 0);
    for (unsigned ctz = last.__m_ctz; ctz != ctz0; --ctz) {
        stream.put('0' + ((w & (_Word(1) << ctz)) != 0));
    }
    stream.put('0' + ((w & (_Word(1) << ctz0)) != 0));

    if (first.base() == last.base()) {
        return (stream);
    }

    --ptr;

    // process full words writing by 8 bytes at once
    for (; ptr != first.__m_blk; --ptr) {
        w = *ptr; // cache word in register for faster access
        if (w == 0) {
            // this loop can be efficently unrolled by the compiler
            for (unsigned i = 0; i < bpw; i += CHAR_BIT)
                stream.write(bin_str[0], CHAR_BIT); // TODO: consider `stream._M_write(bin_str[0], CHAR_BIT);` under GCC
        } else {
            // this loop can be efficently unrolled by the compiler
            for (unsigned i = 0; i < bpw; i += CHAR_BIT) {
                size_t pos = ((w >> (bpw - (i + CHAR_BIT))) & 0xFF);
                const char* const p = bin_str[pos];
                stream.write(p, CHAR_BIT); // TODO: consider `stream._M_write(bin_str[w & mask], CHAR_BIT);` under GCC
            }
        }
    }

    // at this point ptr and first.__m_blk is same
    w = *ptr;

    // process the last word
    for (unsigned ctz = bpw; ctz != first.__m_ctz; --ctz) {
        stream.put('0' + ((w & (_Word(1) << (ctz - 1))) != 0));
    }

    return stream;
}
#endif*/

} // end namespace detail

} // end namespace stdx




