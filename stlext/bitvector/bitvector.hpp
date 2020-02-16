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
#include <bitset>
#include "bit_storage.hpp"
#include "../functional/bit_andnot.hpp"

_STDX_BEGIN


/*!
\brief template class bitvector<_Word, _Alloc> represent a dynamic-sized bit array.

\tparam _Word machinery word type (defaulted to uintmax_t)
\tparam _Alloc memory allocator type (defaulted to std::allocator<_Word>)

class bitvector<_Word, _Alloc> represent a dynamic-sized bit array.

A bitvector is an array that gives access to individual bits and provides
basic operators (such as AND, OR, XOR, NOT etc.) that work on entire arrays of bits.

As a dynamic-sized vector of bit, bitvector instances can be resized in
run-time to any number of bits between 0 and a value of max_size() method.
A bitvector with size of 0 bits is treated as empty vector.

Memory allocation/deallocation is managed by special functions (like reserve(), 
resize(), shrink() and others) that resembles such in a STL vector<T> container.

Internally bit data is stored in continious memory blocks of type _Word.
All bits are packed in machine words of type _Word.
It may or may not using SBO (Small-Buffer-Optimization) depending on 
combination of current OS, processor and environment settings.
Bit storage is implemented in bit_storage<> internal class, and not
intended to use directly.

bitvector<_Word, _Alloc> class implemented according to STL
container specifications and provide some optimized standart algorithms.

*/
template<
    class _Word = uintmax_t,
    class _Alloc = std::allocator<_Word>
>
class bitvector
{

#ifndef _STDX_NO_BITVECTOR_SBO_
#if defined(STDX_PROCESSOR_X86_64) || defined(STDX_PROCESSOR_IA64)
#ifdef _STDX_NO_AGRESSIVE_SBO_ // use SBO for bitvector
    typedef buf_opt optimization_type;
#else // use agressive SBO with tagged pointers
    typedef tag_opt optimization_type;
#endif
#else // non-optimized version
    typedef non_opt optimization_type;
#endif
#else // SBO is disabled
    typedef non_opt optimization_type;
#endif

    typedef detail::bit_storage<_Word, _Alloc, optimization_type> storage_t;
    typedef typename storage_t::traits_type traits_type;

public:
    /*! number of bits per word */
    static constexpr size_t bpw = storage_t::bpw;

    /*! allocator type */
    typedef _Alloc allocator_type;

    /*! type of word */
    typedef _Word word_type;
    /*! type of size */
    typedef size_t size_type;
    /*! word pointer type */
    typedef word_type* pointer;
    /*! constant word pointer type*/
    typedef const word_type* const_pointer;
    /*! element type (bool) */
    typedef bool  element_type;

    /*! type of reference */
    typedef bit_reference<_Word>        reference;
    /*! type of const_reference */
    typedef bit_const_reference<_Word>  const_reference;

    /*! type of iterator */
    typedef bit_iterator<_Word, false>  iterator;
    /*! type of const iterator */
    typedef bit_iterator<_Word, true>   const_iterator;

    /*! type of reverse iterator */
    typedef std::reverse_iterator<iterator> reverse_iterator;
    /*! type of const reverse iterator */
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    /*!
    \brief default constructor
    */
    bitvector() {
    }

    /*!
    \brief parametized constructor
    \param al allocator const reference
    */
    explicit bitvector(const _Alloc& al) :
        __m_storage(al) {
    }

    /*!
    \brief parametized constructor
    \param n  number of bits in vector
    \param v  bits initialization value (0 by default)
    \note \b Complexity: linear O(N/bpw) bit assignments
    */
    explicit bitvector(size_type n, element_type v = element_type(0)) :
        __m_storage(n, v) {
    }

    /*!
    \brief parametized constructor
    \tparam _It  iterator type
    \param nbits number of bits in vector
    \param first iterator pointing to first input word
    \param last  iterator pointing to one-past-last input word
    \note \b Complexity: linear O(N/bpw) bit assignments
    \warning type of iterator value type must be the same as bitvector _Word
    */
    template<class _It>
    explicit bitvector(size_type nbits, _It first, _It last) {
        __m_storage.assign(nbits, first, last);
    }

    /*!
    \brief parametized constructor
    \tparam N   number of bits in bitset
    \param bits bitset const reference
    \note \b Complexity: linear O(N) bit assignments
    */
    template<size_t N>
    bitvector(const std::bitset<N>& bits) :
        __m_storage(N, false)
    {
        for (size_t i = 0; i < N; i++)
            set(i, bits.test(i));
    }


    /// Copy/move constructors and operators
    /// are compiler generated


    /*!
    \brief destructor

    free all memory and resources occupied
    by current instance of bitvector
    */
    ~bitvector() {
    }

    /// MEMORY MANAGEMENT

    /*!
    \brief retrieve allocator object
    \return allocator that is currently in use
    */
    allocator_type get_allocator() const {
        return (allocator_type)__m_storage;
    }

    /*!
    \brief shrinks internal buffer
    */
    void shrink() {
        __m_storage.shrink();
    }

    /*!
    \brief reserve uninitialized memory

    reserve uninitialized memory for bitvector
    of size n bits
    \param n number of reserving bits
    */
    void reserve(size_t n) {
        __m_storage.reserve(n);
    }

    /*!
    \brief resize bitvector
    \param n new number of bits
    \param value value to fill on resize
    */
    void resize(size_t n, element_type value = 0) {
        __m_storage.resize(n, value);
    }

    /*!
    \brief clean up bitvector

    release all resources of bit vector
    and set size to 0
    */
    void clear() {
        __m_storage.clear();
    }


    /// BITWISE ACCESS

    /*!
    \brief clear bit with index pos
    \param pos bit index
    \return reference to modified bitvector
    \note \b Complexity: constant O(1)
    \warning trying to access bits outside valid range is undefined
    */
    inline bitvector& reset(size_type pos) noexcept
    {
        traits_type::clear_bit(__m_storage.data(), pos);
        return (*this);
    }

    /*!
    \brief clear all bits in bitvector

    set all bits to 0.
    \note \b Complexity: linear O(N/bpw)
    */
    inline bitvector& reset() {
        stdx::__fill_n_false(__m_storage.begin(), __m_storage.nbits());
        return (*this);
    }


    /*!
    \brief flip bit at index pos
    \param pos bit index
    \return reference to modified bitvector
    \note never throws
    \note \b Complexity: constant O(1)
    \warning trying to access bits outside valid range is undefined
    */
    inline bitvector& flip(size_type pos) noexcept
    {
        traits_type::flip_bit(__m_storage.data(), pos);
        return (*this);
    }


    /*!
    \brief flip all bits in bitvector

    flip all bits
    \note \b Complexity: linear O(N/bpw)
    */
    inline bitvector& flip() {
        stdx::__flip_inplace(__m_storage.begin(), __m_storage.end());
        return (*this);
    }


    /*!
    \brief set bit with index pos
    \param pos bit index
    \return reference to modified bitvector
    \note never throws
    \note \b Complexity: constant O(1)
    \warning trying to access bits outside valid range is undefined
    */
    inline bitvector& set(size_type pos, bool v) noexcept
    {
        traits_type::set_bit(data(), pos, v);
        return (*this);
    }

    /*!
    \brief set all bits

    set all bits to 1.
    \return reference to modified bitvector
    \note \b Complexity: linear O(N/bpw)
    */
    inline bitvector& set() {
        stdx::__fill_n_true(__m_storage.begin(), __m_storage.nbits());
        return (*this);
    }


    /*!
    \brief test bit at index pos
    \param pos bit index
    \return true if bit is set, otherwise false
    \note never throws
    \note \b Complexity: constant O(1)
    \warning trying to access bits outside valid range is undefined
    */
    inline bool test(size_type pos) const noexcept {
        return traits_type::test_bit(data(), pos);
    }


    /*!
    \brief test bit at index pos
    \param pos bit index
    \return true if bit is set, otherwise false
    \note throw out_of_range exception if pos is out of valid range
    */
    inline bool at(size_type pos) const {
        __stdx_assert(pos < size(), std::out_of_range, "incorrect bit position");
        return test(pos);
    }


    /*!
    \brief subscript operator overload
    \param pos bit index
    \return bit reference proxy class
    \note never throws
    \warning trying to access bits outside valid range is undefined
    */
    inline reference operator[](size_type pos) noexcept {
        return reference(*(data() + traits_type::block_index(pos)), (word_type)traits_type::bit_mask(pos));
    }

    /*!
    \brief subscript operator overload (const version)
    \param pos bit index
    \return bit reference proxy class
    \note never throws
    \warning trying to access bits outside valid range is undefined
    */
    inline bool operator[](size_type pos) const noexcept {
        return this->test(pos);
    }


    /// BIT ITERATORS

    iterator begin() { return __m_storage.begin(); }
    const_iterator begin() const { return __m_storage.begin(); }
    const_iterator cbegin() const { return __m_storage.begin(); }

    iterator end() { return __m_storage.end(); }
    const_iterator end() const { return __m_storage.end(); }
    const_iterator cend() const { return __m_storage.end(); }

    /// REVERSE ITERATORS

    reverse_iterator rbegin() { return reverse_iterator(__m_storage.end()); }
    const_reverse_iterator rbegin() const  { return const_reverse_iterator(__m_storage.end()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(__m_storage.end()); }

    reverse_iterator rend() { return reverse_iterator(__m_storage.begin()); }
    const_reverse_iterator rend() const  { return const_reverse_iterator(__m_storage.begin()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(__m_storage.begin()); }

    /// OBSERVER METHODS

    /*!
    \brief raw-pointer to internal machine words array (non-const version)
    \note provided for convinience
    */
    inline pointer data() { return __m_storage.data(); }
    /*!
    \brief raw-pointer to internal machine words array (const version)
    \note provided for convinience
    */
    inline const_pointer data() const { return __m_storage.data(); }

    /*!
    \brief number of allocated machine words
    \return number of allocated machine words to hold all bits
    */
    inline size_type blocks() const { return __m_storage.nblocks(); }

    /*!
    \brief size of bitvector
    \return number of bits in vector
    */
    inline size_type size() const { return __m_storage.nbits(); }

    /*!
    \brief internal buffer capacity in bits
    */
    inline size_type capacity() const { return (__m_storage.capacity()); }

    /*!
    \brief maximum internal buffer capacity in bits
    */
    inline size_type max_size() const { return (storage_t::max_size); }

    /*!
    \brief count set bits
    \return number of set bits
    */
    inline size_type count() const {
        return stdx::__count_bool_true(begin(), size());
    }

    /*!
    \brief density

    compute density of bit-vector
    \return density factor
    \note \b Complexity: O(N/bpw)
    */
    inline double density() const
    {
        size_type n = size();
        size_type s = count();
        return (n != 0 ? (s / static_cast<double>(n)) : 0.0);
    }

    /*!
    \brief checks if at least one bit is set
    \return true if at least one bit is set, otherwise false
    \note \b Complexity: O(N/bpw)
    */
    inline bool any() const {
        return (stdx::__find_bool_true(begin(), size()) != end());
    }

    /*!
    \brief checks if no bits is set
    \return true if at least one bit is set, otherwise false
    \note \b Complexity: O(N/bpw)
    */
    inline bool none() const {
        return (!any());
    }

    /*!
    \brief checks if all bits is set
    \return true if at least one bit is set, otherwise false
    \note \b Complexity: O(N/bpw)
    */
    inline bool all() const {
        return (count() == size());
    }

    /*!
    \brief checks if bitvector is empty
    \return true if bitvector is empty, otherwise false
    */
    inline bool empty() const {
        return (__m_storage.nbits() == 0);
    }

    /// COMMON BITWIZE OPERATIONS IMPLEMENTATIONS

    /// UNARY OPERATORS OVERLOADING

    /*!
    \brief bitwise AND unary operator overload
    \param __other bitvector to AND with
    \return reference to modified bitvector anded with other
    \note \b Complexity: linear O(size()/bpw) assignments and O(size()/#bpw) bitwise operations
    \note this code uses interally implemented algorithm with micro-optimizations
    */
    inline bitvector& operator&= (const bitvector& other)
    {
        __stdx_assert(other.size() == size(), std::range_error, "incorrect range");
        // this code relies on auto-vectorization
        transform(cbegin(), cend(), other.begin(), begin(), std::bit_and<word_type>());
        return (*this);
    }

    /*!
    \brief bitwise OR unary operator overload
    \param __other bitvector to OR with
    \return reference to modified bitvector ored with other
    \note \b Complexity: linear O(size()/bpw) assignments and O(size()/#bpw) bitwise operations
    \note this code uses interally implemented algorithm with micro-optimizations
    */
    inline bitvector& operator|= (const bitvector& other)
    {
        __stdx_assert(other.size() == size(), std::range_error, "incorrect range");
        // this code relies on auto-vectorization
        transform(cbegin(), cend(), other.begin(), begin(), std::bit_or<word_type>());
        return (*this);
    }

    /*!
    \brief bitwise XOR unary operator overload
    \param __other bitvector to XOR with
    \return reference to modified bitvector xored with other
    \note \b Complexity: linear O(size()/bpw) assignments and O(size()/#bpw) bitwise operations
    \note this code uses interally implemented algorithm with micro-optimizations
    */
    inline bitvector& operator^= (const bitvector& other)
    {
        __stdx_assert(other.size() == size(), std::range_error, "incorrect range");
        // this code relies on auto-vectorization
        transform(cbegin(), cend(), other.begin(), begin(), std::bit_xor<word_type>());
        return (*this);
    }

    /*!
    \brief bitwise AND-NOT unary operator overload
    \param __other bitvector to XOR with
    \return reference to modified bitvector and-not with __other
    \note \b Complexity: linear O(size()/bpw) assignments and O(size()/#bpw) bitwise operations
    \note this code uses interally implemented algorithm with micro-optimizations
    */
    inline bitvector& operator-=(const bitvector& other)
    {
        __stdx_assert(other.size() == size(), std::range_error, "incorrect range");
        // this code relies on auto-vectorization
        transform(cbegin(), cend(), other.begin(), begin(), std::bit_andnot<word_type>());
        return (*this);
    }

    /*!
    \brief bitwise left shift operator overload (SHL)
    \param pos shift amount
    \return reference to modified bitvector shifted left by pos
    \note \b Complexity: linear O(nwords) bitwise operations
    */
    inline bitvector& operator<<= (size_t pos) {
        __m_storage.lshift(pos);
        return (*this);
    }

    /*!
    \brief bitwise right shift operator overload (SHR)
    \param pos shift amount
    \return reference to modified bitvector shifted right by pos
    \note \b Complexity: linear O(nwords) bitwise operations
    */
    inline bitvector& operator>>= (size_t pos) {
        __m_storage.rshift(pos);
        return (*this);
    }

    /*!
    \brief bitwise left shift operator overload (SHL)
    \param pos shift amount
    \return copy of bitvector shifted left by pos (SHL)
    */
    inline bitvector operator<< (size_t pos) const {
        return (bitvector(*this) <<= pos);
    }

    /*!
    \brief bitwise right shift operator overload (SHR)
    \param pos shift amount
    \return copy of bitvector shifted right by pos (SHR)
    */
    inline bitvector operator>> (size_t pos) const {
        return (bitvector(*this) >>= pos);
    }


    /*!
    \brief Inversion operator overload
    \return copy of bitvector with all bits flipped
    \note \b Complexity: linear O(size()/bpw) bitwise operations
    */
    inline bitvector operator~() const {
        return bitvector(*this).flip();
    }


private:
    storage_t __m_storage;
};

/*!
\brief Equality comparison bit-vector operator overload
\param lhs first bitvector
\param rhs second bitvector
\return true if lhs is equal to rhs, otherwise return false 
\note \b Complexity: linear O(size()/bpw) comparison operations at worst case
\relates bitvector
*/
template<
        typename _Word,
        typename _Allocator
        >
inline bool operator== (const bitvector<_Word, _Allocator>& lhs,
                        const bitvector<_Word, _Allocator>& rhs) {
    return stdx::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/*!
\brief Non-equality comparison bit-vector operator overload
\param lhs first bitvector
\param rhs second bitvector
\return true if lhs is not equal to rhs, otherwise return false
\note \b Complexity: linear O(size()/bpw) comparison operations at worst case
\relates bitvector
*/
template<
        typename _Word,
        typename _Allocator
        >
inline bool operator!= (const bitvector<_Word, _Allocator>& lhs,
                        const bitvector<_Word, _Allocator>& rhs) {
    return (!(lhs == rhs));
}

/*!
\brief Bitwise OR binary operator overload
\param lhs first bitvector
\param rhs second bitvector
\return bitvector holding result of OR operation upon lhs and rhs bitvectors
\note \b Complexity: linear O(size()/bpw) assignments and O(size()/bpw) bitwise operations
\relates bitvector
*/
template<
        typename _Word,
        typename _Allocator
        >
inline bitvector<_Word, _Allocator> operator| (const bitvector<_Word, _Allocator>& lhs,
                                               const bitvector<_Word, _Allocator>& rhs)
{
    bitvector<_Word, _Allocator> res(lhs);
    res |= rhs;
    return res;
}

/*!
\brief Bitwise XOR binary operator overload
\param lhs first bitvector
\param rhs second bitvector
\return bitvector holding result of XOR operation upon lhs and rhs bitvectors
\note \b Complexity: linear O(size()/bpw) assignments and O(size()/bpw) bitwise operations
\relates bitvector
*/
template<
        typename _Word,
        typename _Allocator
        >
inline bitvector<_Word, _Allocator> operator^ (const bitvector<_Word, _Allocator>& lhs,
                                               const bitvector<_Word, _Allocator>& rhs)
{
    bitvector<_Word, _Allocator> res(lhs);
    res ^= rhs;
    return res;
}


/*!
\brief Bitwise AND binary operator overload
\param lhs first bitvector
\param rhs second bitvector
\return bitvector holding result of AND operation upon lhs and rhs bitvectors
\note \b Complexity: linear O(size()/bpw) assignments and O(size()/bpw) bitwise operations
\relates bitvector
*/
template<
        typename _Word,
        typename _Allocator
        >
inline bitvector<_Word, _Allocator> operator& (const bitvector<_Word, _Allocator>& lhs,
                                               const bitvector<_Word, _Allocator>& rhs)
{
    bitvector<_Word, _Allocator> res(lhs);
    res &= rhs;
    return res;
}


/*!
\brief Bitwise AND-NOT binary operator overload
\param lhs  first bitvector
\param rhs second bitvector
\return bitvector holding result of AND-NOT operation upon lhs and rhs bitvectors
\note \b Complexity: linear O(size()/bpw) assignments and O(size()/bpw) bitwise operations
\relates bitvector
*/
template<
        typename _Word,
        typename _Allocator
        >
inline bitvector<_Word, _Allocator> operator- (const bitvector<_Word, _Allocator>& lhs, 
                                               const bitvector<_Word, _Allocator>& rhs)
{
    bitvector<_Word, _Allocator> res(lhs);
    res -= rhs;
    return res;
}





_STDX_END

namespace std
{


    /*!
    \brief stream output operator overload

    Output bits in bitvector in human-readable format.

    \param stream reference to output stream
    \param bits target bitvector
    \return reference to output stream
    \relates bitvector
    */
    template<
            typename _OStream,
            typename _Word,
            typename _Allocator
            >
    inline _OStream& operator<< (_OStream& stream, const stdx::bitvector<_Word, _Allocator>& bits)
    {
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

        typedef stdx::bit_traits<uint8_t> traits_type;

        size_t n = bits.size(); // total number of bits
        if (n == 0) // nothing to output
            return (stream);

        size_t d = traits_type::bit_index(n); // number of meanful trailing bits
        size_t offset = (d > 0 ? (traits_type::bpw - d) : 0); // offset in string
        size_t length = (d > 0 ? d : traits_type::bpw); // length of string

        // pointer to first char in bit-vector
        const uint8_t* first = reinterpret_cast<const uint8_t*>(bits.data());
        // move pointer to last char
        const uint8_t* it = first + (traits_type::bit_space(n) - 1);

        // write part of string (output d bits at once)
        stream.write((bin_str[*it] + offset), length);
        --it;

        // process 8 bits at once in reverse order
        // so we do not need to care about alignment
        for (; it >= first; --it) {
            stream << bin_str[*it];
        }
        stream << std::flush; // flush the stream, if any
        return (stream);
    }


}

