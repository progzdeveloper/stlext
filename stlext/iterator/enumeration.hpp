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

#include <algorithm>
#include <iterator>
#include <utility>

#include "../platform/common.h"

_STDX_BEGIN

/// INPUT-OUTPUT ITERATOR

template<
    class _It,
    class _Index = size_t,
    class _Category = typename std::iterator_traits<_It>::iterator_category
>
class enumerable_iterator
{
public:
    typedef std::iterator_traits<_It> traits_type;
    typedef typename traits_type::difference_type difference_type;

    typedef _Category iterator_category;
    typedef std::pair<_Index, _It> value_type;
    typedef value_type& reference;
    typedef value_type* pointer;
    typedef const value_type& const_reference;
    typedef const value_type* const_pointer;

    enumerable_iterator(_It first) : __m_ivp(0, first) {}
    enumerable_iterator(_It first, _Index offset) : __m_ivp(offset, first) {}

    inline _It base() const { return __m_ivp.second; }
    inline _It base() { return __m_ivp.second; }

    inline reference operator* () { return this->__m_ivp; }
    inline pointer operator-> () { return &this->__m_ivp; }

    inline const_reference operator* () const { return this->__m_ivp; }
    inline const_pointer operator-> () const { return &this->__m_ivp; }

    inline enumerable_iterator& operator++() {
        ++this->__m_ivp.first;
        ++this->__m_ivp.second;
        return (*this);
    }

    inline enumerable_iterator operator++(int) {
        enumerable_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    inline bool operator== (const enumerable_iterator& other) {
        return (this->__m_ivp.second == other.__m_ivp.second);
    }

    inline bool operator!= (const enumerable_iterator& other) {
        return !((*this) == other);
    }

    friend inline bool operator== (const enumerable_iterator& lhs, const _It& rhs) {
        return (lhs.__m_ivp.second == rhs);
    }

    friend inline bool operator== (const _It& lhs, const enumerable_iterator& rhs) {
        return (lhs == rhs.__m_ivp.second);
    }

    friend inline bool operator!= (const enumerable_iterator& lhs, const _It& rhs) {
        return (lhs.__m_ivp.second != rhs);
    }

    friend inline bool operator!= (const _It& lhs, const enumerable_iterator& rhs) {
        return (lhs != rhs.__m_ivp.second);
    }
private:
    value_type __m_ivp; // index-value pair
};



/// FORWARD ITERATOR

template<
    class _It,
    class _Index
>
class enumerable_iterator<_It, _Index, std::forward_iterator_tag>
{
public:
    typedef std::iterator_traits<_It> traits_type;
    typedef typename traits_type::difference_type difference_type;

    typedef std::forward_iterator_tag iterator_category;
    typedef std::pair<_Index, _It> value_type;

    typedef value_type& reference;
    typedef value_type* pointer;
    typedef const value_type& const_reference;
    typedef const value_type* const_pointer;


    enumerable_iterator(_It first) : __m_ivp(0, first) {}
    enumerable_iterator(_It first, _Index offset) : __m_ivp(offset, first) {}

    inline _It base() const { return __m_ivp.second; }
    inline _It base() { return __m_ivp.second; }

    inline reference operator* () { return this->__m_ivp; }
    inline pointer operator-> () { return &this->__m_ivp; }

    inline const_reference operator* () const { return this->__m_ivp; }
    inline const_pointer operator-> () const { return &this->__m_ivp; }

    inline enumerable_iterator& operator++() {
        ++this->__m_ivp.first;
        ++this->__m_ivp.second;
        return (*this);
    }

    inline enumerable_iterator operator++(int) {
        enumerable_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    friend inline bool operator== (const enumerable_iterator& lhs, const enumerable_iterator& rhs) {
        return (lhs.__m_ivp.second == rhs.__m_ivp.second);
    }

    friend inline bool operator!= (const enumerable_iterator& lhs, const enumerable_iterator& rhs) {
        return (lhs.__m_ivp.second != rhs.__m_ivp.second);
    }

    friend inline bool operator== (const enumerable_iterator& lhs, const _It& rhs) {
        return (lhs.__m_ivp.second == rhs);
    }

    friend inline bool operator== (const _It& lhs, const enumerable_iterator& rhs) {
        return (lhs == rhs.__m_ivp.second);
    }

    friend inline bool operator!= (const enumerable_iterator& lhs, const _It& rhs) {
        return (lhs.__m_ivp.second != rhs);
    }

    friend inline bool operator!= (const _It& lhs, const enumerable_iterator& rhs) {
        return (lhs != rhs.__m_ivp.second);
    }
protected:
    value_type __m_ivp; // index-value pair
};





/// BIDIRECTIONAL ITERATOR

template<
    class _It,
    class _Index
>
class enumerable_iterator<_It, _Index, std::bidirectional_iterator_tag> :
    public enumerable_iterator<_It, _Index, std::forward_iterator_tag>
{
public:
    typedef enumerable_iterator<_It, _Index, std::forward_iterator_tag> base_type;

    typedef std::iterator_traits<_It> traits_type;
    typedef typename traits_type::difference_type difference_type;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::pair<_Index, _It> value_type;

    typedef value_type& reference;
    typedef value_type* pointer;
    typedef const value_type& const_reference;
    typedef const value_type* const_pointer;


    enumerable_iterator(_It first) : base_type(first) {}
    enumerable_iterator(_It first, _Index offset) : base_type(first, offset) {}

    inline enumerable_iterator& operator--() {
        --this->__m_ivp.first;
        --this->__m_ivp.second;
        return (*this);
    }

    inline enumerable_iterator operator--(int) {
        enumerable_iterator tmp(*this);
        --(*this);
        return tmp;
    }
};



/// RANDOM-ACCESS ITERATOR

template<
    class _It,
    class _Index
>
class enumerable_iterator<_It, _Index, std::random_access_iterator_tag> :
    public enumerable_iterator<_It, _Index, std::bidirectional_iterator_tag>
{
public:
    typedef enumerable_iterator<_It, _Index, std::bidirectional_iterator_tag> base_type;

    typedef std::iterator_traits<_It> traits_type;
    typedef typename traits_type::difference_type difference_type;

    typedef std::random_access_iterator_tag iterator_category;
    typedef std::pair<_Index, _It> value_type;

    typedef value_type& reference;
    typedef value_type* pointer;
    typedef const value_type& const_reference;
    typedef const value_type* const_pointer;


    enumerable_iterator(_It first) : base_type(first) {}
    enumerable_iterator(_It first, _Index offset) : base_type(first, offset) {}


    enumerable_iterator& operator+= (difference_type d) {
        this->__m_ivp.first += d;
        this->__m_ivp.second += d;
    }

    enumerable_iterator& operator-= (difference_type d) {
        this->__m_ivp.first -= d;
        this->__m_ivp.second -= d;
    }
};

template<
    class _It,
    class _Index
>
inline typename enumerable_iterator<_It, _Index, std::random_access_iterator_tag>::difference_type
    operator- (enumerable_iterator<_It, _Index, std::random_access_iterator_tag>& lhs,
               enumerable_iterator<_It, _Index, std::random_access_iterator_tag>& rhs)
{
    return (lhs.__m_ivp.second - rhs.__m_ivp.second);
}

template<
    class _It,
    class _Index
>
inline enumerable_iterator<_It, _Index, std::random_access_iterator_tag>
    operator- (enumerable_iterator<_It, _Index, std::random_access_iterator_tag>& lhs,
               typename enumerable_iterator<_It, _Index, std::random_access_iterator_tag>::difference_type d)
{
    enumerable_iterator<_It, _Index, std::random_access_iterator_tag> ret(lhs);
    ret -= d;
    return (ret);
}


template<
    class _It,
    class _Index
>
inline enumerable_iterator<_It, _Index, std::random_access_iterator_tag>
    operator+ (typename enumerable_iterator<_It, _Index, std::random_access_iterator_tag>::difference_type d,
               enumerable_iterator<_It, _Index, std::random_access_iterator_tag>& rhs)
{
    enumerable_iterator<_It, _Index, std::random_access_iterator_tag> ret(rhs);
    ret += d;
    return (ret);
}

template<
    class _It,
    class _Index
>
inline enumerable_iterator<_It, _Index, std::random_access_iterator_tag>
    operator+ (enumerable_iterator<_It, _Index, std::random_access_iterator_tag>& lhs,
               typename enumerable_iterator<_It, _Index, std::random_access_iterator_tag>::difference_type d)
{
    enumerable_iterator<_It, _Index, std::random_access_iterator_tag> ret(lhs);
    ret += d;
    return (ret);
}






template<
    class _It,
    class _Index
>
inline enumerable_iterator<_It, _Index>
    make_enumerable_iterator(_It first, _Index offset)
{
    return enumerable_iterator<_It, _Index>(first, offset);
}

template<class _It>
inline enumerable_iterator<_It, typename std::iterator_traits<_It>::difference_type>
    make_enumerable_iterator(_It first)
{
    typedef typename std::iterator_traits<_It>::difference_type index_type;
    return enumerable_iterator<_It, index_type>(first, index_type(0));
}




template<
    class _It,
    class _Index
>
class enumeration
{
public:
    typedef enumerable_iterator<_It, _Index> iterator;
    typedef enumerable_iterator<_It, _Index> const_iterator;

    enumeration(_It first, _It last, _Index offset) :
        __m_first(first, offset), __m_last(last, offset) {}

    enumeration(iterator first, iterator last) :
        __m_first(first), __m_last(last) {}

    inline iterator begin() { return __m_first; }
    inline iterator end() { return __m_last; }
    inline const_iterator begin() const { return __m_first; }
    inline const_iterator end() const { return __m_last; }

private:
    iterator __m_first, __m_last;
};



template<class _It, class _Index>
inline enumeration<_It, _Index> enumerate(_It first, _It last, _Index offset) {
    return enumeration<_It, _Index>(first, last, offset);
}

template<class _It>
inline enumeration<_It, typename std::iterator_traits<_It>::difference_type>
    enumerate(_It first, _It last)
{
    typedef typename std::iterator_traits<_It>::difference_type diff_type;
    return enumeration<_It, diff_type>(first, last, diff_type(0));
}

template<class _Container>
inline auto enumerate(const _Container& c) {
    return enumerate<decltype(std::begin(c))>(std::begin(c), std::end(c));
}

template<class _Container>
inline auto enumerate(_Container& c) {
    return enumerate<decltype(std::begin(c))>(std::begin(c), std::end(c));
}

_STDX_END

