// Copyright (c) 2016, Michael Polukarov (Russia).
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

#include <cstring>
#include <climits>

#include <vector>
#include <string>

#include <stdexcept>

//#include "../compability/cxx17/basic_string_view.hpp"
#include <experimental/string_view>

#include "../platform/common.h"


_STDX_BEGIN

template<
    class _Char = char,
    class _Length = uint16_t,
    class _Traits = std::char_traits<_Char>,
    class _Alloc = std::allocator<_Char>
>
class basic_stringset
{
private:
    struct node
    {
        std::ptrdiff_t offset;
        size_t hash_code;

        node() :
            offset(-1), hash_code(0) {
        }

        node(size_t i, size_t h) :
            offset(i), hash_code(h) {
        }
    };

    typedef std::basic_string<
        char,
        std::char_traits<char>,
        _Alloc
    > buffer_type;

    typedef std::vector<
        node,
        _Alloc
    > bucket_map;

    typedef const char* raw_pointer;

public:
    typedef size_t size_type;
    typedef _Length length_type;
    typedef _Char char_type;
    typedef _Char* char_ptr;
    typedef const _Char* const_char_ptr;

    typedef std::basic_string<_Char, _Traits, _Alloc> string_type;

    static constexpr size_t max_length = (1ULL << sizeof(length_type) * CHAR_BIT) - 1;



    class iterator
    {
        friend class basic_stringset;

        //typedef stdx::basic_string_view<_Char, _Traits> view_type;
        typedef std::experimental::basic_string_view<_Char, _Traits> view_type;

        iterator(raw_pointer p, raw_pointer e) :
            __m_end(e)
        {
            assign(p);
        }

        inline iterator& assign(raw_pointer p)
        {
            if (p >= __m_end)
                __m_view = std::move(view_type(reinterpret_cast<const _Char*>(p), 0));
            else {
                __m_view = std::move(view_type(
                    reinterpret_cast<const _Char*>(p + sizeof(length_type)),
                    *reinterpret_cast<const length_type*>(p) / sizeof(_Char)
                 ));
            };
            return (*this);
        }

    public:
        typedef std::forward_iterator_tag iterator_category;

        typedef view_type value_type;
        typedef view_type reference;
        typedef const view_type const_reference;
        typedef view_type* pointer;
        typedef const view_type* const_pointer;

        iterator()  { }

        inline const_reference operator*() const {
            return __m_view;
        }

        inline const_pointer operator->() const {
            return &__m_view;
        }

        inline reference operator*() {
            return __m_view;
        }

        inline pointer operator->() {
            return &__m_view;
        }

        inline iterator& operator++()
        {
            return assign(reinterpret_cast<raw_pointer>(__m_view.data()) +
                          __m_view.size() * sizeof(_Char));
        }

        inline iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        friend inline bool operator== (const iterator& lhs,
                                       const iterator& rhs) {
            return (lhs.__m_view.data() == rhs.__m_view.data());
        }

        friend inline bool operator!= (const iterator& lhs,
                                       const iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        view_type __m_view;
        raw_pointer __m_end;
    };

    basic_stringset() : __m_size(0) {}

    void reserve(size_t n)
    {
        __m_buffer.reserve(n * 8);

        if (__m_size == 0) {
            __m_buckets.resize(n);
            return;
        }

        if ((double)n > (__m_buckets.size() / max_load_factor()))
            __rehash(n);
    }

    void reserve(size_t n, size_t length)
    {
        __m_buffer.reserve(length);

        if (__m_size == 0) {
            __m_buckets.resize(n);
            return;
        }

        if ((double)n > (__m_buckets.size() / max_load_factor()))
            __rehash(n);
    }

    template<class _TTraits, class _TAlloc>
    inline iterator insert(const std::basic_string<_Char, _TTraits, _TAlloc>& s) {
        return insert(s.data(), s.size());
    }

    inline iterator insert(const_char_ptr s) {
        return insert(s, _Traits::length(s));
    }

    iterator insert(const_char_ptr str, size_t len)
    {
        if (len > max_length)
            throw std::runtime_error("string size exceded max length");

        if (len == 0 || str == nullptr)
            return end();

        if (__m_buckets.empty()) {
            __m_buckets.resize(16);
        } else {
            if (load_factor() > max_load_factor()) {
                __rehash(__m_buckets.size() * 2);
            }
        }
        size_t hs = __hash_bytes(str, len * sizeof(_Char));
        const node* where = __find_free(hs, str, len);
        if (where != nullptr)
        {
            length_type n = static_cast<length_type>(len);
            new ((void*)where) node(__m_buffer.size(), hs);
            __m_buffer.append(reinterpret_cast<raw_pointer>(&n), sizeof(n));
            __m_buffer.append(reinterpret_cast<raw_pointer>(str), len * sizeof(_Char));
            ++__m_size;
            return iterator(__m_buffer.data() + where->offset, __m_buffer.data() + (where->offset + len * sizeof(_Char)));
        }
        return end();
    }

    template<class _InIt>
    inline void insert(_InIt first, _InIt last) {
        for (; first != last; ++first)
            insert(*first);
    }


    template<class _TTraits, class _TAlloc>
    iterator find(const std::basic_string<_Char, _TTraits, _TAlloc>& key) const {
        return find(key.data(), key.size());
    }

    iterator find(const_char_ptr key) const {
        return find(key, _Traits::length(key));
    }

    iterator find(const_char_ptr str, size_t len) const
    {
        if (len > max_length)
            return end();

        if (empty() || len == 0 || str == nullptr)
            return end();

        size_t hs = __hash_bytes(str, len * sizeof(_Char));
        const node* p = __find_node(hs, str, len);
        return ((p == nullptr) ? end() : iterator(__m_buffer.data() + p->offset, __m_buffer.data() + __m_buffer.size()));
    }


    template<class _TTraits, class _TAlloc>
    size_t count(const std::basic_string<_Char, _TTraits, _TAlloc>& key) const {
        return count(key.data(), key.size());
    }

    size_t count(const_char_ptr key) const {
        return count(key, _Traits::length(key));
    }

    size_t count(const_char_ptr key, size_t len) const {
        if ((len > max_length || len == 0) || key == nullptr || empty())
            return 0;
        size_t hs = __hash_bytes(key, len * sizeof(_Char));
        return static_cast<size_t>(__find_node(hs, key, len) != nullptr);
    }

    iterator erase(iterator where)
    {
        std::ptrdiff_t offset = ((raw_pointer)where->data() - __m_buffer.data()) - sizeof(length_type);
        size_t size = where->size() + sizeof(length_type);
        for (auto it = __m_buckets.begin(); it != __m_buckets.end(); ++it) {
            if (it->offset == offset) {
                it->offset = -1; // mark as free slot
                continue;
            }
            if (it->offset > offset)
                it->offset -= size;
        }
        auto first = __m_buffer.begin() + offset;
        auto last = first + size;
        auto pos = __m_buffer.erase(first, last);
        --__m_size;
        return iterator(&(*pos), __m_buffer.data() + __m_buffer.size());
    }

    template<class _TTraits, class _TAlloc>
    inline size_t erase(const std::basic_string<_Char, _TTraits, _TAlloc>& key) {
        return erase(key.data(), key.size());
    }

    inline size_t erase(const_char_ptr key) {
        return erase(key, _Traits::length(key));
    }

    size_t erase(const_char_ptr key, size_t size)
    {
        auto pos = find(key, size);
        if (pos != end()) {
            erase(pos);
            return 1;
        }
        return 0;
    }

    void clear()
    {
        __m_buffer.clear();
        __m_buckets.clear();
        __m_size = 0;
    }

    iterator begin() const {
        return iterator(__m_buffer.data(),
                        __m_buffer.data() + __m_buffer.size());
    }

    iterator end() const {
        return iterator(__m_buffer.data() + __m_buffer.size(),
                        __m_buffer.data() + __m_buffer.size());
    }

    double load_factor() const {
        return (__m_size / (double)__m_buckets.size());
    }

    double max_load_factor() const {
        return 0.8;
    }

    bool empty() const { return (__m_size == 0); }

    size_t size() const { return __m_size; }

    size_t capacity() const { return __m_buffer.capacity(); }

    size_t bucket_count() const { return __m_buckets.size(); }

private:
    static inline size_t __hash_bytes(raw_pointer p, size_t n) {
#ifdef STDX_CMPLR_MSVC
        return std::_Hash_seq(reinterpret_cast<const unsigned char*>(p), n);
#else
        return std::_Hash_bytes(p, n, 0xc70f6907UL);
#endif
    }

    void __rehash(size_t n)
    {
        bucket_map temp(n);

        for (auto nodeIt = __m_buckets.cbegin(); nodeIt != __m_buckets.cend(); ++nodeIt)
        {
            if (nodeIt->offset == -1)
                continue;

            size_t offset = nodeIt->hash_code % n;
            node* first = temp.data();
            node* last = temp.data() + n;
            node* current = first + offset;
            node* it = current;
            do
            {
                if (it->offset == -1)
                    break;

                if (++it == last)
                    it = first;
            } while (it != current);
            std::memcpy(it, &(*nodeIt), sizeof(node));
        }
        __m_buckets = std::move(temp);
    }

    const node* __find_node(size_t hs, const _Char* str, size_t len) const
    {
        size_t n = __m_buckets.size();
        size_t offset = hs % n;

        raw_pointer bufptr = __m_buffer.data();
        const node* first = __m_buckets.data();
        const node* last = __m_buckets.data() + n;
        const node* current = first + offset;
        const node* it = current;
        length_type length = 0;
        do
        {
            if (it->offset == -1)
                return nullptr;

            raw_pointer ptr = bufptr + it->offset;
            length = *(reinterpret_cast<const length_type*>(ptr));

            if (length == len &&
                _Traits::compare(ptr + sizeof(length_type), str, len * sizeof(_Char)) == 0)
                    return it;

            if (++it == last)
                it = first;
        } while (it != current);
        return nullptr;
    }

    const node* __find_free(size_t hs, const _Char* str, size_t len) const
    {
        size_t n = __m_buckets.size();
        size_t offset = hs % n;

        raw_pointer bufptr = __m_buffer.data();
        const node* first = __m_buckets.data();
        const node* last = __m_buckets.data() + n;
        const node* current = first + offset;
        const node* it = current;
        length_type length = 0;

        for (size_t i = 0; i < n; i++)
        {
            if (it->offset == -1)
                return it;

            raw_pointer ptr = bufptr + it->offset;
            length = *(reinterpret_cast<const length_type*>(ptr));

            if (length == len)
                if (_Traits::compare(ptr + sizeof(length_type), str, len * sizeof(_Char)) == 0)
                    return nullptr;

            if (++it == last)
                it = first;
        }
        return nullptr;
    }

private:
    buffer_type __m_buffer;
    bucket_map  __m_buckets;
    size_type   __m_size;
};


typedef basic_stringset<char, uint16_t, std::char_traits<char>, std::allocator<char> >            stringset;
typedef basic_stringset<wchar_t, uint16_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >  wstringset;

_STDX_END



