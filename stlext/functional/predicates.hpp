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
#include <climits>
#include <iterator>
#include <bitset>
#include <unordered_set>
#include <initializer_list>
#include <functional>

#include "../platform/common.h"

_STDX_BEGIN

// ANY OF PREDICATE

/*!
\brief struct __is_any_of<T> provides unary predicate functor
\tparam T type of value

struct __is_any_of provides predicate which returns true
if element contains in specified element set.

Element set is sets up using constructor.

\note struct specializations of __is_any_of<T>
can use special optimizations.
\see __is_any_of<char>
\see __is_any_of<wchar_t>
*/
template<typename T>
struct __is_any_of :
        std::unary_function<const T&, bool>
{
    typedef T value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param first iterator pointing to first element
    \param last iterator pointing to one-past-last element

    Construct functor from [__first, __last) iterator range
    */
    template<typename _It>
    __is_any_of(_It first, _It last) :
        __m_uset(first, last) {
    }

    /*!
    \brief function call operator overload
    \return true if element set contains specified
    element \a x, otherwise return false.
    */
    inline bool operator()(const T& x) const {
        return (__m_uset.find(x) != __m_uset.end());
    }

private:
    std::unordered_set<T> __m_uset;
};

/*!
\brief struct __is_any_of<char> provides unary predicate functor specialized for char

struct __is_any_of<char> uses bitset internally to allow fast 
decision whatever element is contained or not.

For more detailed description see \ref __is_any_of.
*/
template<>
struct __is_any_of<char> :
        std::unary_function<char, bool>
{
    static const size_t CHARSET_SIZE = (1ULL << (sizeof(char) * CHAR_BIT));
    typedef char value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_any_of(_It first, _It last)
    {
        typedef typename std::make_unsigned<char>::type uchar_t;
        typedef typename std::iterator_traits<_It>::value_type iter_valty;
        static_assert(std::is_same<iter_valty, value_type>::value, "incorrect iterator value_type");
        for (; first != last; ++first) {
            __m_bits.set((uchar_t)*first);
        }
    }

    /*!
    \brief function call operator overload
    \return true if element set contains specified
    element \a x, otherwise return false.
    */
    inline bool operator()(char x) const {
        return __m_bits.test(x);
    }

private:
    std::bitset<CHARSET_SIZE> __m_bits;
};


/*!
\brief struct __is_any_of<wchar_t> provides unary predicate functor specialized for wchar_t

struct __is_any_of<wchar_t> uses bitset internally to allow fast
decision whatever element is contained or not.

For detailed description see \ref __is_any_of.
*/
template<>
struct __is_any_of<char16_t> :
        std::unary_function<char16_t, bool>
{
    static const size_t CHARSET_SIZE = (1ULL << (sizeof(char16_t) * CHAR_BIT));
    typedef char16_t value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_any_of(_It first, _It last)
    {
        typedef typename std::make_unsigned<char16_t>::type uchar_t;
        typedef typename std::iterator_traits<_It>::value_type iter_valtype;
        static_assert(std::is_same<iter_valtype, value_type>::value, "incorrect iterator value_type");
        for (; first != last; ++first) {
            __m_bits.set((uchar_t)*first);
        }
    }
    /*!
    \brief function call operator overload
    \return true if element set contains specified
    element \a x, otherwise return false.
    */
    inline bool operator()(char16_t x) const {
        return __m_bits.test(x);
    }

private:
    std::bitset<CHARSET_SIZE> __m_bits;
};

// On windows wchar_t is 16-bit
#ifdef STDX_OS_WIN
/*!
\brief struct __is_any_of<wchar_t> provides unary predicate functor specialized for wchar_t

struct __is_any_of<wchar_t> uses bitset internally to allow fast
decision whatever element is contained or not.

For detailed description see \ref __is_any_of.
*/
template<>
struct __is_any_of<wchar_t> :
        std::unary_function<wchar_t, bool>
{
    static const size_t CHARSET_SIZE = (1ULL << (sizeof(wchar_t) * CHAR_BIT));
    typedef wchar_t value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_any_of(_It first, _It last)
    {
        typedef typename std::make_unsigned<wchar_t>::type uchar_t;
        typedef typename std::iterator_traits<_It>::value_type iter_valtype;
        static_assert(std::is_same<iter_valtype, value_type>::value, "incorrect iterator value_type");
        for (; first != last; ++first) {
            __m_bits.set((uchar_t)*first);
        }
    }
    /*!
    \brief function call operator overload
    \return true if element set contains specified
    element \a x, otherwise return false.
    */
    inline bool operator()(wchar_t x) const {
        return __m_bits.test(x);
    }

private:
    std::bitset<CHARSET_SIZE> __m_bits;
};
#endif



// ANY OF PREDICATE PRODUCE FUNCTIONS

/*!
\brief produce function is_any_of()
\tparam _It type of iterator
\param first iterator pointing to first element
\param last iterator pointing to one-past-last element
\return implementation-defined functor

Construct functor from [__first, __last) iterator range
\relates __is_any_of
*/
template<typename _It>
__is_any_of<typename std::iterator_traits<_It>::value_type>
is_any_of(_It first, _It last)
{
    typedef typename std::iterator_traits<_It>::value_type type;
    return __is_any_of<type>(first, last);
}

/*!
\brief produce function is_any_of() (overload)
\tparam _Container type of container
\param delims container holding delimiters
\return implementation-defined functor

Construct functor from container
\relates __is_any_of
*/
template<typename _Container>
__is_any_of<typename _Container::value_type> 
is_any_of(const _Container& delims)
{
    return is_any_of(std::begin(delims), std::end(delims));
}

/*!
\brief produce function is_any_of() (overload)
\tparam _Tx type of value
\tparam _Size size of array
\param delims container holding delimiters
\return implementation-defined functor

Construct functor from fixed size array 
\relates __is_any_of
*/
template<typename _Tx, size_t _Size>
__is_any_of<_Tx> is_any_of(const _Tx (&delims)[_Size]) {
    return is_any_of(std::begin(delims), std::end(delims));
}


/*!
\brief produce function is_any_of() (overload)
\tparam _Tx type of value
\param delims initializer list
\return implementation-defined functor

Construct functor from initializer list
\relates __is_any_of
*/
template<typename _Tx>
__is_any_of<_Tx> is_any_of(std::initializer_list<_Tx> delims) 
{
    return is_any_of(std::begin(delims), std::end(delims));
}





// NONE OF PREDICATE


/*!
\brief struct __is_none_of<T> provides unary predicate functor
\tparam T type of value

struct __is_none_of provides predicate which returns true
if element NOT contains in specified element set.

Element set is sets up using constructor.

\note struct specializations of __is_none_of<_Tx>
can use special optimizations.
\see __is_none_of<char>
\see __is_none_of<wchar_t>
*/
template<typename T>
struct __is_none_of :
        std::unary_function<const T&, bool>
{
    typedef T value_type; //!< type of value
    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_none_of(_It first, _It last) :
        __m_uset(first, last) {
    }

    /*!
    \brief function call operator overload
    \return true if element does NOT contains in specified
    element set, otherwise return false.
    */
    inline bool operator()(const T& x) const {
        return (__m_uset.find(x) == __m_uset.end());
    }

private:
    std::unordered_set<T> __m_uset;
};

/*!
\brief struct __is_none_of<char> provides unary predicate functor specialized for char

struct __is_none_of<char> uses bitset internally to allow fast
decision whatever element is contained or not.

For detailed description see \ref __is_none_of
*/
template<>
struct __is_none_of<char> :
        std::unary_function<char, bool>
{
    static const size_t CHARSET_SIZE = (1ULL << (sizeof(char) * CHAR_BIT));
    typedef char value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_none_of(_It first, _It last)
    {
        typedef typename std::iterator_traits<_It>::value_type iter_valty;
        static_assert(std::is_same<iter_valty, value_type>::value, "incorrect iterator value_type");
        __m_bits.set(); // set all bits
        for (; first != last; ++first) {
            __m_bits.reset(*first); // reset target bit
        }
    }

    /*!
    \brief function call operator overload
    \return true if element does NOT contains in specified
    element set, otherwise return false.
    */
    inline bool operator()(char x) const {
        return __m_bits.test(x);
    }

private:
    std::bitset<CHARSET_SIZE> __m_bits;
};



/*!
\brief struct __is_none_of<char16_t> provides unary predicate functor specialized for char

struct __is_none_of<char> uses bitset internally to allow fast
decision whatever element is contained or not.

For detailed description see \ref __is_none_of
*/
template<>
struct __is_none_of<char16_t> :
        std::unary_function<char16_t, bool>
{
    static const size_t CHARSET_SIZE = (1ULL << (sizeof(char16_t) * CHAR_BIT));
    typedef char16_t value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_none_of(_It first, _It last)
    {
        typedef typename std::make_unsigned<char16_t>::type uchar_t;
        typedef typename std::iterator_traits<_It>::value_type iter_valty;
        static_assert(std::is_same<iter_valty, value_type>::value, "incorrect iterator value_type");
        __m_bits.set(); // set all bits
        for (; first != last; ++first) {
            __m_bits.reset((uchar_t)*first); // reset target bit
        }
    }

    /*!
    \brief function call operator overload
    \return true if element does NOT contains in specified
    element set, otherwise return false.
    */
    inline bool operator()(char16_t x) const {
        return __m_bits.test(x);
    }

private:
    std::bitset<CHARSET_SIZE> __m_bits;
};



#ifdef STDX_OS_WIN
/*!
\brief struct __is_none_of<wchar_t> provides unary predicate functor specialized for wchar_t

struct __is_none_of<wchar_t> uses bitset internally to allow fast
decision whatever element is contained or not.

For detailed description see \ref __is_none_of
*/
template<>
struct __is_none_of<wchar_t> :
        std::unary_function<wchar_t, bool>
{
    static const size_t CHARSET_SIZE = (1ULL << (sizeof(wchar_t) * CHAR_BIT));
    typedef wchar_t value_type; //!< type of value

    /*!
    \brief Parametrized constructor
    \tparam _It type of iterator
    \param __first iterator pointing to first element
    \param __last iterator pointing to one-past-last element

    Construct functor from [first, last) iterator range
    */
    template<typename _It>
    __is_none_of(_It first, _It last)
    {
        typedef typename std::make_unsigned<char16_t>::type uchar_t;
        typedef typename std::iterator_traits<_It>::value_type iter_valty;
        static_assert(std::is_same<iter_valty, value_type>::value, "incorrect iterator value_type");
        __m_bits.set(); // set all bits
        for (; first != last; ++first) {
            __m_bits.reset((uchar_t)*first); // reset target bit
        }
    }

    /*!
    \brief function call operator overload
    \return true if element does NOT contains in specified
    element set, otherwise return false.
    */
    inline bool operator()(wchar_t x) const {
        return __m_bits.test(x);
    }

private:
    std::bitset<CHARSET_SIZE> __m_bits;
};
#endif





// NONE OF PREDICATE PRODUCE FUNCTIONS

/*!
\brief produce function is_none_of()
\tparam _It type of iterator
\param __first iterator pointing to first element
\param __last iterator pointing to one-past-last element
\return implementation-defined functor

Construct functor from [first, last) iterator range
\relates __is_none_of
*/
template<typename _It>
__is_none_of<typename std::iterator_traits<_It>::value_type>
is_none_of(_It first, _It last)
{
    typedef typename std::iterator_traits<_It>::value_type type;
    return __is_none_of<type>(first, last);
}

/*!
\brief produce function is_none_of() (overload)
\tparam _Container type of container
\param delims container holding delimiters
\return implementation-defined functor

Construct functor from container
\relates __is_none_of
*/
template<typename _Container>
__is_none_of<typename _Container::value_type>
is_none_of(const _Container& delims)
{
    return is_none_of(std::begin(delims), std::end(delims));
}

/*!
\brief produce function is_none_of() (overload)
\tparam _Tx type of value
\tparam _Size size of array
\param delims container holding delimiters
\return implementation-defined functor

Construct functor from fixed size array
\relates __is_none_of
*/
template<typename _Tx, size_t _Size>
__is_none_of<_Tx> is_none_of(const _Tx(&delims)[_Size]) {
    return is_none_of(std::begin(delims), std::end(delims));
}


/*!
\brief produce function is_none_of() (overload)
\tparam _Tx type of value
\param delims initializer list
\return implementation-defined functor

Construct functor from initializer list
\relates __is_none_of
*/
template<typename _Tx>
__is_none_of<_Tx> is_none_of(std::initializer_list<_Tx> delims)
{
    return is_none_of(std::begin(delims), std::end(delims));
}

_STDX_END

