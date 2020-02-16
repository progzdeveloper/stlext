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

#include <vector>
#include <algorithm>

#include "storages.hpp"

/*
* This is usefull to simplify crazy C++
* argument dedution rules
*/
#define _INPUT_ITERATOR_REQUIRED(_InIt)                                 \
    typename = typename std::enable_if<                                 \
            std::is_base_of<                                            \
                std::input_iterator_tag,                                \
                typename std::iterator_traits<_InIt>::iterator_category \
            >::value                                                    \
    >::type


namespace compact
{

    template<
        class T,
        class _Alloc = std::allocator<T>,
        size_t _TagBits = 0,
        size_t _SizeBits = 16 - _TagBits
    >
    class vector :
        public basic_vector<T, _SizeBits, _TagBits, _Alloc >
    {
        typedef basic_vector<T, _SizeBits, _TagBits, _Alloc> base_type;
    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef pointer iterator;
        typedef const_pointer const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_pointer> const_reverse_iterator;
        typedef uint16_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef std::allocator<T> allocator_type;

        vector() :
            base_type()
        {
        }

        explicit vector(size_t n,
                        const allocator_type& al = allocator_type()) :
            base_type(al)
        {
             assign(n, value_type());
        }

        explicit vector(size_t n, const_reference v,
                        const allocator_type& al = allocator_type()) :
            base_type(al)
        {
            assign(n, v);
        }

        template<
            class _InIt,
            _INPUT_ITERATOR_REQUIRED(_InIt)
        >
        vector(_InIt first, _InIt last,
               const allocator_type& al = allocator_type()) :
            base_type(al)
        {
            assign(first, last);
        }
        
        template<class _AllocT>
        vector(const std::vector<T, _AllocT>& other,
               const _Alloc& al = allocator_type()) :
            base_type(al)
        {
            assign(other);
        }

        vector(std::initializer_list<T> ilist, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
             assign(ilist);
        }

        vector(const vector& other) :
            base_type(other.get_allocator())
        {
             assign(other);
        }

        vector(const vector& other, const allocator_type& al) :
            base_type(al)
        {
             assign(other);
        }

        vector(vector&& other) noexcept :
            base_type(std::move(base_type::get_allocator()))
        {
            this->__m_data.storage = other.__m_data.storage;
            other.__m_data.storage = 0;
        }


        template<class _AllocT, size_t _NTagBits, size_t _NSizeBits>
        vector(const vector<T, _AllocT, _NTagBits, _NSizeBits>& other, const _Alloc& al = _Alloc()) :
            base_type(al)
        {
            assign(other);
        }


        vector& operator=(const vector& other)
        {
            assign(other);
            return (*this);
        }

        vector& operator=(vector&& other)
        {
            this->swap(other);
            return (*this);
        }

        template<class _AllocT, size_t _NTagBits, size_t _NSizeBits>
        vector& operator=(const vector<T, _AllocT, _NTagBits, _NSizeBits>& other)
        {
            assign(other);
            return (*this);
        }

        vector& operator=(std::initializer_list<T> ilist) {
            assign(ilist);
            return (*this);
        }

        template<class _AllocT>
        vector& operator=(const std::vector<T, _AllocT>& other) {
            assign(other);
            return (*this);
        }

        ~vector() {
            this->__clear();
        }

        void assign(size_t count, const_reference value)
        {
            this->__assign(value, count);
        }

        void assign(std::initializer_list<T> ilist)
        {
            this->__assign(ilist.begin(), ilist.size());
        }

        template<class _AllocT, size_t _NTagBits, size_t _NSizeBits>
        void assign(const vector<T, _AllocT, _NTagBits, _NSizeBits>& other)
        {
            if (static_cast<const void*>(&other) != static_cast<const void*>(this))  // escape self-assignment
                this->__assign(other.data(), other.size());
        }

        template<class _AllocT>
        void assign(const std::vector<T, _AllocT>& other)
        {
            this->__assign(other.data(), other.size());
        }

        template<
            class _InIt,
            _INPUT_ITERATOR_REQUIRED(_InIt)
        >
        void assign(_InIt first, _InIt last)
        {
            typedef typename std::iterator_traits<_InIt>::iterator_category _IterCat;
            this->__assign_range(first, last, _IterCat());
        }


        void swap(vector& other) noexcept
        {
            if (this == &other)
                return;
            std::swap(other.__m_data, this->__m_data);
        }

        reference front() { return (*(this->data())); }
        reference back()  { return (*(this->data() + (this->size() - 1))); }
        const_reference front() const { return (*(this->data())); }
        const_reference back() const  { return (*(this->data() + (this->size() - 1))); }

        reference at(size_t i)
        {
            if (i >= this->size())
                throw std::out_of_range("compact::vector<T>: index out of range");
            return (this->data()[i]);
        }
        const_reference at(size_t i) const
        {
            if (i >= this->size())
                throw std::out_of_range("compact::vector<T>: index out of range");
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

        void clear() { this->__clear(); }

        iterator erase(const_iterator where) {
            assert(where >= begin() && where < end());
            return this->__erase(where, 1);
        }

        iterator erase(const_iterator first, const_iterator last) {
            assert(first >= begin() && first <= last && last <= end());
            return this->__erase(first, last - first);
        }

        iterator insert(const_iterator where, const_reference v) {
            assert(where >= begin() && where <= end());
            return this->__insert(where, v, 1);
        }

        iterator insert(const_iterator where, T&& v) {
            assert(where >= begin() && where <= end());
            return this->__insert(where, std::move(v), 1);
        }

        iterator insert(const_iterator where, size_type count, const_reference v) {
            assert(where >= begin() && where <= end());
            return this->__insert(where, v, count);
        }

        iterator insert(const_iterator where, std::initializer_list<T> ilist) {
            assert(where >= begin() && where <= end());
            return this->__insert(where, ilist.begin(), ilist.size());
        }

        iterator insert(const_iterator where, const_iterator first, const_iterator last) {
            assert(where >= begin() && where <= end());
            return this->__insert(where, first, std::distance(first, last));
        }

        void resize(size_t n)
        {
            const size_type s = this->size();
            if (s < n)
                this->__insert(end(), value_type(), n - s);
            else if (n < s)
                this->__erase(begin() + n, s - n);
        }

        void resize(size_t n, const_reference v)
        {
            const size_type s = this->size();
            if (s < n)
                this->__insert(end(), v, n - s);
            else if (n < s)
                this->__erase(begin() + n, s - n);
        }

        void push_back(const_reference x)
        {
            this->__insert(end(), x, 1);
        }

        template<class... _Args>
        void emplace_back(_Args&&... args)
        {
            size_t offset = this->size();
            this->__expand(1);
            new (this->data() + offset) value_type(std::forward<_Args>(args)...);
        }

        void pop_back()
        {
            if (empty())
                return;
            this->__erase(end() - 1, 1);
        }

    };



    template<
        class T,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits1,
        size_t _SizeBits1,
        size_t _TagBits2,
        size_t _SizeBits2
    >
    inline bool operator== (const vector<T, _Alloc1, _TagBits1, _SizeBits1>& lhs,
                            const vector<T, _Alloc2, _TagBits2, _SizeBits2>& rhs)
    {
        if (static_cast<const void*>(&lhs) == static_cast<const void*>(&rhs))
            return true;
        return (lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
    }


    template<
        class T,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator== (const vector<T, _Alloc1, _TagBits, _SizeBits>& lhs,
                            const std::vector<T, _Alloc2>& rhs)
    {
        return (lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
    }


    template<
        class T,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator== (const std::vector<T, _Alloc1>& lhs,
                            const vector<T, _Alloc2, _TagBits, _SizeBits>& rhs)
    {
        return (lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
    }



    template<
        class T,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits1,
        size_t _SizeBits1,
        size_t _TagBits2,
        size_t _SizeBits2
    >
    inline bool operator!= (const vector<T, _Alloc1, _TagBits1, _SizeBits1>& lhs,
                            const vector<T, _Alloc2, _TagBits2, _SizeBits2>& rhs)
    {
        return !(lhs == rhs);
    }


    template<
        class T,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator!= (const vector<T, _Alloc1, _TagBits, _SizeBits>& lhs,
                            const std::vector<T, _Alloc2>& rhs)
    {
        return !(lhs == rhs);
    }


    template<
        class T,
        class _Alloc1,
        class _Alloc2,
        size_t _TagBits,
        size_t _SizeBits
    >
    inline bool operator!= (const std::vector<T, _Alloc1>& lhs,
                            const vector<T, _Alloc2, _TagBits, _SizeBits>& rhs)
    {
        return !(lhs == rhs);
    }



    template<class T, size_t _TagBits>
    using tagged_vector = vector< T, std::allocator<T>, _TagBits >;

} // end namespace compact

#undef _INPUT_ITERATOR_REQUIRED 


namespace std
{
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _TagBits,
        size_t _SizeBits
    >
    void swap(compact::vector<_Char, _Alloc, _TagBits, _SizeBits>& lhs,
              compact::vector<_Char, _Alloc, _TagBits, _SizeBits>& rhs) {
        lhs.swap(rhs);
    }
} // end namespace std


