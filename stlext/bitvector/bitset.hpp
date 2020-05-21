
#pragma once

#include <cstdint>
#include <algorithm>

#include "bittraits.hpp"
#include "bititerator.hpp"
#include "bitalgo.hpp"

_STDX_BEGIN

template<
    size_t _Size,
    class _Word = uintptr_t
>
class bitset
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

    explicit bitset(bool b = false);
    ~bitset() = default;
    bitset(const bitset&) = default;
    bitset(bitset&&) = default;

    bitset& operator= (const bitset&) = default;
    bitset& operator= (bitset&&) = default;


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
    bitset& set(size_t pos, bool value = true);
    bitset& set();

    bitset& reset(size_t pos);
    bitset& reset();

    bitset& flip(size_t pos);
    bitset& flip();

    const_pointer data() const;

    inline bitset& operator<<=(size_t pos);
    inline bitset& operator>>=(size_t pos);

    inline bitset& operator|=(const bitset& other);
    inline bitset& operator&=(const bitset& other);
    inline bitset& operator^=(const bitset& other);

    inline bitset& operator~();

    inline bool operator==(const bitset& other);
    inline bool operator!=(const bitset& other);

    template<class _Char, class _Traits, class _Alloc>
    inline operator std::basic_string<_Char, _Traits, _Alloc> () const {
        std::basic_string<_Char, _Traits, _Alloc> result;
        return stdx::detail::__to_string(this->begin(), this->end(), result, '1', '0');
    }

private:
    inline void __sanitize_bits();
    inline void __bitwise_shl(size_t pos);
    inline void __bitwise_shr(size_t pos);
    inline void __logical_shl(uint32_t count);
    inline void __logical_shr(uint32_t count);

private:
    _Word __m_words[nwords];
};


// empty bit-set is noop
template<class _Word>
class bitset<0, _Word> {};





template<size_t _Size, class _Word>
bitset<_Size, _Word>::bitset(bool b)
{
    std::fill_n(__m_words, nwords, (b ? ~(static_cast<_Word>(0)) : static_cast<_Word>(0)));
    __sanitize_bits();
}


template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::iterator bitset<_Size, _Word>::begin()
{
    return iterator(&__m_words[0], 0);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::iterator bitset<_Size, _Word>::end()
{
    return iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_iterator bitset<_Size, _Word>::begin() const
{
    return const_iterator(&__m_words[0], 0);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_iterator bitset<_Size, _Word>::end() const
{
    return const_iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_iterator bitset<_Size, _Word>::cbegin() const
{
    return const_iterator(&__m_words[0], 0);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_iterator bitset<_Size, _Word>::cend() const
{
    return const_iterator(&__m_words[nbits / bpw], nbits % bpw);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::reverse_iterator bitset<_Size, _Word>::rbegin()
{
    return reverse_iterator(this->end());
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::reverse_iterator bitset<_Size, _Word>::rend()
{
    return reverse_iterator(this->begin());
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_reverse_iterator bitset<_Size, _Word>::rbegin() const
{
    return const_reverse_iterator(this->end());
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_reverse_iterator bitset<_Size, _Word>::rend() const
{
    return const_reverse_iterator(this->begin());
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::empty() const
{
    return false;
}

template<size_t _Size, class _Word>
size_t bitset<_Size, _Word>::size() const
{
    return nbits;
}

template<size_t _Size, class _Word>
size_t bitset<_Size, _Word>::max_size() const
{
    return nbits;
}

template<size_t _Size, class _Word>
size_t bitset<_Size, _Word>::count(bool on) const
{
    return stdx::count(begin(), end(), on);
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::all() const
{
    return (stdx::detail::__find_bool_false(begin(), _Size) == end());
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::any() const
{
    return (stdx::detail::__find_bool_true(begin(), _Size) != end());
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::none() const
{
    return (stdx::detail::__find_bool_true(begin(), _Size) == end());
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::reference
    bitset<_Size, _Word>::operator[](size_t pos)
{
    return reference(__m_words[traits_type::block_index(pos)], traits_type::bit_mask(pos));
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::operator[](size_t pos) const
{
    return test(pos);
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::at(size_t pos) const
{
    // FIXME: don't forget to uncomment me!
    // __stdx_assert(pos < nbits, std::out_of_range_error)
    return test(pos);
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::test(size_t pos) const
{
    return ((__m_words[traits_type::block_index(pos)] & traits_type::bit_mask(pos)) != static_cast<_Word>(0));
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::set(size_t pos, bool value)
{
    if (value)
        __m_words[traits_type::block_index(pos)] |= traits_type::bit_mask(pos);
    else
        __m_words[traits_type::block_index(pos)] &= ~traits_type::bit_mask(pos);
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::set()
{
    std::fill_n(__m_words, nwords, ~(_Word(0)));
    __sanitize_bits();
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::reset(size_t pos)
{
    __m_words[traits_type::block_index(pos)] &= ~traits_type::bit_mask(pos);
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::reset()
{
    std::fill_n(__m_words, nwords, _Word(0));
    __sanitize_bits();
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::flip(size_t pos)
{
    __m_words[traits_type::block_index(pos)] ^= traits_type::bit_mask(pos);
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::flip()
{
    stdx::detail::__flip_inplace(begin(), end());
    return (*this);
}

template<size_t _Size, class _Word>
typename bitset<_Size, _Word>::const_pointer bitset<_Size, _Word>::data() const
{
    return __m_words;
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::operator<<=(size_t pos)
{
    if (__builtin_expect(pos < _Size, 1)) {
        this->__bitwise_shl(pos);
        this->__sanitize_bits();
        return (*this);
    }
    return this->reset();
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::operator>>=(size_t pos)
{
    if (__builtin_expect(pos < _Size, 1))
    {
        this->__bitwise_shr(pos);
        this->__sanitize_bits();
        return (*this);
    }
    return this->reset();
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::operator|=(const bitset<_Size, _Word> &other)
{
    std::transform(__m_words, __m_words + nwords, other.__m_words, __m_words, std::bit_or<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::operator&=(const bitset<_Size, _Word> &other)
{
    std::transform(__m_words, __m_words + nwords, other.__m_words, __m_words, std::bit_and<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::operator^=(const bitset<_Size, _Word> &other)
{
    std::transform(__m_words, __m_words + nwords, other.__m_words, __m_words, std::bit_xor<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<size_t _Size, class _Word>
bitset<_Size, _Word> &bitset<_Size, _Word>::operator~()
{
    std::transform(__m_words, __m_words + nwords, __m_words, std::bit_not<_Word>{});
    __sanitize_bits();
    return (*this);
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::operator==(const bitset &other)
{
    return std::equal(__m_words, __m_words + nwords, other.__m_words);
}

template<size_t _Size, class _Word>
bool bitset<_Size, _Word>::operator!=(const bitset &other)
{
    return !((*this) == other);
}



template<size_t _Size, class _Word>
void bitset<_Size, _Word>::__sanitize_bits()
{
    if ((nbits % bpw) != 0)
        __m_words[nwords - 1] &= (static_cast<_Word>(1) << (nbits % bpw)) - 1;
}

template<size_t _Size, class _Word>
void bitset<_Size, _Word>::__bitwise_shl(size_t pos)
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

template<size_t _Size, class _Word>
void bitset<_Size, _Word>::__bitwise_shr(size_t pos)
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



template<size_t _Size, class _Word, class _Char, class _Traits>
inline std::basic_ostream<_Char, _Traits>& operator<<(std::basic_ostream<_Char, _Traits>& stream,
                                                      const bitset<_Size, _Word>& bits)
{
    return detail::__to_stream(bits.begin(), bits.end(), stream);
}

_STDX_END // end namespace stdx



namespace std {
    // DR 1182.
    /// std::hash specialization for bitset.
    template<size_t _Size, class _Word>
    struct hash< stdx::bitset<_Size, _Word> >
    {
        size_t operator()(const stdx::bitset<_Size, _Word>& bits) const noexcept
        {
            static const std::size_t __S_seed = static_cast<size_t>(0xc70f6907UL);
            const size_t __clength = (_Size + CHAR_BIT - 1) / CHAR_BIT;
            return std::_Hash_bytes(bits.data(), __clength, __S_seed);
        }
    };

    template<class _Word>
    struct hash< stdx::bitset<0, _Word> >
    {
        size_t operator()(const stdx::bitset<0>&) const noexcept {
            return 0;
        }
    };
}

