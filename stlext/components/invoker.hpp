// Copyright (c) 2018, Michael Polukarov (Russia).
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
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <string>

#include "interpret_traits.hpp"

_STDX_BEGIN

/*!
 * \brief class basic_invoker 
 * 
 * \tparam _Key    models key type
 * \tparam _Source models source type
 * \tparam _Traits models interpret traits
 * 
 * class basic_invoker provides functionality that allows 
 * to bind an arbitary callable objects to keys and invoke
 * an callable by key.
 * 
 * basic_invoker class acts as a associated container
 * holding <key, callable> pairs with fast key lookup.
 * 
 * basic_invoker class can be used as a building block to
 * implement Command Pattern, and moreover to create
 * fnite-state machines that in turn can be used to create 
 * interactive console applications, web servers and other 
 * such applications.
 * 
 */
template<
	class _Key,
	class _Source = std::string,
	class _Traits = interpret_traits<_Source>
>
class basic_invoker
{
public:
	typedef size_t  size_type;
	typedef _Key    key_type;
	typedef _Source source_type;
	typedef _Traits traits_type;

	/*! 
	 * \brief class basic_command basic interface for commands
	 */
	class basic_command
	{
	public:
		basic_command(const std::string& help) :
			__m_help(help) {
		}
		/*! \brief destructor */
		virtual ~basic_command() {}
		/*! 
		 * \brief invoke command 
		 * \param arg argument source
		 */
		virtual void invoke(const _Source& arg) const = 0;
		/*! \brief command description */
		const std::string& help() const { return __m_help; }
	private:
		std::string __m_help;
	};


	/*! \internal */
	template<class F>
	class command;

	/*! \internal */
	template<class _Ret, class... _Args>
	struct command < _Ret(_Args...) > : public basic_command
	{
		static const size_t arity = sizeof...(_Args); //!< function arity
		typedef std::tuple<_Args...> tuple_type; //!< type of arguments tuple
		typedef _Ret(&method_type)(_Args...);

		command(method_type method, const std::string& help) :
			basic_command(help),
			__m_method(method) {
		}
		/*! \reimp */
		void invoke(const _Source& arg) const override
		{
			tuple_type args;
			_Traits::interpret(arg, args);
			apply(__m_method, args);
		}

		method_type __m_method;
	};

	/*! \internal */
	template<class _Ret, class... _Args>
	struct command < _Ret(*)(_Args...) > : public basic_command
	{
		static const size_t arity = sizeof...(_Args); //!< function arity
		typedef std::tuple<_Args...> tuple_type; //!< type of arguments tuple

		typedef _Ret(*method_type)(_Args...);

		command(method_type method, const std::string& help) :
			basic_command(help),
			__m_method(method) {
		}
		/*! \reimp */
		void invoke(const _Source& arg) const override
		{
			tuple_type args;
			_Traits::interpret(arg, args);
			apply(__m_method, args);
		}

		method_type __m_method;
	};

	/*! \internal */
	template<class _Class, class _Ret, class... _Args>
	struct command < _Ret(_Class::*)(_Args...) > : public basic_command
	{
		static const size_t arity = sizeof...(_Args); //!< function arity
		typedef std::tuple<_Args...> tuple_type; //!< type of arguments tuple

		typedef _Ret(_Class::*method_type)(_Args...);

		command(_Class* obj, method_type method, const std::string& help) :
			basic_command(help),
			__m_obj(obj),
			__m_method(method)
		{
		}
		/*! \reimp */
		void invoke(const _Source& arg) const override
		{
			typename std::tuple<typename std::decay<_Args>::type...> args;
			_Traits::interpret(arg, args);
			apply(__m_method, std::tuple_cat(std::make_tuple(__m_obj), args));
		}

		_Class* __m_obj;
		method_type __m_method;
	};

	/*! \internal */
	typedef std::unordered_map<key_type, std::unique_ptr<basic_command> > command_map;

	/*!
	 * \brief default constructor
	 */
	basic_invoker() {
	}

	/*!
	 * \brief destructor
	 */
	virtual ~basic_invoker() {
	}

	/*!
	 * \brief number of binded callables
	 * \return number of binded callables
	 */
	size_type size() const {
		return __m_command_map.size();
	}

	/*!
	 * \brief perform invoking a callable object associated with key \a key
	 * with arguments \a args.
	 * \param key  method key
	 * \param args method args source
	 * \return false if no method associated with key was found,
	 * othewrwise return true.
	 */
	virtual void invoke(const _Key& key, const _Source& args) const
	{
		const basic_command* cmd = find(key);
		if (!cmd)
			throw std::bad_function_call();
		cmd->invoke(args);
	}

	/*!
	 * \brief attempt to find method with specified key
	 * \return const pointer to an instance of basic_command interface
	 * if key was found, otherwise return null pointer.
	 */
	virtual const basic_command* find(const _Key& key) const {
		auto it = __m_command_map.find(key);
		return (it != __m_command_map.end() ? it->second.get() : nullptr);
	}
	
	/*!
	 * \brief bind a command to key \a key
	 * \param key unique key
	 * \param c   command to bind
	 */
	void bind(const _Key& key, basic_command* c) {
		__m_command_map.emplace(key, std::move(std::unique_ptr<basic_command>(c)));
	}

	/*!
	 * \brief bind a class method to key \a key
	 * \tparam _Class  type of class
	 * \tparam _Method type of method pointer
	 * \param key    unique key
	 * \param obj    pointer to \a _Class instance
	 * \param method pointer to class method
	 * \param help   description (optional)
	 */
	template<class _Class, class _Method>
	inline void bind(const _Key& key, _Class* obj, _Method method, const std::string& help = std::string())
	{
		this->bind(key, create_command(obj, method, help));
	}

	/*!
	 * \brief create a command from class method
	 * \tparam _Class  type of class
	 * \tparam _Method type of method pointer
	 * \param obj    pointer to \a _Class instance
	 * \param method pointer to class method
	 * \param help   description (optional)
	 */
	template<class _Class, class _Method>
	static inline basic_command* create_command(_Class* obj, _Method method, const std::string& help = std::string()) {
		return new command<_Method>(obj, method, help);
	}

	/*!
	 * \brief create a command from callable object
	 * \tparam _Callable  models callable type
	 * \param c     instance of _Callable type
	 * \param help  description (optional)
	 */
	template<class _Callable>
	static inline basic_command* create_command(_Callable c, const std::string& help = std::string()) {
		return new command<_Callable>(c, help);
	}

protected:
	command_map __m_command_map;
};


_STDX_END

