
#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#include "bittraits.hpp"
#include "bititerator.hpp"

_STDX_BEGIN

template<class _Impl>
class crtp {
protected:
    _Impl* downcast() { return static_cast<_Impl*>(this); }
    const _Impl* downcast() const { return static_cast<const _Impl*>(this); }
};

template<class _Word, class _Impl>
class bit_codec : public crtp<_Impl>
{
public:
    typedef stdx::bit_iterator<_Word, false> bit_iterator;
    typedef stdx::bit_iterator<const _Word, true>  const_bit_iterator;
    typedef stdx::bit_traits<_Word> traits_type;
    static constexpr size_t bpw = traits_type::bpw;


    template<class _InIt, class _OutIt>
    _OutIt encode(_InIt first, _InIt last, _OutIt out) const {
        for (; first != last; ++first) {
            out = this->downcast()->pack(*first, out);
        }
        return out;
    }

    template<class _InIt, class _OutIt>
    _OutIt decode(_InIt first, _InIt last, _OutIt out) const {
        _Word w;
        for(; first != last; ++first, ++out) {
            first = this->downcast()->unpack(first, last, w);
            *out = w;
        }
        return out;
    }



    template<class _OutIt>
    _OutIt encode_bits(const_bit_iterator first, const_bit_iterator last, _OutIt out)
    {
        if (first.__m_ctz != 0) {
            throw std::logic_error("unaligned encoding is not yet implemented");
        }

        if (first == last)
            return out;

        const _Impl* pimpl = this->downcast();

        size_t __n = last - first;
        if (__n < bpw) {
            return pimpl->pack(*first.__m_blk & (traits_type::bit_mask(last.__m_ctz) - 1), out);
        }
        // do middle whole words
        for (; __n >= bpw; ++first.__m_blk, __n -= bpw) {
            out = pimpl->pack(*first.__m_blk, out);
        }
        // do last word
        if (__n > 0)
            return pimpl->pack(*first.__m_blk & (traits_type::bit_mask(__n) - 1), out);
        return out;
    }


    template<class _InIt>
    bit_iterator decode_bits(_InIt first, _InIt last, bit_iterator out) const
    {
        if (out.__m_ctz != 0) {
            throw std::logic_error("unaligned decoding is not yet implemented");
        }
        _Word w;
        for(; first != last; ++first, out += bpw) {
            first = this->downcast()->unpack(first, last, w);
            *out.__m_blk = w;
        }
        return out;
    }
};


/*!
 * \brief LEB128 codec
 *
 * LEB128 or Little Endian Base 128 is a form of
 * variable-length code compression used to store
 * an arbitrarily large integer in a small number
 * of bytes.
 *
 * See https://en.wikipedia.org/wiki/LEB128 for details
 */
template<class _Word>
class leb128_codec :
      public bit_codec<_Word, leb128_codec<_Word> >
{
    friend class bit_codec<_Word, leb128_codec<_Word> >;

    /*!
     * \brief Encodes an unsigned variable-length
     * integer using the MSB algorithm.
     * This function assumes that the value is
     * stored as little endian.
     * \param w The input value. Any standard unsigned
     * integer type is allowed.
     * \param out An output byte iterator. Must have a
     * minimum size dependent on the input size
     * (32 bit = 5 bytes, 64 bit = 10 bytes).
     * \return updated output iterator
     */
    template<class _OutIt>
    _OutIt pack(_Word w, _OutIt out) const
    {
        do {
            uint8_t byte = w & 0x7f;
            w >>= 7;
            if (w != 0) {     /* more bytes to come */
                byte |= 0x80; /* set high order bit of byte */
            }
            *out = byte; ++out;
        } while(w != 0);
        return out;
    }


    /*!
     * \brief Decodes an unsigned variable-length
     * integer using the MSB  algorithm.
     * \param first An iterator pointed to begining
     * of variable-length encoded integer of arbitrary size.
     * \param last An iterator pointed to ending of
     * variable-length  encoded integer of arbitrary size.
     * \param w result decoded integer
     * \return advanced input iterator
     */
    template<class _InIt>
    _InIt unpack(_InIt first, _InIt last, _Word& w) const
    {
        _Word result = 0;
        size_t shift = 0;
        for(; first != last; ++first) {
            uint8_t byte = *first;
            result |= (_Word(byte & 0x7f) << shift);
            shift += 7;
            if (!(byte & 0x80))
                break;
        }
        w = result;
        return first;
    }
};



template<class _Word>
class sqlite_codec :
      public bit_codec<_Word, leb128_codec<_Word> >
{
    friend class bit_codec<_Word, leb128_codec<_Word> >;

    static const unsigned cut1 = 185;
    static const unsigned cut2 = 249;

    template<class W, class _InIt>
    struct unaligned_load;

    template<class _InIt>
    struct unaligned_load<uint8_t, _InIt> {
        inline uint8_t operator()(_InIt& first) const {
            uint8_t result = *first;
            ++first;
            return result;
        }
    };

    template<class _InIt>
    struct unaligned_load<uint16_t, _InIt> {
        inline uint16_t operator()(_InIt& first) const {
            uint8_t buf[sizeof(uint16_t)];
            buf[0] = *first; ++first;
            buf[1] = *first; ++first;
            uint16_t x;
            std::memcpy(&x, buf, sizeof(uint16_t));
            return x;
        }
    };

    template<class _InIt>
    struct unaligned_load<uint32_t, _InIt> {
        inline uint16_t operator()(_InIt& first) const {
            uint32_t buf[sizeof(uint16_t)];
            buf[0] = *first; ++first;
            buf[1] = *first; ++first;
            buf[0] = *first; ++first;
            buf[1] = *first; ++first;
            uint32_t x;
            std::memcpy(&x, buf, sizeof(uint32_t));
            return x;
        }
    };

    template<class _InIt>
    struct unaligned_load<uint64_t, _InIt> {
        inline uint64_t operator()(_InIt& first) const {
            uint8_t buf[sizeof(uint64_t)];
            buf[0] = *first; ++first;
            buf[1] = *first; ++first;
            buf[2] = *first; ++first;
            buf[3] = *first; ++first;
            buf[4] = *first; ++first;
            buf[5] = *first; ++first;
            buf[6] = *first; ++first;
            buf[7] = *first; ++first;
            buf[8] = *first; ++first;
            uint64_t x;
            std::memcpy(&x, buf, sizeof(uint64_t));
            return x;
        }
    };


    template<class _OutIt>
    _OutIt pack(_Word w, _OutIt out) const
    {
        if (w < cut1) {
            // 1 byte.
            *out = w; ++out;
        } else if (w <= cut1 + 255 + 256 * (cut2 - 1 - cut1)) {
            // 2 bytes.
            w -= cut1;
            *out = (cut1 + (w >> 8)); ++out;
            *out = (w & 0xff); ++out;
        } else {
            // 3-9 bytes.
            unsigned bits = 64 - __clz(w);
            unsigned bytes = (bits + 7) / 8;
            *out = (cut2 + (bytes - 2)); ++out;
            for (unsigned n = 0; n < bytes; n++) {
                *out = (w & 0xff); ++out;
                w >>= 8;
            }
        }
        return out;
    }


    template<class _InIt>
    _InIt unpack(_InIt first, _InIt last, _Word& w) const
    {
        static const unaligned_load<_Word, _InIt> __uload;
        _Word result = 0;
        for(; first != last; ) {
            uint8_t b0 = *first; ++first;
            if (b0 < cut1) {
                result = b0;
                break;
            } else if (b0 < cut2) {
                uint8_t b1 = *first; ++first;
                result = cut1 + b1 + ((b0 - cut1) << 8);
                break;
            } else {
                size_t sh = b0 - cut2;
                result = __uload(first) & ((uint64_t(1) << 8 * sh << 16) - 1);
                //in += 2 + sh;
                break;
            }
        }
        w = result;
        return first;
    }



};

_STDX_END // end namespace stdx

