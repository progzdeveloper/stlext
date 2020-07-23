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

#include <cstdint>
#include <algorithm>

#include "bittraits.hpp"
#include "bititerator.hpp"
#include "bitalgo.hpp"

_STDX_BEGIN

// TODO: can we provide modifiying
// methods (such as xor, or, not and
// most important left/right shift
// that can harm the source of bitview???

template<
    class _Word,
    size_t _Size
>
class bitview
{
public:
    static_assert(std::is_integral<_Word>::value && std::is_unsigned<_Word>::value,
                  "_Word type must be an unsigned interal type");

    static constexpr size_t nbits  = _Size;
    static constexpr size_t bpw    = (CHAR_BIT * sizeof(_Word));
    static constexpr size_t nwords = (nbits + bpw-1) / bpw;
    //static constexpr size_t nwords = ((nbits) / bpw + ((nbits) % bpw == 0 ? 0 : 1));

    typedef _Word word_type;
    typedef bool value_type;

    typedef stdx::bit_traits<_Word> traits_type;

    typedef stdx::bit_iterator<_Word, false> iterator;
    typedef stdx::bit_iterator<_Word, true>  const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef stdx::bit_reference<_Word> reference;
    typedef stdx::bit_reference<const _Word> const_reference;

    typedef _Word* pointer;
    typedef const _Word* const_pointer;

    bitview(const_pointer wp = nullptr);
    ~bitview() = default;
    bitview(const bitview&) = default;
    bitview(bitview&&) = default;

    bitview& operator= (const bitview&) = default;
    bitview& operator= (bitview&&) = default;


    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();

    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    bool empty() const;
    size_t size() const;
    size_t max_size() const;
    size_t count(bool on = true) const;

    bool all() const;
    bool any() const;
    bool none() const;

    reference operator[](size_t pos);
    bool operator[](size_t pos) const;

    bool at(size_t pos) const;
    bool test(size_t pos) const;
    bitview& set(size_t pos, bool value = true);
    bitview& set();

    bitview& reset(size_t pos);
    bitview& reset();

    bitview& flip(size_t pos);
    bitview& flip();

    const_pointer data() const;

    bitview& operator<<=(size_t pos);
    bitview& operator>>=(size_t pos);

    bitview& operator|=(const bitview& other);
    bitview& operator&=(const bitview& other);
    bitview& operator^=(const bitview& other);

    bitview& operator~();

    bool operator==(const bitview& other);
    bool operator!=(const bitview& other);

    template<class _Char, class _Traits, class _Alloc>
    inline operator std::basic_string<_Char, _Traits, _Alloc> () const {
        std::basic_string<_Char, _Traits, _Alloc> result;
        return stdx::detail::__to_string(this->begin(), this->end(), result, _Char('1'), _Char('0'));
    }

private:
    inline void __sanitize_bits();
    inline void __bitwise_shl(size_t pos);
    inline void __bitwise_shr(size_t pos);

private:
    _Word* __m_words;
    // TODO: consider the more complex internal representation
    // for X64 targets, where we can pack the ctz of first word
    // into the higher 6 bits of pointer (using tagged pointers).
    // This can provide user with interface where one can use
    // non-aligned bitview's (where ctz is not 0).
};


// empty bit-view is noop
template<class _Word>
class bitview<_Word, 0> {};





template<class _Word, size_t _Size>
bitview<_Word, _Size>::bitview(bitview<_Word, _Size>::const_pointer wp) :
    __m_words(const_cast<pointer>(wp))
{
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::iterator bitview<_Word, _Size>::begin()
{
    return iterator(&__m_words[0], 0);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::iterator bitview<_Word, _Size>::end()
{
    return iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_iterator bitview<_Word, _Size>::begin() const
{
    return const_iterator(&__m_words[0], 0);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_iterator bitview<_Word, _Size>::end() const
{
    return const_iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_iterator bitview<_Word, _Size>::cbegin() const
{
    return const_iterator(&__m_words[0], 0);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_iterator bitview<_Word, _Size>::cend() const
{
    return const_iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::reverse_iterator bitview<_Word, _Size>::rbegin()
{
    return reverse_iterator(&__m_words[0], 0);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::reverse_iterator bitview<_Word, _Size>::rend()
{
    return reverse_iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_reverse_iterator bitview<_Word, _Size>::rbegin() const
{
    return const_reverse_iterator(&__m_words[0], 0);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_reverse_iterator bitview<_Word, _Size>::rend() const
{
    return const_reverse_iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::empty() const
{
    return false;
}

template<class _Word, size_t _Size>
size_t bitview<_Word, _Size>::size() const
{
    return nbits;
}

template<class _Word, size_t _Size>
size_t bitview<_Word, _Size>::max_size() const
{
    return nbits;
}

template<class _Word, size_t _Size>
size_t bitview<_Word, _Size>::count(bool on) const
{
    return stdx::count(begin(), end(), on);
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::all() const
{
    return (stdx::detail::__find_bool_false(begin(), _Size) == end());
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::any() const
{
    return (stdx::detail::__find_bool_true(begin(), _Size) != end());
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::none() const
{
    return (stdx::detail::__find_bool_true(begin(), _Size) == end());
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::reference
    bitview<_Word, _Size>::operator[](size_t pos)
{
    return reference(__m_words[traits_type::block_index(pos)], traits_type::bit_mask(pos));
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::operator[](size_t pos) const
{
    return test(pos);
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::at(size_t pos) const
{
    // FIXME: don't forget to uncomment me!
    // __stdx_assert(pos < nbits, std::out_of_range_error)
    return test(pos);
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::test(size_t pos) const
{
    return ((__m_words[traits_type::block_index(pos)] & traits_type::bit_mask(pos)) != static_cast<_Word>(0));
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::set(size_t pos, bool value)
{
    if (value)
        __m_words[traits_type::block_index(pos)] |= traits_type::bit_mask(pos);
    else
        __m_words[traits_type::block_index(pos)] &= ~traits_type::bit_mask(pos);

    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::set()
{
    std::fill_n(__m_words, nwords, ~(_Word(0)));
    __sanitize_bits();
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::reset(size_t pos)
{
    __m_words[traits_type::block_index(pos)] &= ~traits_type::bit_mask(pos);
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::reset()
{
    std::fill_n(__m_words, nwords, _Word(0));
    __sanitize_bits();
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::flip(size_t pos)
{
    __m_words[traits_type::block_index(pos)] ^= traits_type::bit_mask(pos);
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::flip()
{
    stdx::detail::__flip_inplace(begin(), end());
    return (*this);
}

template<class _Word, size_t _Size>
typename bitview<_Word, _Size>::const_pointer bitview<_Word, _Size>::data() const
{
    return __m_words;
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::operator<<=(size_t pos)
{
    if (__builtin_expect(pos < _Size, 1)) {
        this->__bitwise_shl(pos);
        this->__sanitize_bits();
        return (*this);
    }
    return this->reset();
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::operator>>=(size_t pos)
{
    if (__builtin_expect(pos < _Size, 1))
    {
        this->__bitwise_shr(pos);
        this->__sanitize_bits();
        return (*this);
    }
    return this->reset();
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::operator|=(const bitview& other)
{
    std::transform(__m_words, __m_words + nwords, other.__m_words, __m_words, std::bit_or<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::operator&=(const bitview& other)
{
    std::transform(__m_words, __m_words + nwords, other.__m_words, __m_words, std::bit_and<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::operator^=(const bitview& other)
{
    std::transform(__m_words, __m_words + nwords, other.__m_words, __m_words, std::bit_xor<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<class _Word, size_t _Size>
bitview<_Word, _Size> &bitview<_Word, _Size>::operator~()
{
    std::transform(__m_words, __m_words + nwords, __m_words, std::bit_not<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::operator==(const bitview &other)
{
    return std::equal(__m_words, __m_words + nwords, other.__m_words);
}

template<class _Word, size_t _Size>
bool bitview<_Word, _Size>::operator!=(const bitview &other)
{
    return !((*this) == other);
}



template<class _Word, size_t _Size>
void bitview<_Word, _Size>::__sanitize_bits()
{
    __m_words[nwords - 1] &= ~((~static_cast<_Word>(0)) << (nbits % bpw));
}

template<class _Word, size_t _Size>
void bitview<_Word, _Size>::__bitwise_shl(size_t pos)
{
    if (__builtin_expect(pos != 0, 1))
    {
        const size_t wshift = pos / bpw;
        const size_t offset = pos % bpw;

        if (offset == 0) {
            for (size_t i = nwords - 1; i >= wshift; --i)
                __m_words[i] = __m_words[i - wshift];
        } else {
            const size_t sub_offset = (bpw  - offset);
            for (size_t i = nwords - 1; i > wshift; --i)
                __m_words[i] = ((__m_words[i - wshift] << offset) | (__m_words[i - wshift - 1] >> sub_offset));
            __m_words[wshift] = __m_words[0] << offset;
        }

        std::fill(__m_words + 0, __m_words + wshift, static_cast<_Word>(0));
    }
}

template<class _Word, size_t _Size>
void bitview<_Word, _Size>::__bitwise_shr(size_t pos)
{
    if (__builtin_expect(pos != 0, 1))
    {
        const size_t wshift = pos / bpw;
        const size_t offset = pos % bpw;
        const size_t wlimit = nwords - wshift - 1;

        if (offset == 0) {
            for (size_t i = 0; i <= wlimit; ++i)
                __m_words[i] = __m_words[i + wshift];
        } else {
            const size_t sub_offset = (bpw - offset);
            for (size_t i = 0; i < wlimit; ++i)
                __m_words[i] = ((__m_words[i + wshift] >> offset) | (__m_words[i + wshift + 1] << sub_offset));
            __m_words[wlimit] = __m_words[nwords-1] >> offset;
        }

        std::fill(__m_words + wlimit + 1, __m_words + nwords, static_cast<_Word>(0));
    }
}

template<class _Word, size_t _Size, class _Char, class _Traits>
inline std::basic_ostream<_Char, _Traits>& operator<<(std::basic_ostream<_Char, _Traits>& stream,
                                                      const bitview<_Word, _Size>& bits)
{
    return detail::__to_stream(bits.begin(), bits.end(), stream);
}

_STDX_END // end namespace stdx



namespace std {
    // DR 1182.
    /// std::hash specialization for bitset.
    template<class _Word, size_t _Size>
    struct hash< stdx::bitview<_Word, _Size> >
    {
        size_t operator()(const stdx::bitview<_Word, _Size>& bits) const noexcept
        {
            static const std::size_t __S_seed = static_cast<size_t>(0xc70f6907UL);
            const size_t __clength = (_Size + CHAR_BIT - 1) / CHAR_BIT;
            return std::_Hash_bytes(bits.data(), __clength, __S_seed);
        }
    };

    template<class _Word>
    struct hash< stdx::bitview<_Word, 0> >
    {
        size_t operator()(const stdx::bitview<_Word, 0>&) const noexcept {
            return 0;
        }
    };
}

