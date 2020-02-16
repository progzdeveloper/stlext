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
#include <climits>

#include "../platform/common.h"
#include "../platform/bits.h"


_STDX_BEGIN

/*! */
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
     * \precond pos is less than equal bits in word multiplies number of words
     * pointed by bits pointer
     */
    static inline bool test_bit(const _Word* blocks, size_t pos) {
        return ((blocks[block_index(pos)] & (_Word)bit_mask(pos)) != 0);
    }

    /*!
     * \brief set or unset bit in bits array
     * at position pos according to value of v
     * \param blocks pointer to word array
     * \param pos absolute bit position
     * \param v value (true or false)
     * \precond bits is non-zero valid pointer
     * \precond pos is less than equal bits in word times number of words
     * pointed by bits pointer
     */
    static inline void set_bit(_Word* blocks, size_t pos, bool v)
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
     * \precond pos is less than equal bits in word times number of words
     * pointed by bits pointer
     */
    static inline void clear_bit(_Word* blocks, size_t pos) {
        blocks[block_index(pos)] &= (_Word)~bit_mask(pos);
    }

    /*!
     * \brief flip bit in bits array at position pos
     * \param blocks pointer to word array
     * \param pos absolute bit position
     * \param v value(true or false)
     * \precond bits is non-zero valid pointer
     * \precond pos is less than equal bits in word times number of words
     * pointed by bits pointer
     */
    static inline void flip_bit(_Word* blocks, size_t pos) {
        blocks[block_index(pos)] ^= (_Word)bit_mask(pos);
    }

    /*!
     * \brief Count amount of machine words to hold n bits
     * \return number of machine words of type _Word to hold n bits
     */
    static inline size_t bit_space(size_t nbits) {
        return (nbits / bpw + static_cast<size_t>(nbits % bpw != 0));
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


