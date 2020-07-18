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
#include <cstdarg>

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "../platform/common.h"


_STDX_BEGIN

/*!
 * \brief struct argument
 * \tparam N number of argument counting from 0.
 * Provides ability to get n-th argument type
 */

#define __DECL_ARGS_REGULAR()                                                     \
    template <size_t I>                                                           \
    struct argument {                                                             \
        static_assert(I < arity, "invalid argument index");                       \
        typedef typename std::tuple_element<I, arguments_tuple>::type type;       \
    };

#define __DECL_ARGS_ELIPSIS()                                                    \
    template <size_t I, bool _IsElipsis = is_elipsis>                            \
    struct argument {                                                            \
        static_assert(I < arity, "invalid argument index");                      \
        typedef typename std::tuple_element<I, arguments_tuple>::type type;      \
    };                                                                           \
                                                                                 \
    template <bool _IsElipsis>                                                   \
    struct argument<arity-1, _IsElipsis> {                                       \
        static_assert(_IsElipsis == true, "");                                   \
        typedef va_list type;                                                    \
    };



// predeclaration

/*!
 * \brief struct function_traints
 * \tparam _Ret result type of function invokation
 * \tparam _Args argument types of function
 * struct function_traints provides ability to
 * retrieve function traits such as function arity,
 * result type and n-th argument type.
 */

template <class T>
struct function_traits;



/*!
 * \brief struct function_traints<> specialization
 * for free function references
 */
template <class _Ret, class... _Args>
struct function_traits<_Ret(_Args...)>
{
    typedef void   class_type;   //!< type of class object (for class members)
    typedef _Ret   result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple;  //!< type of arguments tuple

    typedef std::function<_Ret (_Args...)> function_type;

    typedef _Ret (*signature_type)(_Args...);
    typedef _Ret (callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_member_function = false;
    static constexpr bool is_elipsis = false;
    static constexpr bool is_const = false;

    __DECL_ARGS_REGULAR()
};



/*!
 * \brief struct function_traints<> specialization
 * for free function pointers
 */
template <class _Ret, class... _Args>
struct function_traits<_Ret (*)(_Args...)>
{
    typedef void   class_type;   //!< type of class object (for class members)
    typedef _Ret   result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple;  //!< type of arguments tuple

    typedef std::function<_Ret (_Args...)> function_type;

    typedef _Ret (*signature_type)(_Args...);
    typedef _Ret (callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_member_function = false;
    static constexpr bool is_elipsis = false;
    static constexpr bool is_const = false;

    __DECL_ARGS_REGULAR()
};


/*!
 * \brief struct function_traints<> specialization
 * for pointers to const member function
 */
template <class _Class, class _Ret, class... _Args>
struct function_traits<_Ret(_Class::*)(_Args...) const>
{
    typedef _Class class_type; //!< type of class object (for class members)
    typedef _Ret   result_type; //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple; //!< type of arguments tuple

    typedef std::function<_Ret (_Args...)> function_type;

    typedef _Ret(_Class::* signature_type)(_Args...) const;
    typedef _Ret(callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_member_function = true;
    static constexpr bool is_elipsis = false;
    static constexpr bool is_const = true;

    __DECL_ARGS_REGULAR()
};

/*!
 * \brief struct function_traints<> specialization
 * for pointers to non-const member function
 */
template <class _Class, class _Ret, class... _Args>
struct function_traits<_Ret(_Class::*)(_Args...) >
{
    typedef _Class class_type;   //!< type of class object (for class members)
    typedef _Ret   result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple;  //!< type of arguments tuple

    typedef std::function<_Ret (_Args...)> function_type;

    typedef _Ret(_Class::* signature_type)(_Args...);
    typedef _Ret(callable_type)(_Args...);

    static constexpr size_t arity = sizeof...(_Args);
    static constexpr bool is_member_function = true;
    static constexpr bool is_elipsis = false;
    static constexpr bool is_const = false;

    __DECL_ARGS_REGULAR()
};



/*!
 * \brief struct function_traints<> specialization
 * for  generic types that are functors, delegate to its 'operator()'
 */
template <class T>
struct function_traits
{
    typedef function_traits<decltype(&T::operator())> _Base_type;

    typedef typename _Base_type::class_type      class_type;
    typedef typename _Base_type::result_type     result_type;
    typedef typename _Base_type::arguments_tuple arguments_tuple;

    typedef typename _Base_type::function_type   function_type ;

    typedef typename _Base_type::signature_type  signature_type;
    typedef typename _Base_type::callable_type   callable_type;

    static constexpr size_t arity = _Base_type::arity;
    static constexpr bool is_member_function = _Base_type::is_member_function;
    static constexpr bool is_elipsis = _Base_type::is_elipsis;
    static constexpr bool is_const = _Base_type::is_const;

    __DECL_ARGS_REGULAR()
};


/// elipsis



/*!
 * \brief struct function_traints<> specialization
 * for pointers to const member elipsis-like function
 */
template <class _Class, class _Ret, class... _Args>
struct function_traits< _Ret(_Class::*)(_Args..., ...) const>
{
    typedef _Class class_type;   //!< type of class object (for class members)
    typedef _Ret   result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple;  //!< type of arguments tuple

    typedef std::function<_Ret (_Args..., ...)> function_type;

    typedef _Ret(_Class::* signature_type)(_Args..., ...) const;
    typedef _Ret(callable_type)(_Args..., ...) const;

    static constexpr size_t arity = sizeof...(_Args) + 1;
    static constexpr bool is_member_function = true;
    static constexpr bool is_elipsis = true;
    static constexpr bool is_const = true;

    __DECL_ARGS_ELIPSIS()
};


/*!
 * \brief struct function_traints<> specialization
 * for pointers to const member elipsis-like function
 */
template <class _Class, class _Ret, class... _Args>
struct function_traits< _Ret(_Class::*)(_Args..., ...) >
{
    typedef _Class class_type;   //!< type of class object (for class members)
    typedef _Ret   result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple;  //!< type of arguments tuple

    typedef std::function<_Ret (_Args..., ...)> function_type;

    typedef _Ret(_Class::* signature_type)(_Args..., ...);
    typedef _Ret(callable_type)(_Args..., ...);


    static constexpr size_t arity = sizeof...(_Args) + 1;
    static constexpr bool is_member_function = true;
    static constexpr bool is_elipsis = true;
    static constexpr bool is_const = false;

    __DECL_ARGS_ELIPSIS()
};




/*!
 * \brief struct function_traints<> specialization
 * for free function pointers to elipsis-like functions
 */
template <class _Ret, class... _Args>
struct function_traits< _Ret (*)(_Args..., ...) >
{
    typedef void   class_type;   //!< type of class object (for class members)
    typedef _Ret   result_type;  //!< type of result of function invokation
    typedef std::tuple<_Args...> arguments_tuple;  //!< type of arguments tuple

    typedef std::function<_Ret (_Args..., ...)> function_type;

    typedef _Ret (*signature_type)(_Args..., ...);
    typedef _Ret (callable_type)(_Args..., ...);

    static constexpr size_t arity = sizeof...(_Args) + 1;
    static constexpr bool is_member_function = false;
    static constexpr bool is_elipsis = true;
    static constexpr bool is_const = false;

    __DECL_ARGS_ELIPSIS()
};


#undef __DECL_ARGS_REGULAR
#undef __DECL_ARGS_ELIPSIS



template <class _Callable>
typename function_traits<_Callable>::function_type make_function(_Callable c){
  return (typename function_traits<_Callable>::function_type)(c);
}




_STDX_END
