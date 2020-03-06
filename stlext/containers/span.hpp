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
#include <type_traits>
#include <iterator>
#include <algorithm>

#include "../platform/common.h"
#include "../compability/utility"

_STDX_BEGIN

static constexpr std::size_t dynamic_extent = std::size_t(-1);

template<
    class _Elem,
    std::size_t _Extent = dynamic_extent
>
class span
{
    static_assert(_Extent != 0, "unable to create span with zero extent");
public:
    typedef typename std::remove_cv<_Elem>::type value_type;
    typedef _Elem           element_type;
    typedef std::ptrdiff_t  index_type;
    typedef _Elem*          pointer;
    typedef const _Elem*    const_pointer;
    typedef _Elem&          reference;
    typedef const _Elem&    const_reference;

    typedef const _Elem* const_iterator;
    typedef const_iterator iterator;

    using reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_reverse_iterator = reverse_iterator;

    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr std::size_t npos = dynamic_extent;
    static constexpr std::size_t extent = _Extent;

    constexpr span() :
        __m_data(nullptr) {
    }

    template<class _Container>
    constexpr span(const _Container& c) :
        __m_data(const_cast<pointer>(std::data(c))) {
    }

    constexpr span(const_pointer ptr, size_type /*n*/) :
        __m_data(const_cast<pointer>(ptr)) {
    }

    constexpr span(const_pointer first, const_pointer /*last*/) :
        __m_data(const_cast<pointer>(first)) {
    }

    span(const element_type(&arr)[_Extent]) :
        __m_data(const_cast<pointer>(arr)) {
    }

    span(const span& other) :
        __m_data(other.__m_data) {
    }

    template<class U, std::size_t N>
    span(const span<U, N>& other) :
        __m_data(other.__m_data) {
    }

    template<class U>
    span& operator= (const span<U, _Extent>& other) {
        __m_data = other.__m_data;
        return (*this);
    }

    inline const_iterator begin() const { return (__m_data); }
    inline const_iterator end() const { return (__m_data + extent); }
    inline const_iterator rbegin() const { return std::make_reverse_iterator(end()); }
    inline const_iterator rend() const { return std::make_reverse_iterator(begin());  }

    constexpr size_type space() const {
        return extent * sizeof(value_type);
    }

    constexpr size_type size() const {
        return (extent);
    }

    constexpr size_type max_size() const {
        return (extent);
    }

    constexpr bool empty() const {
        return false;
    }

    constexpr const_pointer data() const {
        return __m_data;
    }

    constexpr const_reference operator[](size_t pos) const {
        return __m_data[pos];
    }

    constexpr const_reference at(size_type pos) const {
        __stdx_assertx(pos < extent, std::out_of_range, "span::at(): pos is out of range");
        return __m_data[pos];
    }

    template<std::size_t _Offset, std::size_t _Count>
    constexpr span<_Elem, _Count> subspan() const {
        return span<_Elem, _Count>(__m_data + _Offset, (_Count == dynamic_extent ? extent - _Offset : _Count));
    }

    constexpr span<_Elem, dynamic_extent> subspan(size_type offset = 0, size_type n = npos) const {
        __stdx_assertx(offset < extent, std::out_of_range, "span::sub(): __pos is out of range");
        return span<_Elem, dynamic_extent>(__m_data + offset, (std::min)(n, extent - offset));
    }

    constexpr const_reference front() const {
        return *(__m_data);
    }

    constexpr const_reference back() const {
        return *(__m_data + (extent - 1));
    }

private:
    pointer __m_data;
};



template<class _Elem>
class span<_Elem, dynamic_extent>
{
public:
    typedef typename std::remove_cv<_Elem>::type value_type;
    typedef _Elem           element_type;
    typedef std::ptrdiff_t  index_type;
    typedef _Elem*          pointer;
    typedef const _Elem*    const_pointer;
    typedef _Elem&          reference;
    typedef const _Elem&    const_reference;

    using const_iterator = const _Elem*;
    using iterator = const_iterator;

    using reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_reverse_iterator = reverse_iterator;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr std::size_t npos = size_type(-1);
    static constexpr std::size_t extent = dynamic_extent;

    span() :
        __m_data(nullptr), __m_size(0) {
    }

    template<class _Container>
    span(const _Container& c) :
        __m_data(const_cast<pointer>(std::data(c))),
        __m_size(std::size(c)) {
    }

    span(const_pointer ptr, size_type n) :
        __m_data(const_cast<pointer>(ptr)),
        __m_size(n) {
    }

    span(const_pointer first, const_pointer last) :
        __m_data(const_cast<pointer>(first)),
        __m_size(last - first) {
    }

    span(const span& other) :
        __m_data(other.__m_data),
        __m_size(other.__m_size) {
    }

    template<class U, std::size_t N>
    span(const span<U, N>& other) :
        __m_data(other.__m_data),
        __m_size(other.__m_size) {
    }

    template<class U>
    span& operator= (const span<U>& other) {
        __m_data = other.__m_data;
        __m_size = other.__m_size;
        return (*this);
    }

    inline const_iterator begin() const { return (__m_data); }
    inline const_iterator end() const { return (__m_data + __m_size); }
    inline const_iterator rbegin() const { return std::make_reverse_iterator(end()); }
    inline const_iterator rend() const { return std::make_reverse_iterator(begin()); }

    constexpr size_type space() const {
        return __m_size * sizeof(value_type);
    }

    constexpr size_type size() const {
        return __m_size;
    }

    constexpr size_type max_size() const {
        return size_type(-1);
    }

    constexpr bool empty() const {
        return (__m_size == 0);
    }

    constexpr const_pointer data() const {
        return __m_data;
    }

    inline const_reference at(size_type pos) const {
        __stdx_assertx(pos < __m_size, std::out_of_range, "span::at(): pos is out of range");
        return __m_data[pos];
    }

    constexpr const_reference operator[](size_t pos) const {
        return __m_data[pos];
    }

    template<
            std::size_t _Offset,
            std::size_t _Count = dynamic_extent
            >
    constexpr span<_Elem, _Count> subspan() const {
        return span<_Elem, _Count>(__m_data + _Offset, _Count == dynamic_extent ? (__m_size - _Offset) : _Count);
    }

    constexpr span<_Elem, dynamic_extent> subspan(size_type offset = 0, size_type n = npos) const {
        __stdx_assertx(offset < __m_size, std::out_of_range, "span::subspan(): __pos is out of range");
        return span<_Elem, dynamic_extent>(__m_data + offset, (std::min)(n, __m_size - offset));
    }

    constexpr const_reference operator()(size_t pos) const {
        return __m_data[pos];
    }

    constexpr const_reference front() const {
        return *(__m_data);
    }

    constexpr const_reference back() const {
        return *(__m_data + (__m_size - 1));
    }

    inline void pop_front() noexcept {
        if (__m_size > 0) {
            ++__m_data;
            --__m_size;
        }
    }

    inline void pop_back() noexcept {
        __m_size -= (__m_size > 0);
    }

    inline void remove_prefix(size_type n) {
        __m_data += n;
        __m_size -= n;
    }

    inline void remove_suffix(size_type n) {
        __m_size -= n;
    }

private:
    pointer __m_data;
    size_t __m_size;
};



template<
    class T1, std::size_t N1,
    class T2, std::size_t N2
>
inline bool operator== (const span<T1, N1>& lhs, const span<T2, N2>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<
    class T1, std::size_t N1,
    class T2, std::size_t N2
>
inline bool operator!= (const span<T1, N1>& lhs, const span<T2, N2>& rhs) {
    return !(lhs == rhs);
}



template<
    class T1, std::size_t N1,
    class T2, std::size_t N2
>
inline bool operator< (const span<T1, N1>& lhs, const span<T2, N2>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<
    class T1, std::size_t N1,
    class T2, std::size_t N2
>
inline bool operator<= (const span<T1, N1>& lhs, const span<T2, N2>& rhs) {
    return !(rhs < lhs);
}

template<
    class T1, std::size_t N1,
    class T2, std::size_t N2
>
inline bool operator> (const span<T1, N1>& lhs, const span<T2, N2>& rhs) {
    return (rhs < lhs);
}


template<
    class T1, std::size_t N1,
    class T2, std::size_t N2
>
inline bool operator>= (const span<T1, N1>& lhs, const span<T2, N2>& rhs) {
    return !(lhs < rhs);
}



template<class _Elem, size_t _Size>
inline span<_Elem, _Size> make_span(_Elem (&c)[_Size]) {
    return span<_Elem, _Size>(c);
}


template<class _Container>
inline span<typename _Container::value_type> make_span(const _Container& c) {
    return span<typename _Container::value_type>(c);
}

_STDX_END


