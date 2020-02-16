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
#include <cstdint>
#include <atomic>
#include <memory>

#include "../platform/common.h"

_STDX_BEGIN

class atomic_count
{
public:

	explicit atomic_count(size_t v) : value_(v)
	{
	}

	inline size_t operator++()
	{
		return value_.fetch_add(1, std::memory_order_acq_rel) + 1;
	}

	inline size_t operator--()
	{
		return value_.fetch_sub(1, std::memory_order_acq_rel) - 1;
	}

	inline operator size_t() const
	{
		return value_.load(std::memory_order_acquire);
	}

private:

	atomic_count(atomic_count const &);
	atomic_count & operator=(atomic_count const &);

	std::atomic_size_t value_;
};




/*!
* \brief Thread unsafe reference counter policy for \c intrusive_ref_counter
*
* The policy instructs the \c intrusive_ref_counter base class to implement
* a reference counter suitable for single threaded use only. Pointers to the same
* object with this kind of reference counter must not be used by different threads.
*/
struct thread_unsafe_counter
{
	typedef size_t type;

	static inline size_t load(size_t const& counter)
	{
		return counter;
	}

	static inline void increment(size_t& counter)
	{
		++counter;
	}

	static inline size_t decrement(size_t& counter)
	{
		return --counter;
	}
};

/*!
* \brief Thread safe reference counter policy for \c intrusive_ref_counter
*
* The policy instructs the \c intrusive_ref_counter base class to implement
* a thread-safe reference counter, if the target platform supports multithreading.
*/
struct thread_safe_counter
{
	typedef atomic_count type;

	static inline size_t load(atomic_count const& counter)
	{
		return (counter);
	}

	static inline void increment(atomic_count& counter)
	{
		++counter;
	}

	static inline size_t decrement(atomic_count& counter)
	{
		return --counter;
	}
};



template<typename T = void>
struct empty_delete {
	inline void operator()(T*) {}
};

template<>
struct empty_delete<void> {
	template<typename T>
	inline void operator()(T*) {}
};





template< typename _Type, typename _ThreadPolicy, typename _Destroyer >
class intrusive_ref_counter;

template< typename _Type, typename _ThreadPolicy, typename _Destroyer >
void intrusive_ptr_acqure(const intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer>* p);

template< typename _Type, typename _ThreadPolicy, typename _Destroyer >
void intrusive_ptr_release(const intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer>* p);



/*!
* \brief A reference counter base class
*
* This base class can be used with user-defined classes to add support
* for \c intrusive_ptr. The class contains a reference counter defined by the \c _ThreadPolicy.
* Upon releasing the last \c intrusive_ptr referencing the object
* derived from the \c intrusive_ref_counter class, operator \c delete
* is automatically called on the pointer to the object.
*
* The other template parameter, \c _Type, is the user's class that derives from \c intrusive_ref_counter.
*/
template<
	typename _Type,
	typename _ThreadPolicy = thread_safe_counter,
	typename _Destroyer = std::default_delete<_Type>
>
class intrusive_ref_counter
{
private:
	//! Reference counter type
	typedef typename _ThreadPolicy::type counter_type;
	//! Reference counter
	mutable counter_type m_ref_counter;

public:
	/*!
	* Default constructor
	*
	* \post <tt>use_count() == 0</tt>
	*/
	inline intrusive_ref_counter() : m_ref_counter(0)
	{
	}

	/*!
	* Copy constructor
	*
	* \post <tt>use_count() == 0</tt>
	*/
	inline intrusive_ref_counter(intrusive_ref_counter const&) : m_ref_counter(0)
	{
	}

	/*!
	* Assignment
	*
	* \post The reference counter is not modified after assignment
	*/
	inline intrusive_ref_counter& operator= (intrusive_ref_counter const&) { return *this; }

	/*!
	* \return The reference counter
	*/
	inline size_t use_count() const
	{
		return (typename _ThreadPolicy::load(m_ref_counter));
	}

	/*!
	* \return true if reference count is unique, false otherwise
	*/
	inline bool unique() const
	{
		return (typename _ThreadPolicy::load(m_ref_counter) == 1);
	}

	static _Destroyer get_deleter() { return _Destroyer(); }

	/*!
	* Destructor
	*/
	~intrusive_ref_counter() {
		assert(this->use_count() == 0);
	}


	friend void intrusive_ptr_acqure<_Type, _ThreadPolicy, _Destroyer>(const intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer>* p);
	friend void intrusive_ptr_release<_Type, _ThreadPolicy, _Destroyer>(const intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer>* p);
};



template<typename _Type, typename _ThreadPolicy, typename _Destroyer>
void intrusive_ptr_acqure(const intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer>* p)
{
	typename _ThreadPolicy::increment(p->m_ref_counter);
}

template<typename _Type, typename _ThreadPolicy, typename _Destroyer>
void intrusive_ptr_release(const intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer>* p)
{
	typedef intrusive_ref_counter<_Type, _ThreadPolicy, _Destroyer> type;
	if (typename _ThreadPolicy::decrement(p->m_ref_counter) == 0)
		typename type::get_deleter()(const_cast<_Type*>(static_cast< const _Type* >(p)));
}






//
//  intrusive_ptr
//
//  A smart pointer that uses intrusive reference counting.
//
//  Relies on unqualified calls to
//  
//      void intrusive_ptr_acqure(T * p);
//      void intrusive_ptr_release(T * p);
//
//          (p != 0)
//
//  The object is responsible for destroying itself.
//

template<class T>
class intrusive_ptr
{
private:
	typedef intrusive_ptr this_type;

public:
	typedef T element_type;

	intrusive_ptr() : px(0)
	{
	}

	intrusive_ptr(T * p, bool add_ref = true) : px(p)
	{
		if (px != 0 && add_ref) intrusive_ptr_acqure(px);
	}

	template<class U>
	intrusive_ptr(intrusive_ptr<U> const & rhs) : px(rhs.get())
	{
		if (px != 0) intrusive_ptr_acqure(px);
	}

	intrusive_ptr(intrusive_ptr const & rhs) : px(rhs.px)
	{
		if (px != 0) intrusive_ptr_acqure(px);
	}

	~intrusive_ptr()
	{
		if (px != 0) intrusive_ptr_release(px);
	}

	template<class U> intrusive_ptr & operator=(intrusive_ptr<U> const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}

	// Move support

#if !defined( _NO_CXX11_RVALUE_REFERENCES )

	intrusive_ptr(intrusive_ptr && rhs) : px(rhs.px)
	{
		rhs.px = 0;
	}

	intrusive_ptr & operator=(intrusive_ptr && rhs)
	{
		this_type(static_cast< intrusive_ptr && >(rhs)).swap(*this);
		return (*this);
	}

#endif

	intrusive_ptr & operator=(intrusive_ptr const & rhs)
	{
		this_type(rhs).swap(*this);
		return (*this);
	}

	intrusive_ptr & operator=(T * rhs)
	{
		this_type(rhs).swap(*this);
		return (*this);
	}

	void reset()
	{
		this_type().swap(*this);
	}

	void reset(T * rhs)
	{
		this_type(rhs).swap(*this);
	}

	void reset(T * rhs, bool add_ref)
	{
		this_type(rhs, add_ref).swap(*this);
	}

	inline T * get() const
	{
		return px;
	}

	inline T * detach()
	{
		T * ret = px;
		px = 0;
		return ret;
	}


	inline T & operator*() const
	{
		assert(px != 0);
		return *px;
	}

	inline T * operator->() const
	{
		assert(px != 0);
		return px;
	}

	// implicit conversion to "bool"
	explicit operator bool() const {
		return (px != nullptr);
	}


	void swap(intrusive_ptr & rhs)
	{
		T * tmp = px;
		px = rhs.px;
		rhs.px = tmp;
	}

private:
	T * px;
};


template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
	return (a.get() == b.get());
}

template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
	return (a.get() != b.get());
}

template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, U * b)
{
	return (a.get() == b);
}

template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, U * b)
{
	return (a.get() != b);
}

template<class T, class U> inline bool operator==(T * a, intrusive_ptr<U> const & b)
{
	return (a == b.get());
}

template<class T, class U> inline bool operator!=(T * a, intrusive_ptr<U> const & b)
{
	return (a != b.get());
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
	return a.get() != b.get();
}

#endif

#if !defined( _NO_CXX11_NULLPTR )

template<class T>
inline bool operator==(intrusive_ptr<T> const & p, nullptr_t)
{
	return (p.get() == 0);
}

template<class T>
inline bool operator==(nullptr_t, intrusive_ptr<T> const & p)
{
	return (p.get() == 0);
}

template<class T>
inline bool operator!=(intrusive_ptr<T> const & p, nullptr_t)
{
	return (p.get() != 0);
}

template<class T>
inline bool operator!=(nullptr_t, intrusive_ptr<T> const & p)
{
	return (p.get() != 0);
}

#endif

template<class T>
inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
	return std::less<T*>()(a.get(), b.get());
}

template<class T>
void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
{
	lhs.swap(rhs);
}

// mem_fn support

template<class T>
T * get_pointer(intrusive_ptr<T> const & p)
{
	return p.get();
}

template<class T, class U>
inline intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)
{
	return static_cast<T *>(p.get());
}

template<class T, class U>
inline intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)
{
	return const_cast<T *>(p.get());
}

template<class T, class U>
inline intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)
{
	return dynamic_cast<T *>(p.get());
}

// operator<<
template<class E, class T, class Y> 
std::basic_ostream<E, T>& operator<< (std::basic_ostream<E, T> & os, intrusive_ptr<Y> const & p)
{
	os << p.get();
	return os;
}

_STDX_END

// hash_value

namespace std
{
	template<typename T>
	struct hash< stdx::intrusive_ptr<T> > :
		public hash < T* >
	{
		typedef hash<T*> basic_hash;

		inline size_t operator()(stdx::intrusive_ptr<T> const & p) {
			return basic_hash::operator()(p.get());
		}
	};
}

