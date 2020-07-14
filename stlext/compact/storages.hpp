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

#include <cassert>
#include <climits>
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <string>

static_assert(sizeof(void*) == 8, "non Intel-x64 platforms is not supported");


namespace compact
{
    static constexpr size_t ptr_mask = 0x0000FFFFFFFFFFFF;
    static constexpr size_t ptr_bits = 48;

    template<
        class _Alloc,
        bool _IsEmpty = std::is_empty<_Alloc>::value
    >
    class alloc_ebo
    {
    public:
        alloc_ebo() {}
        alloc_ebo(const _Alloc& a) : al(a) {}

        _Alloc& get_allocator() { return al; }
        const _Alloc& get_allocator() const { return al; }
    private:
        _Alloc al;
    };


    template<class _Alloc>
    class alloc_ebo<_Alloc, true> :
        public _Alloc
    {
    public:
        alloc_ebo() : _Alloc() {}
        alloc_ebo(const _Alloc& a) : _Alloc(a) {}

        _Alloc& get_allocator() { return (*this); }
        const _Alloc& get_allocator() const { return (*this); }
    };




    template<
        class T,
        size_t _SizeBits,
        size_t _TagBits,
        class _Alloc
    >
    class vector_storage :
        public alloc_ebo<_Alloc>
    {
    protected:

        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
                uint64_t tag  : _TagBits;
            };
            uint64_t storage;
        } __m_data;

    public:
        vector_storage() {
            __m_data.storage = 0;
        }

        explicit vector_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al)
        {
            __m_data.storage = 0;
        }

        bool tag() const { return static_cast<uint16_t>(__m_data.tag); }
        void tag(uint16_t t) { __m_data.tag = t; }
    };

    template<
        class T,
        size_t _SizeBits,
        class _Alloc
    >
    class vector_storage<T, _SizeBits, 1, _Alloc> :
        public alloc_ebo<_Alloc>
    {
    protected:
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
                uint64_t tag : 1;
            };
            uint64_t storage;
        } __m_data;

    public:
        vector_storage() {
            __m_data.storage = 0;
        }

        explicit vector_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al) {
            __m_data.storage = 0;
        }

        bool tag() const { return static_cast<bool>(__m_data.tag); }
        void tag(bool on) { __m_data.tag = on; }
    };


    template<
        class T,
        size_t _SizeBits,
        class _Alloc
    >
    class vector_storage<T, _SizeBits, 0, _Alloc> :
        public alloc_ebo<_Alloc>
    {
    protected:
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
            };
            uint64_t storage;
        } __m_data;

    public:
        vector_storage() {
            __m_data.storage = 0;
        }

        explicit vector_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al) {
            __m_data.storage = 0;
        }
    };



    template<
        class T,
        size_t _SizeBits,
        size_t _TagBits,
        class _Alloc
    >
    class basic_vector :
            public vector_storage<T, _SizeBits, _TagBits, _Alloc>
    {
        typedef vector_storage<T, _SizeBits, _TagBits, _Alloc> storage_type;
        static_assert(_SizeBits > 0, "size bits cannot be 0");
        static_assert(_SizeBits + _TagBits <= 16, "incorrect number of bits for size and tag");


    protected:

        T* __allocate(size_t n) {
            return this->get_allocator().allocate(n);
        }

        void __deallocate(T* ptr, size_t n) {
            if (this->size() > 0)
                this->get_allocator().deallocate(ptr, n);
        }

        void __destroy_range(T* first, T* last) {
            for (; first != last; ++first)
                first->~T();
        }


        void __clear()
        {
            size_t s = size();
            if (s > 0) {
                T* ptr = data();
                __destroy_range(ptr, ptr + s);
                __deallocate(ptr, s);
                this->__m_data.storage = 0;
            }
        }

        T* __erase(const T* where, size_t n)
        {
            if (n == 0)
                return const_cast<T*>(where);

            size_t s = size();
            T* ptr = data();
            /*if ((where < ptr) || ((where + n) > (ptr + s))) {
                throw std::out_of_range("pos/size is out of valid range");
            }*/

            size_t __size = s - n;
            if (__size == 0) { // special case - clear
                __destroy_range(ptr, ptr + s);
                __deallocate(ptr, s);
                this->__m_data.storage = 0;
                return nullptr;
            } else {
                T* space = __allocate(__size);
                T* mid = std::uninitialized_copy(ptr, const_cast<T*>(where), space);
                std::uninitialized_copy(const_cast<T*>(where + n), ptr + s, mid);

                // ???
                __destroy_range(ptr, ptr + s);
                __deallocate(ptr, s);

                this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
                this->__m_data.size = __size;
                return mid;
            }
        }

        template<class _FwdIt>
        T* __insert(const T* where, _FwdIt first, size_t n)
        {
            if (n == 0)
                return const_cast<T*>(where);

            size_t s = size();
            size_t __size = n + s;
            if ( __size > this->max_size()) {
                throw std::length_error("maximum vector size exceeded");
            }

            T* ptr = data();
            /*if (where < ptr || where > (ptr + s)) {
                throw std::out_of_range("pointer is out of valid range");
            }*/

            T* space = __allocate(__size);
            T* it = std::uninitialized_copy(std::make_move_iterator(ptr),
                                            std::make_move_iterator(const_cast<T*>(where)),
                                            space);
            T* pos = it;
            it = std::uninitialized_copy_n(std::make_move_iterator(first), n, it);
            std::uninitialized_copy(std::make_move_iterator(const_cast<T*>(where)),
                                    std::make_move_iterator(ptr + s), it);

            __destroy_range(ptr, ptr + s);
            __deallocate(ptr, s);

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __size;
            return pos;
        }


        T* __insert(const T* where, const T& v, size_t n)
        {
            if (n == 0)
                return const_cast<T*>(where);

            size_t s = size();
            size_t __size = n + s;

            if ( __size > this->max_size() ) {
                throw std::length_error("maximum vector size exceeded");
            }

            T* ptr = data();
            /*if (where < ptr || where > (ptr + s)) {
                throw std::out_of_range("pointer is out of valid range");
            }*/

            T* space = __allocate(__size);
            T* it = std::uninitialized_copy(ptr, const_cast<T*>(where), space);
            T* pos = it;
            it = std::uninitialized_fill_n(it, n, v);
            std::uninitialized_copy(const_cast<T*>(where), ptr + s, it);

            __destroy_range(ptr, ptr + s);
            __deallocate(ptr, s);

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __size;

            return pos;
        }

        void __assign(const T* what, size_t n)
        {
            size_t s = size();
            T* ptr = data();
            if (n == s) { // special case -- size equal: just copy
                std::copy_n(what, n, ptr);
                return;
            }

            if (n == 0) { // special case -- clear
                __destroy_range(ptr, ptr + s);
                __deallocate(ptr, s);
                this->__m_data.storage = 0;
                return;
            }

            T* space = __allocate(n);
            std::copy_n(what, n, space);

            // deallocate old storage if any
            __destroy_range(ptr, ptr + s);
            __deallocate(ptr, s);

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = n;
        }

        void __assign(const T& val, size_t n)
        {
            size_t s = size();
            T* ptr = data();
            if (n == s) { // special case -- size equal: just copy
                std::fill_n(ptr, n, val);
                return;
            }

            if (n == 0) { // special case -- clear
                if (s > 0) {
                    __destroy_range(ptr, ptr + s);
                    __deallocate(ptr, s);
                    this->__m_data.storage = 0;
                }
                return;
            }

            T* space = __allocate(n);
            std::uninitialized_fill_n(space, n, val);

            // deallocate old storage if any
            __destroy_range(ptr, ptr + s);
            __deallocate(ptr, s);

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = n;
        }


        T* __expand(size_t n)
        {
            size_t s = size();
            size_t __size = n + s;
            if ( __size > this->max_size() ) {
                throw std::length_error("maximum vector size exceeded");
            }

            T* ptr = data();
            T* space = __allocate(__size);
            //std::uninitialized_copy(ptr, ptr + s, space);
            std::uninitialized_copy(std::make_move_iterator(ptr),
                                    std::make_move_iterator(ptr + s),
                                    space);

            // ???
            __destroy_range(ptr, ptr + s);
            __deallocate(ptr, s);

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __size;
            return space;
        }

        template<class _It>
        void __assign_range(_It first, _It last, std::input_iterator_tag)
        {

            if (first == last)
                return;

            __clear();
            size_t s = 0;
            size_t n = sizeof(uintmax_t); // initial capacity
            T* ptr = this->__expand(n); // intitial reallocation
            T* end = ptr + n; // setup sentinel end pointer
            for (; first != last; ++first, ++ptr, ++s)
            {
                // move towards input sequence
                if (ptr == end) {
                    // reallocate if previously
                    // allocated space was exhaused
                    ptr = this->__expand(n);
                    // advance pointers
                    ptr += n;
                    end = ptr + n;
                    // update actual size
                    n <<= 1;
                }
                // copy value
                *ptr = *first;
            }
            // finally shrink data to exact size
            __erase(data() + s, n - s);
        }

        template<class _It>
        void __assign_range(_It first, _It last, std::forward_iterator_tag)
        {
            if (first == last)
                return;

            __clear();
            __insert(data(), first, std::distance(first, last));
        }

    public:
        basic_vector() : storage_type() {}
        explicit basic_vector(const _Alloc& al) : storage_type(al) {}

        size_t max_size() const { return ((size_t(1) << _SizeBits) - 1); }
        uint16_t size() const { return static_cast<uint16_t>(this->__m_data.size); }

        T* data() { return reinterpret_cast<T*>(this->__m_data.addr & ptr_mask); }
        const T* data() const { return reinterpret_cast<const T*>(this->__m_data.addr & ptr_mask); }
    };







    /// STRING STORAGES


    /// General case (no SSO)

    template<
        class _Char,
        size_t _SizeBits,
        size_t _TagBits, // assume not 1 nor 0
        class _Alloc,
        bool _SSO = (sizeof(_Char) < 4) // assume false
    >
    class string_storage :
        public alloc_ebo<_Alloc>
    {
    protected:
        static constexpr size_t local_bufsize = 0;
        static constexpr size_t max_local_capacity = 0;

        // general case: size of _Char is greater
        // than we can hold inside local buffer,
        // so no SSO can be applied here
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
                uint64_t tag : _TagBits;
            };
            uint64_t storage;
        } __m_data;

    public:
        string_storage() {
            __m_data.storage = 0;
        }

        explicit string_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al) {
            __m_data.storage = 0;
        }

        void tag(uint16_t t) { this->__m_data.tag = t; }
        uint16_t tag() const { return static_cast<uint16_t>(this->__m_data.tag); }
    };

    /// Intermedeate

    // Specialization for empty tag
    template<
        class _Char,
        size_t _SizeBits,
        class _Alloc
    >
    class string_storage<_Char, _SizeBits, 0, _Alloc, false> :
            public alloc_ebo<_Alloc>
    {
    protected:
        static constexpr size_t local_bufsize = 0;
        static constexpr size_t max_local_capacity = 0;

        // size of _Char is greater
        // than we can hold inside local buffer,
        // so no SSO can be applied here
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
            };
            uint64_t storage;
        } __m_data;

    public:
        string_storage() {
            __m_data.storage = 0;
        }

        explicit string_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al) {
            __m_data.storage = 0;
        }
    };


    // Specialization for boolean (1-bit) tag
    template<
        class _Char,
        size_t _SizeBits,
        class _Alloc
    >
    class string_storage<_Char, _SizeBits, 1, _Alloc, false> :
            public alloc_ebo<_Alloc>
    {
    protected:
        static constexpr size_t local_bufsize = 0;
        static constexpr size_t max_local_capacity = 0;

        // size of _Char is greater
        // than we can hold inside local buffer,
        // so no SSO can be applied here
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
                uint64_t tag : 1;
            };
            uint64_t storage;
        } __m_data;

     public:
        string_storage() {
            __m_data.storage = 0;
        }

        explicit string_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al) {
            __m_data.storage = 0;
        }

        void tag(bool on) { this->__m_data.tag = on; }
        bool tag() const { return static_cast<bool>(this->__m_data.tag); }
    };




    /// SSO SPECIALISATION

    // General case
    template<
        class _Char,
        size_t _SizeBits,
        size_t _TagBits,
        class _Alloc
    >
    class string_storage<_Char, _SizeBits, _TagBits, _Alloc, true> :
        public alloc_ebo<_Alloc>
    {
    protected:
        static constexpr size_t local_bufsize = sizeof(uint64_t) / sizeof(_Char);
        static constexpr size_t max_local_capacity = local_bufsize - sizeof(uint16_t) / sizeof(_Char);

        // SSO optimization
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
                uint64_t tag : _TagBits;
            };
            _Char local[local_bufsize];
            uint64_t storage;
        } __m_data;

    public:
        string_storage() {
            __m_data.storage = 0;
        }

        explicit string_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al)  {
            __m_data.storage = 0;
        }

        void tag(uint16_t t) { this->__m_data.tag = t; }
        uint16_t tag() const { return static_cast<uint16_t>(this->__m_data.tag); }
    };




    // Specialization for empty tag
    template<
        class _Char,
        size_t _SizeBits,
        class _Alloc
    >
    class string_storage<_Char, _SizeBits, 0, _Alloc, true> :
        public alloc_ebo<_Alloc>
    {
    protected:
        static constexpr size_t local_bufsize = sizeof(uint64_t) / sizeof(_Char);
        static constexpr size_t max_local_capacity = local_bufsize - sizeof(uint16_t) / sizeof(_Char);

        // SSO optimization
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
            };
            _Char local[local_bufsize];
            uint64_t storage;
        } __m_data;

    public:
        string_storage() {
            __m_data.storage = 0;
        }

        explicit string_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al)  {
            __m_data.storage = 0;
        }
    };



    // Specialization for boolean (1-bit) tag
    template<
        class _Char,
        size_t _SizeBits,
        class _Alloc
    >
    class string_storage<_Char, _SizeBits, 1, _Alloc, true> :
        public alloc_ebo<_Alloc>
    {
    protected:
        static constexpr size_t local_bufsize = sizeof(uint64_t) / sizeof(_Char);
        static constexpr size_t max_local_capacity = local_bufsize - sizeof(uint16_t) / sizeof(_Char);

        // SSO optimization
        union {
            struct {
                uint64_t addr : ptr_bits;
                uint64_t size : _SizeBits;
                uint64_t tag : 1;
            };
            _Char local[local_bufsize];
            uint64_t storage;
        } __m_data;

    public:
        string_storage() {
            __m_data.storage = 0;
        }

        explicit string_storage(const _Alloc& al) :
            alloc_ebo<_Alloc>(al)  {
            __m_data.storage = 0;
        }

        void tag(bool on) { this->__m_data.tag = on; }
        bool tag() const { return static_cast<bool>(this->__m_data.tag); }


    };






    // basic_string storage
    // SSO-optimized

    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _SizeBits,
        size_t _TagBits,
        bool _SSO = sizeof(_Char) < 4 // assume true
    >
    class basic_string_storage :
            public string_storage<_Char, _SizeBits, _TagBits, _Alloc, _SSO>
    {
        typedef string_storage<_Char, _SizeBits, _TagBits, _Alloc> storage_type;
        static_assert(_SizeBits > 0, "size bits cannot be 0");
        static_assert(_SizeBits + _TagBits <= 16, "incorrect number of bits for size and tag");

    protected:
        inline size_t __allocated() const {
            return (this->__m_data.size + 1);
        }

        inline bool __is_local() const {
            return __fit_local(this->__m_data.size + 1);
        }

        static inline bool __fit_local(size_t n) {
            return (n <= storage_type::max_local_capacity);
        }

        inline _Char* __local_ptr() {
            return this->__m_data.local;
        }
        inline const _Char* __local_ptr() const {
            return this->__m_data.local;
        }

        inline _Char* __heap_ptr() {
            return reinterpret_cast<_Char*>(this->__m_data.addr & ptr_mask);
        }
        inline const _Char* __heap_ptr() const {
            return reinterpret_cast<const _Char*>(this->__m_data.addr & ptr_mask);
        }


        _Char* __allocate(size_t n) {
            if (__fit_local(n))
                return this->__local_ptr();
            return this->get_allocator().allocate(n);
        }

        void __destroy() {
            if (!__is_local())
                this->get_allocator().deallocate(this->__heap_ptr(), __allocated());
        }

        void __clear()
        {
            __destroy();
            this->__m_data.storage = 0;
        }

        void __erase(size_t pos, size_t n)
        {
            if (n == 0)
                return;

            size_t s = size();
            if ((pos + n) > (s + 1))
                throw std::out_of_range("pos/size is out of valid range");

            const size_t __how_much = (s + 1) - pos - n;
            size_t __len = s - n;
            if (__len == 0) {
                __clear();
                return;
            }

            _Char* space = __allocate(__len + 1);
            _Char* ptr = data();

             // copy data
            _Traits::move(space, ptr, pos);
            _Traits::move(space + pos, ptr + pos + n, __how_much);

            if (!__is_local())
                this->get_allocator().deallocate(ptr, s);

            if (space != this->__local_ptr()) {
                this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            }

            this->__m_data.size = __len;
            space[__len] = _Char();
        }

        void __insert(size_t pos, const _Char* p, size_t n)
        {
            if (n == 0)
                return;

            size_t s = size();
            size_t __how_much = n + s;
            if ( __how_much > this->max_size() )
                throw std::length_error("maximum string size exceeded");

            _Char* space = __allocate(__how_much + 1);
            _Char* ptr = data();
            if (space != this->__local_ptr()) {
                // copy first chunk
                _Traits::copy(space, ptr, pos);
            }

            // copy data
            _Traits::move(space + pos, p, n);
            _Traits::move(space + n, ptr + pos, s - pos);


            __destroy();
            if (space != this->__local_ptr()) {
                this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            }
            this->__m_data.size = __how_much;
            space[__how_much] = _Char();
        }


        void __insert(size_t pos, size_t n, _Char c)
        {
            if (n == 0)
                return;

            size_t s = size();
            size_t __how_much = n + s;

            if (__how_much > this->max_size() )
                throw std::length_error("maximum string size exceeded");

            _Char* space = __allocate(__how_much + 1);
            _Char* ptr = data();
            if (space != this->__local_ptr()) {
                _Traits::copy(space, ptr, pos);
            }
            _Traits::move(space + __how_much, ptr + pos, s - pos);
            _Traits::assign(space + pos, n, c);

            __destroy();
            if (space != this->__local_ptr()) {
                this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            }
            this->__m_data.size = __how_much;
            data()[__how_much] = _Char();
        }



        void __assign(const _Char* src, size_t n)
        {

            if (n >= this->max_size())
                throw std::length_error("maximum string size exceeded");

            if (n == 0) { // special case - clear
                __clear();
                return;
            }

            size_t s = this->__allocated();
            size_t l = n + 1;
            if (l == s) { // size equal
                _Traits::move(data(), src, n);
                return;
            }

            __destroy();
            if (__fit_local(l))
            {
                // copy data to local storage
                _Traits::move(this->__local_ptr(), src, n);
            }
            else
            {
                // allocate new space
                _Char* space = __allocate(l);
                // copy data to allocated space
                _Traits::copy(space, src, n);
                // reset address
                this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            }

            // reset size
            this->__m_data.size = n;
            // allways null-terminated
            data()[n] = _Char(0);
        }

        void __assign(size_t n, _Char c)
        {
            if (n >= this->max_size())
                throw std::length_error("maximum string size exceeded");

            if (n == 0) { // special case - clear
                __clear();
                return;
            }

            size_t s = this->__allocated();
            size_t l = n + 1;
            if (l == s) { // size equal
                _Traits::assign(data(), n, c);
                return;
            }

            __destroy();
            if (__fit_local(l))
            {
                // fill local storage
                _Traits::assign(this->__local_ptr(), n, c);
            }
            else
            {
                // allocate new space
                _Char* space = __allocate(l);
                // fill allocated space
                _Traits::assign(space, n, c);
                // reset address
                this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            }

            // reset size
            this->__m_data.size = n;
            // allways null-terminated
            data()[n] = _Char(0);
        }

        _Char* __expand(size_t n)
        {
            size_t s = size();
            size_t __size = n + s + 1;
            if ( __size > this->max_size() ) {
                throw std::length_error("maximum string size exceeded");
            }

            _Char* ptr = data();
            _Char* space = __allocate(__size);
            _Traits::move(space, ptr, s);


            __destroy();

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __size - 1;

            return space;
        }

        template<class _It>
        void __assign_range(_It first, _It last, std::input_iterator_tag)
        {
            if (first == last)
                return;

            __clear();
            size_t s = 0;
            size_t n = storage_type::max_local_capacity; // initial capacity
            _Char* ptr = __expand(n);
            _Char* end = ptr + n; // setup sentinel end pointer
            for (; first != last; ++first, ++ptr, ++s)
            {
                // move towards input sequence
                if (ptr == end) {
                    // reallocate if previously
                    // allocated space was exhaused
                    ptr = __expand(n);
                    // advance pointers
                    ptr += n;
                    end = ptr + n;
                    // update actual size
                    n <<= 1;
                }
                // copy value
                *ptr = *first;
            }
            // finally shrink data to exact size
            __erase(s, n - s);
        }

        template<class _It>
        void __assign_range(_It first, _It last, std::forward_iterator_tag)
        {
            if (first == last)
                return;

            __clear();
            __insert(data(), first, std::distance(first, last));
        }

        static int __compare(size_t n1, size_t n2)
        {
            const std::ptrdiff_t __d = std::ptrdiff_t(n1 - n2);
            // clamp __d in [INT_MAX, INT_MIN]
            if (__d > INT_MAX)
              return INT_MAX;
            else if (__d < INT_MIN)
              return INT_MIN;
            else
              return int(__d);
        }

    public:
        basic_string_storage() : storage_type() {}
        explicit basic_string_storage(const _Alloc& al) : storage_type(al) {}

        size_t max_size() const {
            return ((size_t(1) << _SizeBits) - 1);
        }

        uint16_t size() const {
            return this->__m_data.size;
        }

        uint16_t length() const {
            return size();
        }

        _Char* data() {
            return (__is_local() ? this->__local_ptr() : this->__heap_ptr());
        }

        const _Char* data() const {
            return (__is_local() ? this->__local_ptr() : this->__heap_ptr());
        }

        const _Char* c_str() const {
            return data();
        }
    };





    /// NON-SSO SPECIALIZATION

    // basic_string storage
    template<
        class _Char,
        class _Traits,
        class _Alloc,
        size_t _SizeBits,
        size_t _TagBits
    >
    class basic_string_storage<_Char, _Traits, _Alloc, _SizeBits, _TagBits, false> :
            public string_storage<_Char, _SizeBits, _TagBits, _Alloc, false>
    {
        typedef string_storage<_Char, _SizeBits, _TagBits, _Alloc> storage_type;
        static_assert(_SizeBits > 0, "size bits cannot be 0");
        static_assert(_SizeBits + _TagBits <= 16, "incorrect number of bits for size and tag");

    protected:
        inline size_t __allocated() const {
            return (this->__m_data.size + 1);
        }

        inline _Char* __heap_ptr() {
            return reinterpret_cast<_Char*>(this->__m_data.addr & ptr_mask);
        }
        inline const _Char* __heap_ptr() const {
            return reinterpret_cast<const _Char*>(this->__m_data.addr & ptr_mask);
        }


        _Char* __allocate(size_t n) {
            return this->get_allocator().allocate(n);
        }

        void __destroy() {
             this->get_allocator().deallocate(this->__heap_ptr(), __allocated());
        }

        void __clear()
        {
            __destroy();
            this->__m_data.storage = 0;
        }

        void __erase(size_t pos, size_t n)
        {
            if (n == 0)
                return;

            size_t s = size();
            if ((pos + n) > (s + 1))
                throw std::out_of_range("pos/size is out of valid range");

            const size_t __how_much = (s + 1) - pos - n;
            size_t __len = s - n;
            if (__len == 0) {
                __clear();
                return;
            }

            _Char* space = __allocate(__len + 1);
            _Char* ptr = data();

             // copy data
            _Traits::move(space, ptr, pos);
            _Traits::move(space + pos, ptr + pos + n, __how_much);

            __destroy();
            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __len;
            space[__len] = _Char();
        }

        void __insert(size_t pos, const _Char* p, size_t n)
        {
            if (n == 0)
                return;

            size_t s = size();
            size_t __how_much = n + s;
            if ( __how_much > this->max_size() )
                throw std::length_error("maximum string size exceeded");

            _Char* space = __allocate(__how_much + 1);
            _Char* ptr = data();
            // copy first chunk
            _Traits::move(space, ptr, pos);
            // copy data
            _Traits::move(space + pos, p, n);
            _Traits::move(space + n, ptr + pos, s - pos);

            __destroy();
            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __how_much;
            space[__how_much] = _Char();
        }


        void __insert(size_t pos, size_t n, _Char c)
        {
            if (n == 0)
                return;

            size_t s = size();
            size_t __how_much = n + s;

            if (__how_much > this->max_size() )
                throw std::length_error("maximum string size exceeded");

            _Char* space = __allocate(__how_much + 1);
            _Char* ptr = data();
            _Traits::move(space, ptr, pos);
            _Traits::move(space + __how_much, ptr + pos, s - pos);
            _Traits::assign(space + pos, n, c);

            __destroy();
            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __how_much;
            data()[__how_much] = _Char();
        }



        void __assign(const _Char* src, size_t n)
        {

            if (n >= this->max_size())
                throw std::length_error("maximum string size exceeded");

            if (n == 0) { // special case - clear
                __clear();
                return;
            }

            size_t s = this->__allocated();
            size_t l = n + 1;
            if (l == s) { // size equal
                _Traits::move(data(), src, n);
                return;
            }

            // allocate new space
            _Char* space = __allocate(l);
            // copy data to allocated space
            _Traits::copy(space, src, n);

            // destroy old storage
            __destroy();

            // reset address
            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            // reset size
            this->__m_data.size = n;
            // allways null-terminated
            data()[n] = _Char(0);
        }

        void __assign(size_t n, _Char c)
        {
            if (n >= this->max_size())
                throw std::length_error("maximum string size exceeded");

            if (n == 0) { // special case - clear
                __clear();
                return;
            }

            size_t s = this->__allocated();
            size_t l = n + 1;
            if (l == s) { // size equal
                _Traits::assign(data(), n, c);
                return;
            }

            // allocate new space
            _Char* space = __allocate(l);
            // fill allocated space
            _Traits::assign(space, n, c);

            // destroy old storage
            __destroy();

            // reset address
            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            // reset size
            this->__m_data.size = n;
            // allways null-terminated
            data()[n] = _Char(0);
        }

        _Char* __expand(size_t n)
        {
            size_t s = size();
            size_t __size = n + s + 1;
            if ( __size > this->max_size() ) {
                throw std::length_error("maximum string size exceeded");
            }

            _Char* ptr = data();
            _Char* space = __allocate(__size);
            _Traits::copy(space, ptr, s);

            __destroy();

            this->__m_data.addr = reinterpret_cast<uint64_t>(space) & ptr_mask;
            this->__m_data.size = __size - 1;
            return space;
        }


        template<class _It>
        void __assign_range(_It first, _It last, std::input_iterator_tag)
        {
            if (first == last)
                return;

            size_t s = 0;
            size_t n = sizeof(uintmax_t); // initial capacity
            _Char* ptr = this->__expand(n); // intitial reallocation
            _Char* end = ptr + n; // setup sentinel end pointer
            for (; first != last; ++first, ++ptr, ++s)
            {
                // move towards input sequence
                if (ptr == end) {
                    // reallocate if previously
                    // allocated space was exhaused
                    ptr = this->__expand(n);
                    // advance pointers
                    ptr += n;
                    end = ptr + n;
                    // update actual size
                    n <<= 1;
                }
                // copy value
                *ptr = *first;
            }
            // finally shrink data to exact size
            __erase(s, n - s);
        }

        template<class _It>
        void __assign_range(_It first, _It last, std::forward_iterator_tag)
        {
            if (first == last)
                return;

            __insert(data(), first, std::distance(first, last));
        }


        static int __compare(size_t n1, size_t n2)
        {
            const std::ptrdiff_t __d = std::ptrdiff_t(n1 - n2);
            // clamp __d in [INT_MAX, INT_MIN]
            if (__d > INT_MAX)
              return INT_MAX;
            else if (__d < INT_MIN)
              return INT_MIN;
            else
              return int(__d);
        }



    public:
        basic_string_storage() : storage_type() {}
        explicit basic_string_storage(const _Alloc& al) : storage_type(al) {}

        size_t max_size() const {
            return ((size_t(1) << _SizeBits) - 1);
        }

        uint16_t size() const {
            return this->__m_data.size;
        }

        uint16_t length() const {
            return size();
        }

        _Char* data() {
            return (__heap_ptr());
        }

        const _Char* data() const {
            return (__heap_ptr());
        }

        const _Char* c_str() const {
            return data();
        }
    };

} // end namespace compact


