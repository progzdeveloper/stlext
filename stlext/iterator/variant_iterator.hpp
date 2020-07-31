// Copyright (c) 2010-2020, Michael Polukarov (Russia).
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

#ifdef _HAVE_BOOST_LIBS
#include <iterator>
#include "../platform/common.h"

_STDX_BEGIN
#include <boost/variant.hpp>

template<class T, class... _Iterators>
class variant_iterator
{
public:
    typedef boost::variant<_Iterators...> variant_t;

    typedef std::bidirectional_iterator_tag iterator_category;

    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef std::ptrdiff_t difference_type;

    variant_iterator() {}

    template<class _It>
    variant_iterator(_It it) : __m_var(it) {}

    reference operator*() {
        return boost::apply_visitor([](auto x) -> reference { return (reference)(x.operator*()); }, __m_var);
    }

    pointer operator->() {
        return boost::apply_visitor([](auto x) -> pointer { return (x.operator->()); }, __m_var);
    }

    const_reference operator*() const {
        return boost::apply_visitor([](auto x) -> const_reference { return (x.operator*()); }, __m_var);
    }

    const_pointer operator->() const {
        return boost::apply_visitor([](auto x) -> const_pointer { return (x.operator->()); }, __m_var);
    }

    variant_iterator& operator++() {
        boost::apply_visitor([](auto& x) { ++x; }, __m_var);
        return (*this);
    }

    variant_iterator operator++(int) {
        variant_iterator t(*this);
        ++(*this);
        return t;
    }

    variant_iterator& operator--() {
        boost::apply_visitor([](auto& x) { --x; }, __m_var);
        return (*this);
    }

    variant_iterator operator--(int) {
        variant_iterator t(*this);
        --(*this);
        return t;
    }

    bool operator==(const variant_iterator& other) const {
        return (__m_var == other.__m_var);
    }

    bool operator!=(const variant_iterator& other) const {
        return (__m_var != other.__m_var);
    }

private:
    variant_t __m_var;
};

_STDX_END

#endif
