// Copyright (c) 2018-2020, Michael Polukarov (Russia).
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
#include <cstdint>
#include <vector>
#include <algorithm>

#include "bloom_filter_interface.hpp"
#include "bfc_utilities.h"


_STDX_BEGIN

template<
    class _Key,
    class _Hasher = std::hash<_Key>,
    class _Storage = std::vector<uint8_t>
>
class counting_bloom_filter_impl :
        public bloom_filter_base<_Storage, _Hasher>
{
    typedef bloom_filter_base<_Storage, _Hasher> base_type;

public:
    typedef _Key key_type;
    typedef _Key value_type;

    typedef _Key& reference;
    typedef const _Key& const_reference;
    typedef _Key* pointer;
    typedef const _Key* const_pointer;

    counting_bloom_filter_impl(size_t nhashes, size_t capacity)
    {
        this->__m_hash_count = nhashes;
        this->__m_storage.resize(capacity);
        this->clear();
    }

    counting_bloom_filter_impl(double fp, size_t capacity)
    {
        size_t optimal_capacity = (std::max)(base_type::m(fp, capacity), capacity);
        size_t optimal_hashes = base_type::k(optimal_capacity, capacity);
        //optimal_capacity += optimal_hashes - optimal_capacity % optimal_hashes;
        this->__m_hash_count = optimal_hashes;
        this->__m_storage.resize(optimal_capacity);
        this->clear();
    }

    void clear() {
        this->__m_size = 0;
        std::fill(this->__m_storage.begin(), this->__m_storage.end(), 0);
    }

    void insert(const_reference x) {
        this->insert(base_type::hash_code(x));
    }

    void insert(const void* addr, size_t size, size_t seed = 0) {
        this->insert(std::_Hash_bytes(addr, size, seed));
    }

    size_t count(const_reference x) {
        return this->count(base_type::hash_code(x));
    }

    size_t count(const void* addr, size_t size, size_t seed = 0) {
        return this->count(std::_Hash_bytes(addr, size, seed));
    }


    size_t erase(const_reference x) {
        return this->erase(base_type::hash_code(x));
    }

    template<class _InIt>
    size_t erase(_InIt first, _InIt last)
    {
        size_t n = 0;
        for (; first != last; ++first) {
            n += this->erase(*first);
        }
        return n;
    }

    size_t erase(std::initializer_list<_Key> ilist) {
        return this->erase(ilist.begin(), ilist.end());
    }


    bool equal(const counting_bloom_filter_impl& other) const {
        return (this->__m_storage == other.__m_storage);
    }

protected:
    void insert(size_t hash_val)
    {
        // 128b hash, [0] is the first half and [1] the second
        size_t hash_code[2];
        hash_code[0] = __murmur_mix(hash_val); // mix first half
        hash_code[1] = __murmur_mix(hash_code[0]); // mix second half

        size_t w = this->__m_storage.size();
        for (size_t i = 0; i < this->__m_hash_count; i++) {
            // use hash_code[0] as an accumulator, each different hash is computed as
            // (hash_code[0] + (i+1) * hash_code[1]) % __m_width
            hash_code[0] += hash_code[1];
            this->__m_storage[__fastrange(hash_code[0], w)] += 1; // hash_code[0] % w
        }
        ++this->__m_size;
    }

    size_t count(size_t hash_val) const
    {
        size_t result = (std::numeric_limits<size_t>::max)();
        // 128b hash, [0] is the first half and [1] the second
        size_t hash_code[2];
        hash_code[0] = __murmur_mix(hash_val); // mix first half
        hash_code[1] = __murmur_mix(hash_code[0]); // mix second half

        size_t w = this->__m_storage.size();
        for (size_t i = 0; i < this->__m_hash_count; i++) {
            // use hash_code[0] as an accumulator, each different hash is computed as
            // (hash_code[0] + (i+1) * hash_code[1]) % __m_width
            hash_code[0] += hash_code[1];
            auto val = this->__m_storage[__fastrange(hash_code[0], w)]; // hash_code[0] % w
            if (val < result)
                result = val;
        }
        return result;
    }

    size_t erase(size_t hash_val)
    {
        size_t result = 0;
        // 128b hash, [0] is the first half and [1] the second
        size_t hash_code[2];
        hash_code[0] = __murmur_mix(hash_val); // mix first half
        hash_code[1] = __murmur_mix(hash_code[0]); // mix second half

        size_t w = this->__m_storage.size();
        for (size_t i = 0; i < this->__m_hash_count; i++) {
            // use hash_code[0] as an accumulator, each different hash is computed as
            // (hash_code[0] + (i+1) * hash_code[1]) % __m_width
            hash_code[0] += hash_code[1];
            size_t value = this->__m_storage[__fastrange(hash_code[0], w)];
            if (value > 0) {
                result = (std::min)(value, result);
                --value;
                this->__m_storage[__fastrange(hash_code[0], w)] = value;
            }
        }
        size_t n = static_cast<size_t>((result > 0));
        this->__m_size -= n;
        return n;
    }
};


template<
    class _Key,
    class _Hasher = std::hash<_Key>,
    class _Storage = std::vector<uint8_t>
>
using counting_bloom_filter = bloom_filter_interface< counting_bloom_filter_impl<_Key, _Hasher, _Storage> >;


_STDX_END

