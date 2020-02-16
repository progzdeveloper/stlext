// Copyright (c) 2019, Michael Polukarov (Russia).
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

#include <iosfwd>

#include "../compability/string_view"

#include "storages.hpp"


namespace compact {

    template<
        class _Char,
        class _Traits = std::char_traits<_Char>,
        class _Alloc = std::allocator<_Char>,
        size_t _TagBits = 0,
        size_t _SizeBits = 16 - _TagBits
    >
    class basic_string :
            public basic_string_storage<_Char, _Traits, _Alloc, _SizeBits, _TagBits>
    {
        typedef basic_string_storage<
            _Char, _Traits, _Alloc, _SizeBits, _TagBits
        > base_type;

        //template<class _TraitsT, size_t _NSize, size_t _NTag>
        //friend class basic_string<_Char, _TraitsT, _Alloc, _NSize, _NTag>;

    public:

        typedef _Traits traits_type;

        typedef _Char value_type;
        typedef _Char* pointer;
        typedef const _Char* const_pointer;
        typedef _Char& reference;
        typedef const _Char& const_reference;
        typedef pointer iterator;
        typedef const_pointer const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_pointer> const_reverse_iterator;
        typedef uint16_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef std::allocator<_Char> allocator_type;

        static constexpr size_t npos = size_type(-1);

        /**
         *  @brief  Default constructor creates an empty string.
         */
        basic_string() : base_type() {}

        /**
         *  @brief  Construct an empty string using allocator @a al.
         */
        explicit basic_string(const _Alloc& al) : base_type(al) {}


        /**
         *  @brief  Construct string with copy of value of @a other.
         *  @param  other  Source string.
         */
        basic_string(const basic_string& s) :
            base_type()
        {
            assign(s.data(), s.size());
        }


        /**
         *  @brief  Construct string with copy of value of @a other.
         *  @param  other  Source string.
         */
        template<class _TraitsT, class _AllocT, size_t _NTagBits, size_t _NSizeBits>
        basic_string(const basic_string<_Char, _TraitsT, _AllocT, _NTagBits, _NSizeBits>& s) :
            base_type()
        {
            assign(s.data(), s.size());
        }

        /**
         *  @brief  Move construct string.
         *  @param  other  Source string.
         *
         *  The newly-created string contains the exact contents of @a other.
         *  @a other is a valid, but unspecified string.
         */
        basic_string(basic_string&& other) noexcept
            : base_type(std::move(base_type::get_allocator()))
        {
            this->__m_data.storage = other.__m_data.storage;
            other.__m_data.storage = 0;
        }


        /**
         *  @brief  Construct string with copy of value of @a other.
         *  @param s Source string
         */
        template<class _TraitsT, class _AllocT>
        basic_string(const std::basic_string<_Char, _TraitsT, _AllocT>& other) :
            base_type()
        {
            assign(other.data(), other.size());
        }


        /**
         *  @brief  Construct string as copy of a C string.
         *  @param  s   Source C string.
         *  @param  al  Allocator to use (default is default allocator).
         */
        basic_string(const_pointer s, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
            assign(s, traits_type::length(s));
        }


        /**
         *  @brief  Construct string initialized by a character %array.
         *  @param  s   Source character %array.
         *  @param  n   Number of characters to copy.
         *  @param  al  Allocator to use (default is default allocator).
         *
         *  \warning @a s must have at least @a n characters
         */
        explicit basic_string(const_pointer s, size_t n, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
            assign(s, n);
        }


        /**
         *  @brief  Construct string as multiple characters.
         *  @param  n   Number of characters.
         *  @param  c   Character to use.
         *  @param  al  Allocator to use (default is default allocator).
         */
        explicit basic_string(size_t n, value_type c, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
            assign(n, c);
        }

        /**
         *  @brief  Construct string from an initializer %list.
         *  @param  ilist  std::initializer_list of characters.
         *  @param  al  Allocator to use (default is default allocator).
         */
        basic_string(std::initializer_list<_Char> ilist, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
            assign(ilist);
        }

        template<class _InIt>
        basic_string(_InIt first, _InIt last, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
            assign(first, last);
        }


        /**
         *  @brief  Destroy the string instance.
         */
        ~basic_string() {
            clear();
        }

        /**
         *  @brief  Assign the value of @a other to this string.
         *  @param  other  Source string.
         */
        basic_string& operator= (const basic_string& other)
        {
            return assign(other);
        }

        /**
         *  @brief  Assign the value of @a other to this string.
         *  @param  other  Source string.
         */
        template<class _TraitsT, class _AllocT, size_t _NTagBits, size_t _NSizeBits>
        basic_string& operator= (const basic_string<_Char, _TraitsT, _AllocT, _NTagBits, _NSizeBits>& other)
        {
            return assign(other);
        }

        /**
         *  @brief  Assign the value of @a other to this string.
         *  @param  __str  Source string.
         */
        template<class _TraitsT, class _AllocT>
        basic_string& operator= (std::basic_string<_Char, _TraitsT, _AllocT>& other)
        {
            return assign(other);
        }

        /**
         *  @brief  Copy contents of @a other into this string.
         *  @param  other  Source null-terminated string.
         *
         * \note @a other must be null-terminated
         */
        template<class _TraitsT, class _AllocT>
        basic_string& operator= (const_pointer other)
        {
            return assign(other);
        }

        /**
         *  @brief  Move assign the value of @a other to this string.
         *  @param  other  Source string.
         *
         *  The contents of @a other are moved into this string (without copying).
         *  @a other is a valid, but unspecified string.
         **/
        basic_string& operator= (basic_string&& other)
        {
            this->swap(other);
            return (*this);
        }


        /**
         * @brief  swap Swap this string with @ other
         * @param  other Source string to swap with
         */
        void swap(basic_string& other)
        {
            if (this == &other)
                return;
            std::swap(this->__m_data.storage, other.__m_data.storage);
        }


        basic_string& assign(size_t n, value_type c) {
            this->__assign(n, c);
            return (*this);
        }

        basic_string& assign(const_pointer s, size_t n) {
            this->__assign(s, n);
            return (*this);
        }

        basic_string& assign(const_pointer s) {
            this->__assign(s, traits_type::length(s));
            return (*this);
        }

        template<class _TraitsT, class _AllocT, size_t _NTagBits, size_t _NSizeBits>
        basic_string& assign(const basic_string<_Char, _TraitsT, _AllocT, _NSizeBits, _NTagBits>& other)
        {
            if (&other == static_cast<void*>(this))
                return (*this); // escape self-assignment
            this->__assign(other.data(), other.size());
            return (*this);
        }

        template<class _TraitsT, class _AllocT>
        basic_string& assign(const std::basic_string<_Char, _TraitsT, _AllocT>& s) {
            this->__assign(s.data(), s.size());
            return (*this);
        }


        basic_string& assign(std::initializer_list<_Char> ilist) {
            this->__assign(ilist.begin(), ilist.size());
            return (*this);
        }

        template<class _InIt>
        void assign(_InIt first, _InIt last)
        {
            typedef typename std::iterator_traits<_InIt>::iterator_category _IterCat;
            this->__assign_range(first, last, _IterCat());
        }


        basic_string& append(size_t n, value_type c) {
            this->__insert(this->size(), n, c);
            return (*this);
        }

        basic_string& append(const_pointer s, size_t n) {
            this->__insert(this->size(), s, n);
            return (*this);
        }

        basic_string& append(const_pointer s) {
            this->__insert(this->size(), s, traits_type::length(s));
            return (*this);
        }

        basic_string& append(const basic_string& s) {
            this->__insert(this->size(), s.data(), s.size());
            return (*this);
        }

        template<class _TraitsT, class _AllocT>
        basic_string& append(const std::basic_string<_Char, _TraitsT, _AllocT>& s) {
            this->__insert(this->size(), s.data(), s.size());
            return (*this);
        }

        basic_string& append(std::initializer_list<_Char> ilist) {
            this->__insert(this->size(), ilist.begin(), ilist.size());
            return (*this);
        }

        void clear() {
            this->__clear();
        }

        void resize(size_t n)
        {
            const size_type s = this->size();
            if (s < n)
                this->__insert(s, n - s, _Char());
            else if (n < s)
                this->__erase(n, s - n);
        }

        void resize(size_t n, _Char c)
        {
            const size_type s = this->size();
            if (s < n)
                this->__insert(s, n - s, c);
            else if (n < s)
                this->__erase(n, s - n);
        }

        /**
         *  @brief  Remove characters.
         *  @param pos  Index of first character to remove (default 0).
         *  @param count  Number of characters to remove (default remainder).
         *  @return  Reference to this string.
         *  @throw  std::out_of_range  If @a pos is beyond the end of this
         *  string.
         *
         *  Removes @a count characters from this string starting at @a
         *  pos.  The length of the string is reduced by @a count.  If
         *  there are < @a count characters to remove, the remainder of
         *  the string is truncated.  If @a pos is beyond end of string,
         *  out_of_range is thrown.  The value of the string doesn't
         *  change if an error is thrown.
         */
        basic_string& erase( size_type pos = 0, size_type count = npos )
        {
            assert(pos <= this->size());
            this->__erase(pos, (std::min)(count, static_cast<size_type>(this->size() - pos)) );
            return (*this);
        }

        /**
         *  @brief  Remove one character.
         *  @param __position  Iterator referencing the character to remove.
         *  @return  iterator referencing same location after removal.
         *
         *  Removes the character at @a __position from this string. The value
         *  of the string doesn't change if an error is thrown.
         */
        iterator erase(const_iterator where) {
            assert(where >= begin() && where < end());
            const std::ptrdiff_t __pos = where - begin();
            this->__erase(__pos, size_type(1));
            return (this->data() + __pos);
        }

        /**
         *  @brief  Remove a range of characters.
         *  @param __first  Iterator referencing the first character to remove.
         *  @param __last  Iterator referencing the end of the range.
         *  @return  Iterator referencing location of first after removal.
         *
         *  Removes the characters in the range [first,last) from this string.
         *  The value of the string doesn't change if an error is thrown.
         */
        iterator erase(const_iterator first, const_iterator last)
        {
            assert(first >= begin() && first <= last && last <= end());
            const std::ptrdiff_t __pos = first - begin();
            this->__erase(__pos, last - first);
            return (this->data() + __pos);
        }



        void push_back(value_type c) {
            this->__insert(this->size(), 1, c);
        }

        void pop_back()
        {
            if (empty())
                return;
            this->__erase(this->size(), 1);
        }


        basic_string& insert( size_type pos, size_type count, value_type ch ) {
            assert(pos <= this->size());
            this->__insert(pos, count, ch);
            return (*this);
        }

        basic_string& insert( size_type pos, const_pointer s, size_type count ) {
            assert(pos <= this->size());
            this->__insert(pos, s, count);
            return (*this);
        }

        basic_string& insert( size_type pos, const_pointer s ) {
            assert(pos <= this->size());
            this->__insert(pos, s, traits_type::length(s));
            return (*this);
        }

        basic_string& insert( size_type pos, const basic_string& str ) {
            assert(pos <= this->size());
            this->__insert(pos, str.data(), str.size());
            return (*this);
        }

        template<class _TraitsT, class _AllocT>
        basic_string& insert( size_type pos, const std::basic_string<_TraitsT, _AllocT>& str ) {
            assert(pos <= this->size());
            this->__insert(pos, str.data(), str.size());
            return (*this);
        }


        reference front() { return (this->data()[0]); }
        reference back()  { return (this->data()[this->size() - 1]); }
        const_reference front() const { return (this->data()[0]); }
        const_reference back() const  { return (this->data()[this->size() - 1]); }

        reference at(size_t i) {
            if (i >= this->size())
                throw std::out_of_range("index is out of range");
            return (this->data()[i]);
        }

        const_reference at(size_t i) const {
            if (i >= this->size())
                throw std::out_of_range("index is out of range");
            return (this->data()[i]);
        }

        reference operator[](size_t i) { return (this->data()[i]); }
        const_reference operator[](size_t i) const { return (this->data()[i]); }

        iterator begin() { return this->data(); }
        iterator end() { return (this->data() + this->size()); }

        const_iterator begin() const { return this->data(); }
        const_iterator end() const { return (this->data() + this->size()); }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }

        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        bool empty() const { return (this->size() == 0); }

        template<typename _AllocT = std::allocator<_Char> >
        std::basic_string<_Char, _Traits, _AllocT>
            substr(size_t pos, size_t n = npos, const _AllocT& al = _Alloc()) const
        {
            typedef std::basic_string<_Char, _Traits, _AllocT> string_type;
            (void)al;
            return string_type(this->data() + pos, (n == string_type::npos ? this->size() - pos : n), al);
        }

        template<typename _AllocT = std::allocator<_Char> >
        std::basic_string<_Char, _Traits, _AllocT>
            to_string(const _AllocT& al = _AllocT()) const
        {
            (void)al;
            return std::basic_string<_Char, _Traits, _AllocT>(this->data(), this->size(), al);
        }

        operator stdx::basic_string_view<_Char, _Traits>() const noexcept {
            return { this->data(), this->size() };
        }

        inline size_t hash_code() const {
            return std::_Hash_bytes(this->data(), this->size() * sizeof(_Char), 0xc70f6907UL);
        }



        basic_string& operator+= (value_type c) {
            this->push_back(c);
            return (*this);
        }

        basic_string& operator+= (const_pointer s) {
            this->append(s, traits_type::length(s));
            return (*this);
        }

        template<size_t _NSizeBits, size_t _NTagBits>
        basic_string& operator+= (const basic_string<_Char, _Traits, _Alloc, _NSizeBits, _NTagBits>& s) {
            this->append(s.data(), s.size());
            return (*this);
        }

        template<class _TraitsT, class _AllocT>
        basic_string& operator+= (const std::basic_string<_Char, _TraitsT, _AllocT>& s) {
            this->append(s.data(), s.size());
            return (*this);
        }


        int compare(const_pointer s, size_t n) const
        {
            const size_t __size = this->size();
            const size_t __len = std::min(__size, n);
            int __r = traits_type::compare(this->data(), s, __len);
            if (!__r) {
                __r = this->__compare(__size, n);
            }
            return __r;
        }

        int compare(const_pointer s) const {
            return compare(s, traits_type::length(s));
        }

        template<class _AllocT, size_t _NSizeBits, size_t _NTagBits>
        int compare(const basic_string<_Char, _Traits, _AllocT, _NSizeBits, _NTagBits>& other) const {
            if (&other == static_cast<const void*>(this)) return 0;
            return compare(other.data(), other.size());
        }

        template<class _AllocT>
        int compare(const std::basic_string<_Char, _Traits, _AllocT>& other) const {
            return compare(other.data(), other.size());
        }




        /**
         *  @brief  Concatenate C string and string.
         *  @param __lhs  First string.
         *  @param __rhs  Last string.
         *  @return  New string with value of @a __lhs followed by @a __rhs.
         */
        friend basic_string operator+ (const _Char* __lhs, const basic_string& __rhs)
        {
            basic_string __str;

            size_t __len = _Traits::length(__lhs);
            size_t __size = __len + __rhs.size();
            __str.__expand(__size);
            _Char* __p = __str.data();
            _Traits::move(__p, __lhs, __len);
            _Traits::move(__p + __len, __rhs.data(), __rhs.size());
            __p[__size] = _Char(); // allways null-terminated
            return __str;
        }

        /**
         *  @brief  Concatenate character and string.
         *  @param __lhs  First string.
         *  @param __rhs  Last string.
         *  @return  New string with @a __lhs followed by @a __rhs.
         */
        friend basic_string operator+ (_Char __lhs, const basic_string& __rhs)
        {
            basic_string __str;
            size_t __size = __rhs.size() + 1;
            __str.__expand(__size);
            _Char* __p = __str.data();
            *__p = __lhs;
            _Traits::move(__p + 1, __rhs.data(), __rhs.size());
            __p[__size] = _Char(); // allways null-terminated
            return __str;
        }



        friend inline std::basic_ostream<_Char, _Traits>& operator<< (std::basic_ostream<_Char, _Traits>& stream, 
                                                                      const basic_string& s) {
            return stream.write(s.data(), s.size());
        }

        friend inline std::basic_istream<_Char, _Traits>& operator>> (std::basic_istream<_Char, _Traits>& stream, 
                                                                      basic_string& s)
        {
            std::basic_string<_Char, _Traits> str;
            stream >> str;
            if (stream) {
                s.assign(str);
            }
            return stream;
        }

    };





    // operator+
    /**
     *  @brief  Concatenate two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Last string.
     *  @return  New string with value of @a __lhs followed by @a __rhs.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                  const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    {
        basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> __str(__lhs);
        __str.append(__rhs);
        return __str;
    }

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                  const std::basic_string<_Char, _Traits, _Alloc>& __rhs)
    {
        basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> __str(__lhs);
        __str.append(__rhs);
        return __str;
    }


    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(const std::basic_string<_Char, _Traits, _Alloc>& __lhs,
                  const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    {
        basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> __str(__lhs);
        __str.append(__rhs);
        return __str;
    }


    /**
     *  @brief  Concatenate string and C string.
     *  @param __lhs  First string.
     *  @param __rhs  Last string.
     *  @return  New string with @a __lhs followed by @a __rhs.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                  const _Char* __rhs)
    {
        basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> __str(__lhs);
        __str.append(__rhs);
        return __str;
    }



    /**
     *  @brief  Concatenate string and character.
     *  @param __lhs  First string.
     *  @param __rhs  Last string.
     *  @return  New string with @a __lhs followed by @a __rhs.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs, _Char __rhs)
    {
      basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> __str(__lhs);
      __str.append(size_t(1), __rhs);
      return __str;
    }


//#if __cplusplus >= 201103L
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+ (basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>&& __lhs,
                   const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    { return std::move(__lhs.append(__rhs)); }

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
      operator+ (const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                 basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>&& __rhs)
    { return std::move(__rhs.insert(0, __lhs)); }

    /*
    FIXME: dont'n know how to implement this...
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits
        size_t _SizeBits,
    >
    inline basic_string<_CharT, _Traits, _Alloc, _TagBits, _SizeBits>
      operator+(basic_string<_CharT, _Traits, _Alloc, _TagBits, _SizeBits>&& __lhs,
                basic_string<_CharT, _Traits, _Alloc, _TagBits, _SizeBits>&& __rhs)
    {
        const auto __size = __lhs.size() + __rhs.size();
        const bool __cond = (__size > __lhs.capacity() && __size <= __rhs.capacity());
        return __cond ? std::move(__rhs.insert(0, __lhs))
                  : std::move(__lhs.append(__rhs));
    }*/

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(const _Char* __lhs,
                basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>&& __rhs)
    { return std::move(__rhs.insert(0, __lhs)); }


    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(_Char __lhs,
                  basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>&& __rhs)
    { return std::move(__rhs.insert(0, 1, __lhs)); }

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>&& __lhs,
                  const _Char* __rhs)
    { return std::move(__lhs.append(__rhs)); }

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>
        operator+(basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>&& __lhs,
                  _Char __rhs)
    { return std::move(__lhs.append(1, __rhs)); }
//#endif


    // operator ==
    /**
     *  @brief  Test equivalence of two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs.compare(@a __rhs) == 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits1,
        size_t _SizeBits1,
        size_t _TagBits2,
        size_t _SizeBits2
    >
    inline bool operator==(const basic_string<_Char, _Traits, _Alloc1, _TagBits1, _SizeBits1>& __lhs,
                           const basic_string<_Char, _Traits, _Alloc2, _TagBits2, _SizeBits2>& __rhs)
    { return (__lhs.compare(__rhs) == 0); }


    // operator ==
    /**
     *  @brief  Test equivalence of two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs.compare(@a __rhs) == 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator==(const basic_string<_Char, _Traits, _Alloc1, _TagBits, _SizeBits>& __lhs,
                           const std::basic_string<_Char, _Traits, _Alloc2>& __rhs)
    { return (__lhs.compare(__rhs) == 0); }



    // operator ==
    /**
     *  @brief  Test equivalence of two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs.compare(@a __rhs) == 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator==(const std::basic_string<_Char, _Traits, _Alloc1>& __lhs,
                           const basic_string<_Char, _Traits, _Alloc2, _TagBits, _SizeBits>& __rhs)
    { return (__rhs.compare(__lhs) == 0); }


    /**
     *  @brief  Test equivalence of C string and string.
     *  @param __lhs  C string.
     *  @param __rhs  String.
     *  @return  True if @a __rhs.compare(@a __lhs) == 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator==(const _Char* __lhs,
                           const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    { return (__rhs.compare(__lhs) == 0); }

    /**
     *  @brief  Test equivalence of string and C string.
     *  @param __lhs  String.
     *  @param __rhs  C string.
     *  @return  True if @a __lhs.compare(@a __rhs) == 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator==(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                           const _Char* __rhs)
    { return (__lhs.compare(__rhs) == 0); }



    // operator !=
    /**
     *  @brief  Test difference of two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs.compare(@a __rhs) != 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits1,
        size_t _SizeBits1,
        size_t _TagBits2,
        size_t _SizeBits2
    >
    inline bool operator!=(const basic_string<_Char, _Traits, _Alloc1, _TagBits1, _SizeBits1>& __lhs,
                           const basic_string<_Char, _Traits, _Alloc2, _TagBits2, _SizeBits2>& __rhs)
    { return !(__lhs == __rhs); }


    /**
     *  @brief  Test difference of two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs.compare(@a __rhs) != 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator!=(const basic_string<_Char, _Traits, _Alloc1, _TagBits, _SizeBits>& __lhs,
                           const std::basic_string<_Char, _Traits, _Alloc2>& __rhs)
    {
        return !(__lhs == __rhs);
    }

    /**
     *  @brief  Test difference of two strings.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs.compare(@a __rhs) != 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator!=(const std::basic_string<_Char, _Traits, _Alloc1>& __lhs,
                           const basic_string<_Char, _Traits, _Alloc2, _TagBits, _SizeBits>& __rhs)
    {
        return !(__lhs == __rhs);
    }

    /**
     *  @brief  Test difference of C string and string.
     *  @param __lhs  C string.
     *  @param __rhs  String.
     *  @return  True if @a __rhs.compare(@a __lhs) != 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator!=(const _Char* __lhs,
                           const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    { return !(__lhs == __rhs); }

    /**
     *  @brief  Test difference of string and C string.
     *  @param __lhs  String.
     *  @param __rhs  C string.
     *  @return  True if @a __lhs.compare(@a __rhs) != 0.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator!=(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                           const _Char* __rhs)
    { return !(__lhs == __rhs); }



    // operator <
    /**
     *  @brief  Test if string precedes string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits1,
        size_t _SizeBits1,
        size_t _TagBits2,
        size_t _SizeBits2
    >
    inline bool operator<(const basic_string<_Char, _Traits, _Alloc1, _TagBits1, _SizeBits1>& __lhs,
                          const basic_string<_Char, _Traits, _Alloc2, _TagBits2, _SizeBits2>& __rhs)
    { return (__lhs.compare(__rhs) < 0); }


    /**
     *  @brief  Test if string precedes string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator<(const basic_string<_Char, _Traits, _Alloc1, _TagBits, _SizeBits>& __lhs,
                          const std::basic_string<_Char, _Traits, _Alloc2>& __rhs)
    { return (__lhs.compare(__rhs) < 0); }


    /**
     *  @brief  Test if string precedes string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator<(const std::basic_string<_Char, _Traits, _Alloc1>& __lhs,
                          const basic_string<_Char, _Traits, _Alloc2, _TagBits, _SizeBits>& __rhs)
    { return (__rhs.compare(__lhs) > 0); }

    /**
     *  @brief  Test if string precedes C string.
     *  @param __lhs  String.
     *  @param __rhs  C string.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator<(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                          const _Char* __rhs)
    { return (__lhs.compare(__rhs) < 0); }

    /**
     *  @brief  Test if C string precedes string.
     *  @param __lhs  C string.
     *  @param __rhs  String.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator<(const _Char* __lhs,
                          const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    { return (__rhs.compare(__lhs) > 0); }




    // operator >
    /**
     *  @brief  Test if string follows string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs follows @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits1,
        size_t _SizeBits1,
        size_t _TagBits2,
        size_t _SizeBits2
    >
    inline bool operator>(const basic_string<_Char, _Traits, _Alloc1, _TagBits1, _SizeBits1>& __lhs,
                          const basic_string<_Char, _Traits, _Alloc2, _TagBits2, _SizeBits2>& __rhs)
    { return (__lhs.compare(__rhs) > 0); }


    /**
     *  @brief  Test if string follows string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator>(const basic_string<_Char, _Traits, _Alloc1, _TagBits, _SizeBits>& __lhs,
                          const std::basic_string<_Char, _Traits, _Alloc2>& __rhs)
    { return (__lhs.compare(__rhs) > 0); }


    /**
     *  @brief  Test if string follows string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs precedes @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator>(const std::basic_string<_Char, _Traits, _Alloc1>& __lhs,
                          const basic_string<_Char, _Traits, _Alloc2, _TagBits, _SizeBits>& __rhs)
    { return (__rhs.compare(__lhs) < 0); }

    /**
     *  @brief  Test if string follows C string.
     *  @param __lhs  String.
     *  @param __rhs  C string.
     *  @return  True if @a __lhs follows @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator>(const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __lhs,
                          const _Char* __rhs)
    { return (__lhs.compare(__rhs) > 0); }

    /**
     *  @brief  Test if C string follows string.
     *  @param __lhs  C string.
     *  @param __rhs  String.
     *  @return  True if @a __lhs follows @a __rhs.  False otherwise.
     */
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator>(const _Char* __lhs,
                          const basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& __rhs)
    { return (__rhs.compare(__lhs) < 0); }

#if 0
    // operator <=
    /**
     *  @brief  Test if string doesn't follow string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs doesn't follow @a __rhs.  False otherwise.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      inline bool
      operator<=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
             const basic_string<_CharT, _Traits, _Alloc>& __rhs)
      { return __lhs.compare(__rhs) <= 0; }

    /**
     *  @brief  Test if string doesn't follow C string.
     *  @param __lhs  String.
     *  @param __rhs  C string.
     *  @return  True if @a __lhs doesn't follow @a __rhs.  False otherwise.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      inline bool
      operator<=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
             const _CharT* __rhs)
      { return __lhs.compare(__rhs) <= 0; }

    /**
     *  @brief  Test if C string doesn't follow string.
     *  @param __lhs  C string.
     *  @param __rhs  String.
     *  @return  True if @a __lhs doesn't follow @a __rhs.  False otherwise.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      inline bool
      operator<=(const _CharT* __lhs,
             const basic_string<_CharT, _Traits, _Alloc>& __rhs)
      { return __rhs.compare(__lhs) >= 0; }

    // operator >=
    /**
     *  @brief  Test if string doesn't precede string.
     *  @param __lhs  First string.
     *  @param __rhs  Second string.
     *  @return  True if @a __lhs doesn't precede @a __rhs.  False otherwise.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      inline bool
      operator>=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
             const basic_string<_CharT, _Traits, _Alloc>& __rhs)
      { return __lhs.compare(__rhs) >= 0; }

    /**
     *  @brief  Test if string doesn't precede C string.
     *  @param __lhs  String.
     *  @param __rhs  C string.
     *  @return  True if @a __lhs doesn't precede @a __rhs.  False otherwise.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      inline bool
      operator>=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
             const _CharT* __rhs)
      { return __lhs.compare(__rhs) >= 0; }

    /**
     *  @brief  Test if C string doesn't precede string.
     *  @param __lhs  C string.
     *  @param __rhs  String.
     *  @return  True if @a __lhs doesn't precede @a __rhs.  False otherwise.
     */
    template<typename _CharT, typename _Traits, typename _Alloc>
      inline bool
      operator>=(const _CharT* __lhs,
           const basic_string<_CharT, _Traits, _Alloc>& __rhs)
      { return __rhs.compare(__lhs) <= 0; }
#endif


    typedef basic_string< char, std::char_traits<char>, std::allocator<char> > string;
    typedef basic_string< wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > wstring;

    template<size_t _TagBits>
    using tagged_string = basic_string< char, std::char_traits<char>, std::allocator<char>, _TagBits>;

    template<size_t _TagBits>
    using tagged_wstring = basic_string< wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>, _TagBits>;

} // end namespace compact



namespace std
{
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    struct hash< compact::basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> >
    {
        typedef compact::basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits> argument_type;
        typedef size_t result_type;
        inline size_t operator()(const argument_type& s) const {
            return s.hash_code();
        }
    };

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    void swap(compact::basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& lhs,
              compact::basic_string<_Char, _Traits, _Alloc, _TagBits, _SizeBits>& rhs) {
        lhs.swap(rhs);
    }

}// end namespace std


