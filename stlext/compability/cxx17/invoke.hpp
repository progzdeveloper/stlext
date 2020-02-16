#pragma once
#include <type_traits>
#include "../../platform/common.h"

_STDX_BEGIN

namespace detail
{
	template<class T>
	struct _Is_member_function_pointer : 
		std::is_member_function_pointer<T>
	{
		typedef T class_type;
	};

	template<class T>
	struct _Is_member_object_pointer :
		std::is_member_object_pointer<T>
	{
		typedef T class_type;
	};

	// TEMPLATE FUNCTION invoke
	struct _Invoker_pmf_object
	{	// INVOKE a pointer to member function on an object
		template<
			class _Decayed,
			class _Ty1,
			class... _Args
		>
		static auto do_call(_Decayed pmf, _Ty1&& arg1, _Args&&... args)
			-> decltype((std::forward<_Ty1>(arg1).*pmf)(std::forward<_Args>(args)...))
		{	// INVOKE a pointer to member function on an object
			return ((std::forward<_Ty1>(arg1).*pmf)(std::forward<_Args>(args)...));
		}
	};

	struct _Invoker_pmf_pointer
	{	// INVOKE a pointer to member function on a [smart] pointer
		template<
			class _Decayed,
			class _Ty1,
			class... _Args
		>
		static auto do_call(_Decayed pmf, _Ty1&& arg1, _Args&&... args)
			-> decltype(((*std::forward<_Ty1>(arg1)).*pmf)(std::forward<_Args>(args)...))
		{	// INVOKE a pointer to member function on a [smart] pointer
			return (((*std::forward<_Ty1>(arg1)).*pmf)(std::forward<_Args>(args)...));
		}
	};

	struct _Invoker_pmd_object
	{	// INVOKE a pointer to member data on an object
		template<
			class _Decayed,
			class _Ty1
		>
		static auto do_call(_Decayed pmd, _Ty1&& arg1)
			-> decltype(std::forward<_Ty1>(arg1).*pmd)
		{	// INVOKE a pointer to member data on an object
			return (std::forward<_Ty1>(arg1).*pmd);
		}
	};

	struct _Invoker_pmd_pointer
	{	// INVOKE a pointer to member data on a [smart] pointer
		template<
			class _Decayed,
			class _Ty1
		>
		static auto do_call(_Decayed pmd, _Ty1&& arg1)
			-> decltype((*std::forward<_Ty1>(arg1)).*pmd)
		{	// INVOKE a pointer to member data on a [smart] pointer
			return ((*std::forward<_Ty1>(arg1)).*pmd);
		}
	};

	struct _Invoker_functor
	{	// INVOKE a function object
		template<
			class _Callable,
			class... _Types
		>
		static auto do_call(_Callable&& obj, _Types&&... args)
			-> decltype(std::forward<_Callable>(obj)(
				std::forward<_Types>(args)...))
		{	// INVOKE a function object
			return (std::forward<_Callable>(obj)(std::forward<_Types>(args)...));
		}
	};


	template<
		class _Callable,
		class _Ty1,
		class _Decayed
	>
	struct _Member_function_invoker
	{
		typedef typename std::conditional<
			std::is_base_of<
			typename _Is_member_function_pointer<_Decayed>::class_type,
			typename std::decay<_Ty1>::type
			>::value,
			_Invoker_pmf_object,
			_Invoker_pmf_pointer
		>::type base_type;
	};


	template<
		class _Callable,
		class _Ty1,
		class _Decayed
	>
	struct _Member_object_invoker
	{
		typedef typename std::conditional<
			std::is_base_of<
			typename _Is_member_object_pointer<_Decayed>::class_type,
			typename std::decay<_Ty1>::type
			>::value,
			_Invoker_pmd_object,
			_Invoker_pmd_pointer
		>::type base_type;
	};






	template<
		class _Callable,
		class _Ty1,
		class _Decayed = typename std::decay<_Callable>::type,
		bool _Is_pmf = std::is_member_function_pointer<_Decayed>::value,
		bool _Is_pmd = std::is_member_object_pointer<_Decayed>::value
	>
	struct _Invoker1;

	template<
		class _Callable,
		class _Ty1,
		class _Decayed
	>
	struct _Invoker1<_Callable, _Ty1, _Decayed, true, false> : 
		_Member_function_invoker<_Callable, _Ty1, _Decayed>::base_type
		/*: std::conditional<
			std::is_base_of<
				typename _Is_member_function_pointer<_Decayed>::class_type,
				typename std::decay<_Ty1>::type
			>::value,
			_Invoker_pmf_object,
			_Invoker_pmf_pointer
		>::type*/
	{	// pointer to member function
	};

	template<
		class _Callable,
		class _Ty1,
		class _Decayed
	>
	struct _Invoker1<_Callable, _Ty1, _Decayed, false, true> : 
		_Member_object_invoker<_Callable, _Ty1, _Decayed>::base_type
		/*: std::conditional<
			std::is_base_of<
				typename _Is_member_object_pointer<_Decayed>::class_type,
				typename std::decay<_Ty1>::type
			>::value,
			_Invoker_pmd_object,
			_Invoker_pmd_pointer
		>::type*/
	{	// pointer to member data
	};

	template<class _Callable,
		class _Ty1,
		class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, false, false>
		: _Invoker_functor
	{	// function object
	};

	template<
		class _Callable,
		class... _Types
	>
	struct _Invoker;

	template<class _Callable>
	struct _Invoker<_Callable>
		: _Invoker_functor
	{	// zero arguments
	};

	template<
		class _Callable,
		class _Ty1,
		class... _Types2
	>
	struct _Invoker<_Callable, _Ty1, _Types2...> : 
			_Invoker1<_Callable, _Ty1>
	{	// one or more arguments
	};


} // end namespace detail


/*!
 * \fn invoke(_Callable&& _Obj, _Types&&... _Args)
 * \brief Invoke the Callable object with the specified parameters
 * \tparam _Callable models callable type
 * \tparam _Types... models callable parameter pack
 * 
 * \param _Obj callable object to be invoked
 * \param _Args arguments parameter pack to pass to callable object
 * \return result of invoking callable object
 *
 * \note This function is part of C++17 standard library
 * \note Provided for forward compability
 */
template<class _Callable,
	class... _Types> inline
	auto invoke(_Callable&& _Obj, _Types&&... _Args)
	-> decltype(detail::_Invoker<_Callable, _Types...>::do_call(
				std::forward<_Callable>(_Obj), std::forward<_Types>(_Args)...))
{	// INVOKE a callable object
	return (detail::_Invoker<_Callable, _Types...>::do_call(std::forward<_Callable>(_Obj),
													std::forward<_Types>(_Args)...));
}



_STDX_END



