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
#include <cmath>
#include <climits>


#include "../platform/common.h"


_STDX_BEGIN


#include <cmath>
#include <functional>

template<class _Storage, class _Hasher>
struct bloom_filter_base  : public _Hasher
{
    size_t size() const {
        return __m_size;
    }

    size_t hash_count() const {
        return __m_hash_count;
    }

    size_t capacity() const {
        return __m_storage.size();
    }


    double false_positive_rate() const
    {
        const double n = static_cast<double>(this->capacity());
        static const double k = static_cast<double>(this->hash_count());
        static const double m = static_cast<double>(this->size());
        static const double e =
                2.718281828459045235360287471352662497757247093699959574966;
        return std::pow(1 - std::pow(e, -k * n / m), k);
    }

protected:
    inline size_t hash_code(typename _Hasher::argument_type x) const {
        return _Hasher::operator()(x);
    }

    static size_t m(double fp, size_t capacity) {
      auto ln2 = std::log(2);
      return std::ceil(-(capacity * std::log(fp) / ln2 / ln2));
    }

    static size_t k(size_t cells, size_t capacity) {
      auto frac = static_cast<double>(cells) / static_cast<double>(capacity);
      return std::ceil(frac * std::log(2));
    }

protected:
    _Storage __m_storage;
    size_t   __m_size;
    size_t   __m_hash_count;
};







template<class _FilterClass>
class bloom_filter_interface : public _FilterClass
{
public:
    typedef typename _FilterClass::key_type   key_type;
    typedef typename _FilterClass::value_type value_type;

    typedef typename _FilterClass::reference reference;
    typedef typename _FilterClass::const_reference const_reference;

    typedef typename _FilterClass::pointer pointer;
    typedef typename _FilterClass::const_pointer const_pointer;

    bloom_filter_interface(size_t nhashes, size_t capacity) :
        _FilterClass(nhashes, capacity) {}

    bloom_filter_interface(double fp, size_t capacity) :
        _FilterClass(fp, capacity) {}


    using _FilterClass::insert;

    template<class _InIt>
    void insert(_InIt first, _InIt last)
    {
        for (; first != last; ++first) {
            _FilterClass::insert(*first);
        }
    }

    void insert(std::initializer_list<key_type> ilist) {
        this->insert(ilist.begin(), ilist.end());
    }


    using _FilterClass::count;

    template<class _InIt>
    size_t count(_InIt first, _InIt last) const
    {
        size_t n = 0;
        for (; first != last; ++first) {
            n += (_FilterClass::count(*first) > 0);
        }
        return n;
    }

    auto count(std::initializer_list<key_type> ilist) const {
        return count(ilist.begin(), ilist.end());
    }


    template<class _InIt>
    _InIt all_of(_InIt first, _InIt last) const {
        for (; first != last; ++first) {
            if (_FilterClass::count(*first) == 0)
                break;
        }
        return first;
    }

    auto all_of(std::initializer_list<key_type> ilist) const {
        return all_of(ilist.begin(), ilist.end());
    }

    template<class _InIt>
    _InIt any_of(_InIt first, _InIt last) const {
        for (; first != last; ++first) {
            if (_FilterClass::count(*first) > 0)
                break;
        }
        return first;
    }

    auto any_of(std::initializer_list<key_type> ilist) const {
        return any_of(ilist.begin(), ilist.end());
    }

};


template<class _FilterClass>
bool operator==(const bloom_filter_interface<_FilterClass>& lhs,
                const bloom_filter_interface<_FilterClass>& rhs) {
    return lhs.equal(rhs);
}

template<class _FilterClass>
bool operator!=(const bloom_filter_interface<_FilterClass>& lhs,
                const bloom_filter_interface<_FilterClass>& rhs) {
    return !(lhs == rhs);
}


_STDX_END
