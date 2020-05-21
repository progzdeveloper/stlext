

#pragma once

#include <climits>
#include <cstdlib>

#include "../platform/common.h"

_STDX_BEGIN


template<class _Word>
struct bit_traits
{
    /*! \brief number of bits per word */
    static constexpr size_t bpw = sizeof(_Word) * CHAR_BIT;

    /*!
     * \brief test bit in bits array at position pos
     * \param blocks pointer to word array
     * \param pos absolute bit position at position pos
     * \precond bits is non-zero valid pointer
     * \precond pos is less than equal bits in word multiplies
     * number of words pointed by bits pointer
     */
    static inline bool test_bit_unchecked(const _Word* blocks, size_t pos) {
        return ((blocks[block_index(pos)] & (_Word)bit_mask(pos)) != 0);
    }

    /*!
     * \brief set or unset bit in bits array
     * at position pos according to value of v
     * \param blocks pointer to word array
     * \param pos absolute bit position
     * \param v value (true or false)
     * \precond bits is non-zero valid pointer
     * \precond pos is less than equal bits in word times
     * number of words pointed by bits pointer
     */
    static inline void set_bit_unchecked(_Word* blocks, size_t pos, bool v)
    {
        if (v)
            blocks[block_index(pos)] |= (_Word)bit_mask(pos);
        else
            blocks[block_index(pos)] &= (_Word)~bit_mask(pos);
    }

    /*!
     * \brief clear bit in bits array at position pos
     * \param blocks pointer to word array
     * \param pos absolute bit position
     * \param v value(true or false)
     * \precond bits is non-zero valid pointer
     * \precond pos is less than equal bits in word times
     * number of words pointed by bits pointer
     */
    static inline void clear_bit_unchecked(_Word* blocks, size_t pos) {
        blocks[block_index(pos)] &= (_Word)~bit_mask(pos);
    }

    /*!
     * \brief flip bit in bits array at position pos
     * \param blocks pointer to word array
     * \param pos absolute bit position
     * \param v value(true or false)
     * \precond bits is non-zero valid pointer
     * \precond pos is less than equal bits in word times
     * number of words pointed by bits pointer
     */
    static inline void flip_bit_unchecked(_Word* blocks, size_t pos) {
        blocks[block_index(pos)] ^= (_Word)bit_mask(pos);
    }

    /*!
     * \brief Count amount of machine words to hold n bits
     * \return number of machine words of type _Word to hold n bits
     */
    static inline size_t bit_space(size_t nbits) {
        //return (nbits / bpw + static_cast<size_t>(nbits % bpw != 0));
        //return ((nbits) / bpw + ((nbits) % bpw == 0 ? 0 : 1));
        return (nbits + bpw-1) / bpw;
    }
    /*!
     * \internal word index
     * \param pos absolute bit position
     * \return word index
     */
    static inline size_t block_index(size_t pos) {
        return (pos / bpw);
    }

    /*!
     * \internal bit index
     * \param pos absolute bit position
     * \return bit index
     */
    static inline unsigned bit_index(size_t pos) {
        return static_cast<size_t>(pos % bpw);
    }

    /*!
     * \internal bit mask
     * \param pos absolute bit position
     * \return bit mask
     */
    static inline _Word bit_mask(size_t pos) {
        return (_Word(1) << bit_index(pos));
    }
};

_STDX_END


