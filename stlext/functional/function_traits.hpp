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
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "../platform/common.h"


_STDX_BEGIN

/*!
\brief struct function_traints
\tparam F type of function object

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class F>
struct function_traits;

/*!
\brief struct function_traints
\tparam _Ret result type of function invokation
\tparam _Args argument types of function

struct function_traints provides ability to
retrieve function traits such as function arity,
result type and n-th argument type.
*/
template <class _Ret, class... _Args>
struct function_traits<_Ret(_Args...)> {
    typedef void class_type;   //!< type of class object (for class members)
    typedef _Ret result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> tuple_type;  //!< type of arguments tuple

    typedef std::function<_Ret(_Args...)>  function_type;  //!< type of std::function<> object
    typedef _Ret(callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);  //!< function arity
    static constexpr bool is_elipsis = false;

    /*!
    \brief helper struct argument
    \tparam N number of argument counting from 0.
    Provides ability to get n-th argument type
    */
    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type
            type;  //!< type of N-th argument
    };
};

/*!
\brief struct function_traints
\tparam _Ret result type of function invokation
\tparam _Args argument types of function

struct function_traints provides ability to
retrieve function traits such as function arity,
result type and n-th argument type.
*/
template <class _Ret, class... _Args>
struct function_traits<_Ret(_Args..., ...)> {
    typedef void class_type;   //!< type of class object (for class members)
    typedef _Ret result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> tuple_type;  //!< type of arguments tuple

    typedef void function_type;  //!< type of std::function<> object
    typedef _Ret(callable_type)(_Args..., ...);

    static constexpr size_t arity = sizeof...(_Args);  //!< function arity
    static constexpr bool is_elipsis = true;

    /*!
    \brief helper struct argument
    \tparam N number of argument counting from 0.
    Provides ability to get n-th argument type
    */
    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type  type;  //!< type of N-th argument
    };
};

/*!
\brief struct function_traints<> specialization for free function

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Ret, class... _Args>
struct function_traits<_Ret (*)(_Args...)> {
    typedef void class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Args...> tuple_type;

    typedef std::function<_Ret(_Args...)> function_type;
    typedef _Ret (*callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_elipsis = false;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};

/*!
\brief struct function_traints<> specialization for free elipsis-function

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Ret, class... _Args>
struct function_traits<_Ret (*)(_Args..., ...)> {
    typedef void class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Args...> tuple_type;

    typedef void function_type;
    typedef _Ret (*callable_type)(_Args..., ...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_elipsis = true;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};

/*!
\brief struct function_traints<> specialization for member function pointer

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Class, class _Ret, class... _Args>
struct function_traits<_Ret (_Class::*)(_Args...)> {
    typedef _Class class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Args...> tuple_type;

    typedef std::function<_Ret(_Class*, _Args...)> function_type;

    typedef _Ret (_Class::*callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_elipsis = false;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};

/*!
\brief struct function_traints<> specialization for elipsis member function
pointer

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Class, class _Ret, class... _Args>
struct function_traits<_Ret (_Class::*)(_Args..., ...)> {
    typedef _Class class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Args...> tuple_type;

    typedef void function_type;

    typedef _Ret (_Class::*callable_type)(_Args..., ...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_elipsis = true;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};

/*!
\brief struct function_traints<> specialization for const member function
pointer

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Class, class _Ret, class... _Args>
struct function_traits<_Ret (_Class::*)(_Args...) const> {
    typedef _Class class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Args...> tuple_type;

    typedef std::function<_Ret(_Class*, _Args...)> function_type;

    typedef _Ret (_Class::*callable_type)(_Args...) const;

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_elipsis = false;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};

/*!
\brief struct function_traints<> specialization for const elipsis member
function pointer

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Class, class _Ret, class... _Args>
struct function_traits<_Ret (_Class::*)(_Args..., ...) const> {
    typedef _Class class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Args...> tuple_type;

    typedef void function_type;

    typedef _Ret (_Class::*callable_type)(_Args..., ...) const;

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_elipsis = true;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};

/*!
\brief struct function_traints<> specialization for member object pointer

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class _Class, class _Ret>
struct function_traits<_Ret(_Class::*)> {
    typedef _Class class_type;
    typedef _Ret result_type;
    typedef std::tuple<_Class> tuple_type;

    typedef std::function<_Ret(_Class*)> function_type;

    typedef _Ret (_Class::*callable_type)();

    static constexpr size_t arity = 1;
    static constexpr bool is_elipsis = false;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };
};


/*!
\brief struct function_traints<> specialization for std::function<> object type

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class F>
struct function_traits {
   private:
    /* V.Lissev found out that to prevent miscompilations on GCC
     * we need a fully qualified name of stdx::function_traits<...>
     * for the helper __call_type type definition.
     */
    typedef typename stdx::function_traits<decltype(&F::operator())> __call_type;

   public:
    typedef F callable_type;
    typedef F function_type;
    typedef F class_type;
    typedef typename __call_type::result_type result_type;
    typedef typename __call_type::tuple_type tuple_type;

    static constexpr size_t arity = __call_type::arity;
    static constexpr bool is_elipsis = false;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "invalid argument index");
        using type = typename __call_type::template argument<N>::type;
    };
};

/*!
\brief struct function_traints<> specialization for std::function<> reference
object type

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class F>
struct function_traits<F&> : public function_traits<F> {};

/*!
\brief struct function_traints<> specialization for std::function<>
rvalue-reference object type

\see \ref function_traits< _Ret(_Args...) > for full documentation
*/
template <class F>
struct function_traits<F&&> : public function_traits<F> {};

_STDX_END
