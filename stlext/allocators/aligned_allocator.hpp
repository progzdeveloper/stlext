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
#include <memory>
#include <type_traits>
#include "../platform/common.h"

_STDX_BEGIN

template<
	typename _Type, 
	size_t _Align 
>
class aligned_allocator
{
public:
	typedef _Type value_type;
	typedef value_type *pointer;
	typedef const value_type *const_pointer;
	typedef void *void_pointer;
	typedef const void *const_void_pointer;

	typedef value_type& reference;
	typedef const value_type& const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef std::true_type propagate_on_container_copy_assignment;
	typedef std::true_type propagate_on_container_move_assignment;
	typedef std::true_type propagate_on_container_swap;

	aligned_allocator<_Type, _Align> select_on_container_copy_construction() const
	{	// return this allocator
		return (*this);
	}

	template<class _Other>
	struct rebind
	{	// convert this type to allocator<_Other>
		typedef aligned_allocator<_Other, _Align> other;
	};

	pointer address(reference _Val) const throw()
	{	// return address of mutable _Val
		return (std::addressof(_Val));
	}

	const_pointer address(const_reference _Val) const throw()
	{	// return address of nonmutable _Val
		return (std::addressof(_Val));
	}

	aligned_allocator() throw()
	{	// construct default allocator (do nothing)
	}

	aligned_allocator(const aligned_allocator&) throw()
	{	// construct by copying (do nothing)
	}

	template<class _Other>
	aligned_allocator(const aligned_allocator<_Other, _Align>&) throw()
	{	// construct from a related allocator (do nothing)
	}

	template<class _Other>
	aligned_allocator<_Type, _Align>& operator=(const aligned_allocator<_Other, _Align>&)
	{	// assign from a related allocator (do nothing)
		return (*this);
	}

	void deallocate(pointer _Ptr, size_type)
	{	// deallocate object at _Ptr, ignore size
#ifdef STDX_CMPLR_MSVC
		::_aligned_free(_Ptr);
#else
		::free(_Ptr);
#endif
	}

	// ###
	pointer allocate(size_type _Count)
	{	// allocate array of _Count elements
#ifdef STDX_CMPLR_MSVC
		void_pointer addr = ::_aligned_malloc(_Count * sizeof(_Type), _Align);
#else
		void_pointer addr = (::memalign(_Align, _Count * sizeof(_Type)));
#endif
		if (addr == nullptr) {
			throw std::bad_alloc();
		}
		return static_cast<pointer>(addr);
	}

	// ###
	pointer allocate(size_type _Count, const_void_pointer hint)
	{	// reallocate array of _Count elements
#ifdef STDX_CMPLR_MSVC
		return ::_aligned_realloc(hint, nbytes, _Align);
#else
		return ::realloc(hint, nbytes);
#endif
	}

	void construct(pointer _Ptr)
	{	// default construct object at _Ptr
		::new ((void *)_Ptr) _Type();
	}

	void construct(pointer _Ptr, const _Type& _Val)
	{	// construct object at _Ptr with value _Val
		::new ((void *)_Ptr) _Type(_Val);
	}

	template<class _Objty,
	class... _Types>
		void construct(_Objty *_Ptr, _Types&&... _Args)
	{	// construct _Objty(_Types...) at _Ptr
		::new ((void *)_Ptr) _Objty(std::forward<_Types>(_Args)...);
	}


	template<class _Uty>
	void destroy(_Uty *_Ptr)
	{	// destroy object at _Ptr
		_Ptr->~_Uty();
	}

	size_t max_size() const throw()
	{	// estimate maximum array size
		return ((size_t)(-1) / sizeof(_Type));
	}
};


template<size_t _Align>
class aligned_allocator<void, _Align>
{	// generic allocator for type void
public:
	typedef aligned_allocator<void, _Align> other;

	typedef void value_type;

	typedef void *pointer;
	typedef const void *const_pointer;
	typedef void *void_pointer;
	typedef const void *const_void_pointer;

	template<class _Other>
	struct rebind
	{	// convert this type to an allocator<_Other>
		typedef aligned_allocator<_Other, _Align> other;
	};

	aligned_allocator() throw()
	{	// construct default allocator (do nothing)
	}

	aligned_allocator(const aligned_allocator<void, _Align>&) throw()
	{	// construct by copying (do nothing)
	}

	template<class _Other>
	aligned_allocator(const aligned_allocator<_Other, _Align>&) throw()
	{	// construct from related allocator (do nothing)
	}

	template<class _Other>
	aligned_allocator<void, _Align>& operator=(const aligned_allocator<_Other, _Align>&)
	{	// assign from a related allocator (do nothing)
		return (*this);
	}
};



template<
	class _Ty,
	class _Other,
	size_t _Align
>
inline bool operator==(const aligned_allocator<_Ty, _Align>&, const aligned_allocator<_Other, _Align>&) throw()
{	// test for allocator equality
	return (true);
}

template<
	class _Ty,
	class _Other,
	size_t _Align
>
inline bool operator!=(const aligned_allocator<_Ty, _Align>& _Left, const aligned_allocator<_Other, _Align>& _Right) throw()
{	// test for allocator inequality
	return (!(_Left == _Right));
}


_STDX_END

