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

#include <bitset>

#include "bitset.hpp"
#include "bitstorage.hpp"

_STDX_BEGIN

/*!
 * \class bitvector
 *
 * \tparam _Word   type of machine word
 * \tparam _Opt    type of space optimization
 * \tparam _Alloc  type of memory allocator
 *
 *
 */
template<
     class _Word = uintptr_t,
     class _Alloc = std::allocator<_Word>,
     size_t _Opt = __DefaultBitStorageTag
>
class bitvector :
        public bitstorage<_Opt, _Word, _Alloc>
{
    static_assert(std::is_integral<_Word>::value && std::is_unsigned<_Word>::value,
                  "_Word type must be an unsigned interal type");

public:
    typedef bitstorage<_Opt, _Word, _Alloc> storage_type; /*! bit-storage type */

    typedef _Alloc allocator_type; /*! allocator type */

    typedef size_t size_type;      /*! type of size */
    typedef bool   element_type;   /*! element type (bool) */
    typedef _Word  word_type;      /*! type of word */

    typedef _Word* pointer;              /*! word pointer type */
    typedef const _Word* const_pointer;  /*! constant word pointer type*/


    typedef stdx::bit_traits<_Word> traits_type;  /*! bit traits type */


    typedef stdx::bit_reference<_Word>            reference;       /*! type of reference */
    typedef stdx::bit_reference<const _Word>      const_reference; /*! type of const_reference */


    typedef stdx::bit_iterator<_Word, false>      iterator;         /*! type of iterator */
    typedef stdx::bit_iterator<_Word, true>       const_iterator;   /*! type of const iterator */

    typedef std::reverse_iterator<iterator>       reverse_iterator;        /*! type of reverse iterator */
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;  /*! type of constant reverse iterator */

    //
    // Constants
    //

    static constexpr size_t bpw          = traits_type::bpw; /*! number of bits per word */
    static constexpr size_t optimization = _Opt;             /*! storage space optimization tag */


    //
    // Constructors and destructor
    //

    bitvector();

    bitvector(const bitvector& other);
    //bitvector(bitvector&& other);

    explicit bitvector(const _Alloc& al);
    explicit bitvector(size_type n, bool on = false, const _Alloc& al = _Alloc());

    bitvector(const_pointer p, size_t pn, size_t nbits = 0, const _Alloc& al = _Alloc());

    bitvector(std::initializer_list<bool> ilist, const _Alloc& al = _Alloc());

    // TODO: think how we can implement this efficently?
    /*template<class _InIt>
    bitvector(_InIt first, _InIt last, size_t nbits = 0, const _Alloc& al = _Alloc());*/

    template<class _Wx, bool _IsConst>
    bitvector(bit_iterator<_Wx, _IsConst> first,
              bit_iterator<_Wx, _IsConst> last,
              const _Alloc& al = _Alloc());

    template<class _Wx, class _Al, size_t _Tag>
    bitvector(const bitvector<_Wx, _Al, _Tag>& other);

    template<size_t N, class _Wx>
    bitvector(const stdx::bitset<N, _Wx>& bits);

    template<size_t N>
    bitvector(const std::bitset<N>& bits);

    ~bitvector();

    //
    // Assignment and move
    //


    template<class _Wx, class _Al, size_t _Tag>
    bitvector& operator= (const bitvector<_Wx, _Al, _Tag>& other);

    bitvector& operator= (std::initializer_list<bool> ilist);

    bitvector& operator= (const bitvector& other);
    //bitvector& operator= (bitvector&& other);

    void assign(size_t n, bool on);
    void assign(const_pointer p, size_t pn, size_t nbits = 0);

    void assign(std::initializer_list<bool> ilist);

    // TODO: think how we can implement this efficently?
    /*template<class _InIt>
    bitvector& assign(_InIt first, _InIt last, size_t nbits = 0);*/

    template<class _Wx, bool _IsConst>
    void assign(bit_iterator<_Wx, _IsConst> first,
                bit_iterator<_Wx, _IsConst> last);



    allocator_type get_allocator() const;

    void swap(bitvector& other);

    //
    // Iterators
    //

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

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    //
    // Memory management and observers
    //

    template<bool _IsConst>
    iterator insert(const_iterator pos,
                    bit_iterator<_Word, _IsConst> first,
                    bit_iterator<_Word, _IsConst> last);

    iterator insert(const_iterator pos,
                    size_type count,
                    bool value = false);

    iterator insert(const_iterator pos,
                    std::initializer_list<bool> ilist);


    iterator erase(const_iterator first, const_iterator last);
    iterator erase(const_iterator pos, size_t n);


    void resize(size_t nbits, bool on = false);

    //
    // Provided by bitstorage interface:
    //
    // void resize(size_t nbits);
    // void shrink();
    // void clear();
    //
    // pointer data();
    // const_pointer data() const;
    // size_t size() const;
    //

    size_type max_size() const;
    size_type nblocks() const;
    size_type capacity() const;

    bool empty() const;

    size_type count(bool on = true) const;
    double density() const;

    bool all() const;
    bool any() const;
    bool none() const;

    //
    // Bit-level access operations
    //

    reference operator[](size_t pos);
    bool operator[](size_t pos) const;

    bool test(size_type pos) const;
    bool at(size_type pos) const;

    bitvector &set(size_type pos, bool on = true);
    bitvector& set();

    bitvector& reset(size_type pos);
    bitvector& reset();

    bitvector& flip(size_type pos);
    bitvector& flip();

    //
    // Set-relational operations
    //

    bool contains(const bitvector& other);
    bool intersects(const bitvector& other);

    //
    // Bitwise operations
    //

    bitvector& operator~ ();

    bitvector& operator&= (const bitvector& other);
    bitvector& operator|= (const bitvector& other);
    bitvector& operator^= (const bitvector& other);

    bitvector& operator<<= (size_type pos);
    bitvector& operator>>= (size_type pos);

    bitvector operator<< (size_type pos);
    bitvector operator>> (size_type pos);


    //
    // String conversion
    //

    template<class _Char, class _Traits, class _CharAlloc>
    inline operator std::basic_string<_Char, _Traits, _CharAlloc> () const {
        std::basic_string<_Char, _Traits, _CharAlloc> result;
        return stdx::detail::__to_string(this->begin(), this->end(), result, _Char('1'), _Char('0'));
    }

    template<class _Char, class _Traits, class _CharAlloc>
    inline std::basic_string<_Char, _Traits, _CharAlloc>&
        to_string(std::basic_string<_Char, _Traits, _CharAlloc>& s, _Char on, _Char off) const {
            return stdx::detail::__to_string(this->begin(), this->end(), s, on, off);
    }
};





//
// Contructors and destructor
//

/*!
 * \brief Default constructor
 *
 * Constructs an empty bit vector
 * \note \b Complexity: constant O(1)
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector() :
    storage_type()
{
}

/*!
 * \brief Copy constructor
 * Constructs a bit vector by coping data from other.
 * \param other const bit vector reference to copy from
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector(const bitvector& other) :
    storage_type(other)
{
}

/*!
 * \brief Move contructor
 * Constructs a bit vector by moving data from other.
 * \param other bit vector rvalue reference to move from
 * \note \b Complexity: constant O(1)
 */
/*template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector(bitvector&& other) {
    static_cast<storage_type*>(this)->move(static_cast<storage_type&&>(other));
}*/

/*!
 * \brief Parametized constructor
 * Constructs an empty bit vector.
 * \param al allocator const reference
 * \note \b Complexity: constant O(1)
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector(const _Alloc &al) :
    storage_type(al)
{
}

/*!
 * \brief Parametized constructor
 * Construct a bitvector with size n bits initialized with value on.
 * \param n  number of bits in vector
 * \param on bits initialization value (0 by default)
 * \param al allocator const reference
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector(size_type n, bool on, const _Alloc& al) :
    storage_type(al)
{
    this->assign(n, on);
}

/*!
 * \brief Parametized constructor
 *
 * Assign bits from array pointed by pointer p.
 *
 * Optional nbits argument regulates number of bits to copy from
 * source array. The default value of 0 means that all bits from
 * source array must be copied, so actual size of bitvector becomes
 * pn * bpw. If nbits is greater than 0 than coping process stops
 * when number of elemens is exhaused in array p is exhaused or
 * desired number of bits was reached, whichever comes first.
 * \note If number of elements in array pointed by pn is less than
 * desired number of bits an std::logic_error is throw.
 * \param p     pointer to first input word
 * \param pn    number of words pointer by first
 * \param nbits desired number of bits in vector (default 0, that means all bits in all words)
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector(const_pointer p, size_t pn, size_t nbits, const _Alloc &al) :
    storage_type(al)
{
    this->assign(p, pn, nbits);
}

/*!
 * \brief Parametized constructor
 *
 * Construct a bitvector from initializer list of boolean values
 *
 * \note the initilaizer list processed in a reverse
 * order, that results in natural order of bits on
 * little-endian processor.
 * \param ilist initializer list
 * \param al    allocator const reference
 * \note \b Complexity: linear O(N) bit assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::bitvector(std::initializer_list<bool> ilist, const _Alloc& al) :
    storage_type(al)
{
    this->assign(ilist);
}

// TODO: think how we can implement this efficently?
/*
template<class _Word, class _Alloc, size_t _Opt>
template<class _InIt>
bitvector<_Word, _Alloc, _Opt>::bitvector(_InIt first, _InIt last, size_t nbits, const _Alloc& al) :
    storage_type(al)
{
}*/

/*!
 * \brief Parametized constructor
 *
 * Construct a bitvector from range of bit iterators
 * \param first first range iterator
 * \param last  last range iterator
 * \param al    allocator const reference
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
template<class _Wx, bool _IsConst>
bitvector<_Word, _Alloc, _Opt>::bitvector(
          bit_iterator<_Wx, _IsConst> first,
          bit_iterator<_Wx, _IsConst> last,
          const _Alloc& al) :
    storage_type(al)
{
    this->assign(first, last);
}

/*!
 * \brief Copy constrcutor
 * Constructs a bit vector by coping data from other.
 * \param other source bitvector
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
template<class _Wx, class _Al, size_t _Tag>
bitvector<_Word, _Alloc, _Opt>::bitvector(const bitvector<_Wx, _Al, _Tag>& other) :
    storage_type()
{
    const_iterator first(reinterpret_cast<const_pointer>(other.data()), 0);
    this->assign(first, first + other.size());
}

/*!
 * \brief Copy constrcutor
 * Constructs a bit vector by coping data from bitset bits.
 * \param bits bitset const reference
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
template<size_t N, class _Wx>
bitvector<_Word, _Alloc, _Opt>::bitvector(const stdx::bitset<N, _Wx>& bits)
{
    this->assign(bits.begin(), bits.end());
}


/*!
 * \brief Copy constrcutor
 * Constructs a bit vector by coping data from bitset bits.
 * \param bits bitset const reference
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
template<size_t N>
bitvector<_Word, _Alloc, _Opt>::bitvector(const std::bitset<N>& bits)
{
    this->resize(N, false);
    for (size_t i = 0; i < bits.size(); ++i) {
        this->set(i, bits.test(i));
    }
}



//
// Destructor
//


/*!
 * \brief Destructor
 *
 * Destroy the bitvector and release all occupied resources.
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>::~bitvector()
{
}

/*!
 * \brief Bits assignmet
 *
 * Assign n bits initilized with value on
 * \param n number of desired bits in bit vector
 * \param on value to initialize bits (defualt is 0)
 */
template<class _Word, class _Alloc, size_t _Opt>
void bitvector<_Word, _Alloc, _Opt>::assign(size_t n, bool on)
{
    this->resize(n, on);
}

/*!
 * \brief Bits assignmet
 *
 * Assign bits from array pointed by pointer p.
 *
 * Optional nbits argument regulates number of bits to copy from
 * source array. The default value of 0 means that all bits from
 * source array must be copied, so actual size of bitvector becomes
 * pn * bpw. If nbits is greater than 0 than coping process stops
 * when number of elemens is exhaused in array p is exhaused or
 * desired number of bits was reached, whichever comes first.
 * \note If number of elements in array pointed by pn is less than
 * desired number of bits an std::logic_error is throw.
 * \param p     pointer to first input word
 * \param pn    number of words pointer by first
 * \param nbits desired number of bits in vector (default 0, that means all bits in all words)
 * \note \b Complexity: linear O(N) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
void bitvector<_Word, _Alloc, _Opt>::assign(const_pointer p, size_t pn, size_t nbits)
{
    if (nbits == 0) {
        nbits = (pn * traits_type::bpw);
    } else {
        __stdx_assert((pn * traits_type::bpw) >= traits_type::bit_space(nbits), std::logic_error);
    }
    this->__resize(nbits);
    const_iterator first(p, 0);
    stdx::copy_n(first, nbits, this->begin());
}

// TODO: think how we can implement this efficently?
/*template<class _InIt>
bitvector& assign(_InIt first, _InIt last, size_t nbits = 0);*/


/*!
 * \brief Bits assignmet
 *
 * Assign bits from initializer list of boolean values.
 *
 * \note the initilaizer list processed in a reverse
 * order, that results in natural order of bits on
 * little-endian processor.
 * \param ilist initializer list
 * \param al    allocator const reference
 * \note \b Complexity: linear O(N) bit assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
void bitvector<_Word, _Alloc, _Opt>::assign(std::initializer_list<bool> ilist)
{
    if (ilist.size() == 0)
        return;

    this->__resize(ilist.size());
    size_t i = 0;
    for (auto it = ilist.end()-1; it >= ilist.begin(); --it, ++i) {
        this->set(i, *it);
    }
    this->__sanitize_bits();
}

/*!
 * \brief Bits assignment
 *
 * Assign from range of bit iterators
 * \param first first range iterator
 * \param last  last range iterator
 * \param al    allocator const reference
 * \note \b Complexity: linear O(N/bpw) assignments
 */
template<class _Word, class _Alloc, size_t _Opt>
template<class _Wx, bool _IsConst>
void bitvector<_Word, _Alloc, _Opt>::assign(bit_iterator<_Wx, _IsConst> first,
                                            bit_iterator<_Wx, _IsConst> last)
{
    typedef typename std::conditional<
            _IsConst, const uint8_t*, uint8_t*
    >::type srcptr_t;
    typedef uint8_t* dstptr_t;

    size_t n = std::distance(first, last);
    this->__resize(n);
    stdx::bit_iterator<uint8_t, _IsConst>  source(reinterpret_cast<srcptr_t>(first.base()), first.pos());
    stdx::bit_iterator<uint8_t, false>     target(reinterpret_cast<dstptr_t>(this->data()), 0);
    stdx::copy_n(source, n, target);
}

/*!
 * \brief Get an internal allocator
 * \return allocator used by this bit vector
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::allocator_type
bitvector<_Word, _Alloc, _Opt>::get_allocator() const {
    return static_cast<const storage_type*>(this)->get_allocator();
}

/*!
 * \brief Swap this bitvector with other
 */
template<class _Word, class _Alloc, size_t _Opt>
void bitvector<_Word, _Alloc, _Opt>::swap(bitvector& other)
{
    if (&other == this)
        return; // attempt to swap with self

    static_cast<storage_type*>(this)->swap(static_cast<storage_type&>(other));
}

//
// Assignment and move operators
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
template<class _Wx, class _Al, size_t _Tag>
bitvector<_Word, _Alloc, _Opt>& bitvector<_Word, _Alloc, _Opt>::operator= (const bitvector<_Wx, _Al, _Tag>& other)
{
    if (&other == this)
        return (*this); // escape self-assignment

    this->assign(other.begin(), other.end());
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>& bitvector<_Word, _Alloc, _Opt>::operator= (std::initializer_list<bool> ilist)
{
    this->assign(ilist);
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>& bitvector<_Word, _Alloc, _Opt>::operator= (const bitvector& other)
{
    if (&other == this)
        return (*this); // escape self-assignment
    this->assign(other.begin(), other.end());
    return (*this);
}

/*!
 *
 */
/*template<class _Word, class _Alloc,  size_t _Opt>
bitvector<_Word, _Alloc, _Opt>& bitvector<_Word, _Alloc, _Opt>::operator= (bitvector&& other)
{
    if (std::addressof(other) == this)
        return (*this); // escape self-move

    return this->move(static_cast<storage_type>(other));
}*/




//
// Iterators
//
/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::begin() { return iterator(this->data(), 0); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::end() { return (begin() += this->size()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_iterator
bitvector<_Word, _Alloc, _Opt>::begin() const { return const_iterator(this->data(), 0); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_iterator
bitvector<_Word, _Alloc, _Opt>::end() const { return (begin() += this->size()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_iterator
bitvector<_Word, _Alloc, _Opt>::cbegin() const { return const_iterator(this->data(), 0); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_iterator
bitvector<_Word, _Alloc, _Opt>::cend() const { return (cbegin() += this->size()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::reverse_iterator
bitvector<_Word, _Alloc, _Opt>::rbegin() { return reverse_iterator(end()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::reverse_iterator
bitvector<_Word, _Alloc, _Opt>::rend() { return reverse_iterator(begin()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_reverse_iterator
bitvector<_Word, _Alloc, _Opt>::rbegin() const { return const_reverse_iterator(end()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_reverse_iterator
bitvector<_Word, _Alloc, _Opt>::rend() const { return const_reverse_iterator(begin()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_reverse_iterator
bitvector<_Word, _Alloc, _Opt>::crbegin() const { return const_reverse_iterator(end()); }

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::const_reverse_iterator
bitvector<_Word, _Alloc, _Opt>::crend() const { return const_reverse_iterator(begin()); }


//
// Memory management
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
template<bool _IsConst>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::insert(const_iterator pos,
                bit_iterator<_Word, _IsConst> beg,
                bit_iterator<_Word, _IsConst> end)
{
    if (beg == end)
        return (this->begin() + (pos - this->begin()));

    // FIXME: don't forget to uncomment me!
    //__stdx_assert(pos >= this->begin() && pos <= end(), std::out_of_range);

    size_t offset = pos - this->begin();
    size_t s = this->size();
    size_t n = std::distance(beg, end);
    size_t length = s + n;
    if ( length > this->max_size() ) {
        throw std::length_error("maximum vector size exceeded");
    }

    this->resize(length);  // pos is invalidated after resize
    iterator first = this->begin();
    iterator result = first + offset;
    if (offset != s)
        stdx::copy(result, first + s, result + count);
    stdx::copy(beg, end, result);
    return result;
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::insert(const_iterator pos,
                                       size_type count,
                                       bool value)
{
    if (count == 0)
        return (this->begin() + (pos - this->cbegin()));

    // FIXME: don't forget to uncomment me!
    //__stdx_assert(pos >= this->begin() && pos <= end(), std::out_of_range);

    size_t offset = pos - this->cbegin();
    size_t s = this->size();
    size_t n = s + count;
    if ( n > this->max_size()) {
        throw std::length_error("maximum vector size exceeded");
    }

    this->__resize(n); // pos is invalidated after resize

    iterator first = this->begin();
    iterator result = first + offset;
    stdx::copy(result, first + s, result + n);
    stdx::fill_n(result, count, value);
    return result;
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::insert(const_iterator pos,
                                       std::initializer_list<bool> ilist)
{
    if (ilist.size() == 0)
        return (this->begin() + (pos - this->cbegin()));

    // FIXME: don't forget to uncomment me!
    //__stdx_assert(pos >= this->begin() && pos <= end(), std::out_of_range);

    size_t offset = pos - this->cbegin();
    size_t s = this->size();
    size_t n = ilist.size();
    size_t length = s + n;
    if ( length > this->max_size() ) {
        throw std::length_error("maximum vector size exceeded");
    }

    this->__resize(length);  // pos is invalidated after resize
    iterator first = this->begin();
    iterator result = first + offset;
    if (offset != s) {
        stdx::copy(result, first + s, result + n);
    }

    s = offset;
    for (auto it = ilist.end()-1; it >= ilist.begin(); --it, ++s) {
        this->set(s, *it);
    }
    return result;
}
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::erase(const_iterator first, const_iterator last)
{
    return erase(first, std::distance(first, last));
}

template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::iterator
bitvector<_Word, _Alloc, _Opt>::erase(const_iterator pos, size_t n)
{
    // FIXME: don't forget to uncomment me!
    //__stdx_assert(pos != this->cend() && ((std::distance(this->cbegin(), pos) + n) < size(), std::out_of_range);

    size_t i = std::distance(this->cbegin(), pos);
    auto ppos = stdx::copy(pos + n, this->cend(), this->begin() + i);
    this->__resize(std::distance(this->begin(), ppos));
    return this->begin() + (i + n);
}


/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
void bitvector<_Word, _Alloc, _Opt>::resize(size_t nbits, bool on)
{
    size_t n = this->size();
    this->__resize(nbits);
    if (nbits > n) {
        stdx::fill(this->begin() + n, end(), on);
    }
}

//
// Observers
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::size_type
bitvector<_Word, _Alloc, _Opt>::max_size() const
{
    return storage_type::max_bits;
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::size_type
bitvector<_Word, _Alloc, _Opt>::nblocks() const {
    return traits_type::bit_space(this->size());
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::size_type
bitvector<_Word, _Alloc, _Opt>::capacity() const {
    return (nblocks() * bpw);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::empty() const {
    return (this->size() == 0);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename bitvector<_Word, _Alloc, _Opt>::size_type
bitvector<_Word, _Alloc, _Opt>::count(bool on) const {
    return stdx::count(cbegin(), cend(), on);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
double bitvector<_Word, _Alloc, _Opt>::density() const {
    size_type n = this->size();
    return (n != 0 ? (this->count() / static_cast<double>(n)) : 0.0);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::all() const {
    size_type n = this->size();
    return (stdx::detail::__count_bool_true(this->begin(), n) == n);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::any() const {
    return (stdx::detail::__find_bool_true(this->begin(), this->size()) != this->end());
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::none() const {
    return (!any());
}


//
// Bit-level access
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
typename  bitvector<_Word, _Alloc, _Opt>::reference
bitvector<_Word, _Alloc, _Opt>::operator[](size_t pos) {
    return reference(*(this->data() + traits_type::block_index(pos)),
                     (word_type)traits_type::bit_mask(pos));
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::operator[](size_t pos) const {
    return this->test(pos);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::test(size_type pos) const {
    return traits_type::test_bit_unchecked(this->data(), pos);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::at(size_type pos) const {
    // FIXME: don't forget to uncomment me!
    //__stdx_assert(pos < size(), std::out_of_range);
    return this->test(pos);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::set(size_type pos, bool on) {
    traits_type::set_bit_unchecked(this->data(), pos, on);
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::set() {
    stdx::detail::__fill_n_true(this->begin(), this->size());
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::reset(size_type pos) {
    traits_type::clear_bit_unchecked(this->data(), pos);
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::reset() {
    stdx::detail::__fill_n_false(this->begin(), this->size());
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::flip(size_type pos) {
    traits_type::flip_bit_unchecked(this->data(), pos);
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::flip() {
    stdx::detail::__flip_inplace(this->begin(), this->end());
    return (*this);
}




//
// Set-relational operations
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::contains(const bitvector& other)
{
    if (&other == this)
        return true;

    // FIXME: don't forget to uncomment me!
    //__stdx_assert(other.size() == this->size(), std::logic_error);
    const_pointer first1 = this->data();
    const_pointer last1 = this->data() + this->nblocks();
    const_pointer first2 = other.data();
    for (; first1 != last1; ++first1) {
        if ((*first1) & ~(*first2))
            return false;
    }
    return true;
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bool bitvector<_Word, _Alloc, _Opt>::intersects(const bitvector& other)
{
    if (&other == this)
        return true;

    size_type n = (std::min)(this->nblocks(), other.nblocks());
    const_pointer first1 = this->data();
    const_pointer last1 = this->data() + n;
    const_pointer first2 = other.data();
    for (; first1 != last1; ++first1) {
        if ((*first1) & (*first2))
            return true;
    }
    return false;
}



//
// Bit-level access operations (in class definitions)
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::operator~ ()
{
    return bitvector(*this).flip();
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::operator&= (const bitvector& other)
{
    if (&other == this)
        return (*this);

    // FIXME: don't forget to uncomment me!
    //__stdx_assertx(other.size() == size(), std::range_error, "incorrect range");

    // this code relies on auto-vectorization
    stdx::transform(cbegin(), cend(), other.begin(), begin(), std::bit_and<word_type>());
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::operator|= (const bitvector& other)
{
    if (&other == this)
        return (*this);

    // FIXME: don't forget to uncomment me!
    //__stdx_assertx(other.size() == size(), std::range_error, "incorrect range");

    // this code relies on auto-vectorization
    stdx::transform(cbegin(), cend(), other.begin(), begin(), std::bit_or<word_type>());
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::operator^= (const bitvector& other)
{
    if (&other == this)
        return reset();

    // FIXME: don't forget to uncomment me!
    //__stdx_assertx(other.size() == size(), std::range_error, "incorrect range");

    // this code relies on auto-vectorization
    stdx::transform(cbegin(), cend(), other.begin(), begin(), std::bit_xor<word_type>());
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::operator<<= (size_type pos)
{
    this->__bitwise_shl(pos);
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>&
bitvector<_Word, _Alloc, _Opt>::operator>>= (size_type pos)
{
    this->__bitwise_shr(pos);
    return (*this);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>
bitvector<_Word, _Alloc, _Opt>::operator<< (size_type pos)
{
    return (bitvector(*this) <<= pos);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
bitvector<_Word, _Alloc, _Opt>
bitvector<_Word, _Alloc, _Opt>::operator>> (size_type pos)
{
    return (bitvector(*this) >>= pos);
}


//
// Bit-level access operations (out of class definitions)
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
inline bitvector<_Word, _Alloc, _Opt> operator& (const bitvector<_Word,  _Alloc, _Opt>& lhs,
                                                 const bitvector<_Word,  _Alloc, _Opt>& rhs)
{
    bitvector<_Word, _Alloc, _Opt> result(lhs);
    return (result &= rhs);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
inline bitvector<_Word, _Alloc, _Opt> operator| (const bitvector<_Word, _Alloc, _Opt>& lhs,
                                                 const bitvector<_Word, _Alloc, _Opt>& rhs)
{
    bitvector<_Word, _Alloc, _Opt> result(lhs);
    return (result |= rhs);
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
inline bitvector<_Word, _Alloc, _Opt> operator^ (const bitvector<_Word, _Alloc, _Opt>& lhs,
                                                 const bitvector<_Word, _Alloc, _Opt>& rhs)
{
    bitvector<_Word, _Alloc, _Opt> result(lhs);
    return (result ^= rhs);
}


//
// Comparison
//

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
inline bool operator== (const bitvector<_Word, _Alloc, _Opt>& lhs,
                        const bitvector<_Word, _Alloc, _Opt>& rhs)
{
    if (&lhs == &rhs)
        return true;
    return stdx::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/*!
 *
 */
template<class _Word, class _Alloc, size_t _Opt>
inline bool operator!= (const bitvector<_Word, _Alloc, _Opt>& lhs,
                        const bitvector<_Word, _Alloc, _Opt>& rhs)
{
    return (!(lhs == rhs));
}


//
// Stream serialization
//

/*!
 *
 */
template<
    class _Char, class _Traits,
    class _Word, class _Alloc, size_t _Opt
>
inline std::basic_ostream<_Char, _Traits>& operator<<(std::basic_ostream<_Char, _Traits>& stream,
                                                      const bitvector<_Word,  _Alloc, _Opt>& bits)
{
    return stdx::detail::__to_stream(bits.begin(), bits.end(), stream);
}

_STDX_END // end namespace stdx



namespace std {

    /// std::hash specialization for bitvector.
    template<class _Word,  class _Alloc, size_t _Opt>
    struct hash< stdx::bitvector<_Word, _Alloc, _Opt> >
    {
        size_t operator()(const stdx::bitvector<_Word, _Alloc, _Opt>& bits) const noexcept
        {
            static const std::size_t __S_seed = static_cast<size_t>(0xc70f6907UL);
            return std::_Hash_bytes(bits.data(), bits.nblocks(), __S_seed);
        }
    };
}

