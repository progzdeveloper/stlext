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
#include <typeinfo>
#include <typeindex>
#include <algorithm>
#include <unordered_map>

#include "../platform/common.h"


_STDX_BEGIN

/*!
 *
 * \brief template class_factory<_Base, _Key, _Args...> provides generic
 * implementation of Factory Method pattern.
 *
 * \tparam _Base type of base class
 * \tparam _Key  type of key
 *
 * template class class_factory<_Base, _Key, _Args...> provides generic
 * implementation of factory method pattern. The main aim of this class is to
 * simplify implementation of Factory Method pattern. Users of this class
 * can define full featured class factory for their class hierarhy with
 * only a few lines of code.
 *
 * Internally class_factory uses unordered_map<> to hold special objects
 * called 'creator' along with corresponding key.
 *
 * The creator class is implementation defined template class wich
 * have a possibility to create concrete class type derived from _Base class
 * using specified allocator (std::allocator<T> by default).
 *
 */
template<
    class _Base, 
    class _Key,
    class _KeyHash = std::hash<_Key>,
    class _KeyComp = std::equal_to<_Key>,
    class _Alloc = std::allocator<void>
>
class class_factory
{
    static_assert(!std::is_same<_Key, void>::value,
                  "_Key type cannot be void type");

    static_assert(!std::is_reference<typename std::remove_cv<_Base>::type>::value,
                  "_Base type cannot be a reference type");

public:
    using key_type = _Key ;   //!< type of key
    using base_type = _Base ; //!< type of base class

    using identifier = std::pair<key_type, const std::type_info&>;

protected:
    class creator_base
    {
    public:
        creator_base(size_t size) : __m_size(size) { }

        virtual ~creator_base() { }

        virtual const std::type_info& product_type() const = 0;

    protected:
        const size_t __m_size;
    };


    template<class... _Args>
    class basic_creator : public creator_base
    {
    public:
        basic_creator(size_t n) :
            creator_base(n) {
        }
        
        template<class _Allocator>
        inline _Base* create(const _Allocator& al, _Args&&... args) const
        {
            // rebind allocator
            using allocator_type = typename std::allocator_traits<_Allocator>::template rebind_alloc<char>;

            allocator_type alloc_proxy(al);
            char* addr = nullptr;
            try {
                // allocation and/or construction may throw
                addr = alloc_proxy.allocate(this->__m_size);
                construct(addr, std::forward<_Args>(args)...);
            }
            catch (...) {
                alloc_proxy.deallocate(addr, this->__m_size);
                addr = nullptr;
                throw;
            }
            return reinterpret_cast<_Base*>(addr);
        }

    protected:
        virtual _Base* construct(void* addr, _Args&&... /*args*/) const {
            return reinterpret_cast<_Base*>(addr);
        }
    };
    
    template<class _Product, class... _Args>
    class creator : public basic_creator<_Args...>
    {
    public:
        creator() :
            basic_creator<_Args...>(sizeof(_Product)) {
        }
        
        // \reimp from creator_base
        inline const std::type_info& product_type() const {
            return typeid(_Product);
        }
    protected:
        // \reimp from basic_creator<...>
        inline _Base* construct(void* addr, _Args&&... args) const {
            return new(addr)_Product(std::forward<_Args>(args)...);
        }
    };

    struct hasher : public _KeyHash
    {
        inline size_t operator()(const identifier& id) const {
            return _KeyHash::operator()(id.first) ^ id.second.hash_code();
        }
    };

    struct key_comp : public _KeyComp
    {
        inline size_t operator()(const identifier& lhs, const identifier& rhs) const {
            return (_KeyComp::operator()(lhs.first, rhs.first) && lhs.second == rhs.second);
        }
    };

    using allocator_type = typename std::allocator_traits<_Alloc>::template rebind_alloc<std::pair<const identifier, creator_base*>>;
    using creator_map = std::unordered_map<identifier, creator_base*, hasher, key_comp, allocator_type>;

    struct deallocator : 
        public allocator_type
    {
        template<class _Other>
        deallocator(const _Other& al) :
            allocator_type(al) {}
        
        template<class U>
        void operator()(U *_Ptr) const {
            // rebind allocator
            typedef typename _Alloc::template rebind<U>::other allocator_type;
            allocator_type alloc_proxy(*this);
            alloc_proxy.destroy(_Ptr);
            alloc_proxy.deallocate(_Ptr, 1);
        }
    };

    template<class _Product, class... _Args>
    struct registerer 
    {
        using product_type = typename std::decay_t<_Product>;
        using args_tuple = std::tuple<typename std::decay<_Args>::type...>;
        using creator_type = creator<product_type, typename std::decay<_Args>::type...>;
        
        static_assert(std::is_base_of<_Base, product_type>::value, "_Product does not inherits _Base class");
        
        template<class _Allocator>
        static inline creator_type* make_creator(const _Allocator& al)
        {
            // rebind allocator
            using allocator_type = typename std::allocator_traits<_Allocator>::template rebind_alloc<creator_type>;

            allocator_type alloc_proxy(al);
            creator_type* addr = nullptr;
            try {
                // allocation and/or construction may throw
                addr = alloc_proxy.allocate(1);
                alloc_proxy.construct(addr);
            }
            catch (...) {
                alloc_proxy.deallocate(addr, 1);
                addr = nullptr;
                throw;
            }
            return addr;
        }

        static inline identifier make_identifier(const key_type& key) {
            return { key, typeid(args_tuple) };
        }
    };

    template<class _Product, class... _Args>
    struct registerer<_Product(_Args...)> :
        public registerer<_Product, _Args...> {
    };

private:
    typedef class_factory<_Base, _Key, _KeyHash, _KeyComp, allocator_type> this_type;


public:
    /*!
     * \brief The iterator class provides iterations over stored keys
     *
     * class iterator provides forward only iterator for keys stored
     * in class factory mapping.
     */
    class _Iterator
    {
        friend class_factory;

        _Iterator(typename creator_map::const_iterator it) :
            __m_it(it) {
        }
    public:
        typedef std::forward_iterator_tag iterator_category;

        using value_type = identifier;
        using reference = identifier& ;
        using const_reference = const identifier&;
        using pointer = identifier*;
        using const_pointer = const identifier*;
        using difference_type = std::ptrdiff_t;
        using distance_type = std::ptrdiff_t;

        _Iterator() {}
        _Iterator(const _Iterator& other) : __m_it(other.__m_it) {}

        const_reference operator*() const { return __m_it->first; }
        const_pointer operator->() const { return &__m_it->first;  }

        const std::type_info& product_type() const { return __m_it->second->product_type(); }

        _Iterator& operator++() { ++__m_it; return (*this); }
        _Iterator operator++(int) { iterator tmp = (*this); ++__m_it; return tmp; }

        friend bool operator== (const _Iterator& __lhs, const _Iterator& __rhs) {
            return (__lhs.__m_it == __rhs.__m_it);
        }

        friend bool operator!= (const _Iterator& __lhs, const _Iterator& __rhs) {
            return (__lhs.__m_it != __rhs.__m_it);
        }

    private:
        typename creator_map::const_iterator __m_it;
    };


    typedef _Iterator iterator;

    /*!
     * \brief Default constructor
     */
    class_factory() { }

    class_factory(const class_factory&) = delete;
    class_factory& operator=(const class_factory&) = delete;

    /*!
     * \brief Destructor
     *
     * Release all resources of class_factory
     */
    virtual ~class_factory()
    { 
        deallocator d(__m_creators.get_allocator());
        for (auto it = __m_creators.begin(); it != __m_creators.end(); ++it)
            d(it->second);
    }
    
    /*!
     * \brief Registrate new product type in factory
     *
     * Registrate new product type in factory with specified key
     * \tparam _Product type of product class
     * \tparam _Args... variadic type parameter pack
     * \param key key for _Product type
     */
    template<class _Product, class... _Args>
    inline typename std::enable_if_t<!std::is_function_v<_Product>, void>
        registrate(const key_type& key)
    {
        using registeter_type = registerer<_Product, _Args...>;
        __m_creators.emplace(registeter_type::make_identifier(key), registeter_type::make_creator(__m_creators.get_allocator()));
    }

    /*!
     * \brief Registrate new product type in factory
     *
     * Registrate new product type in factory with specified key \a key
     * This overload accepts template paramenter pack in function style
     * \tparam _Fn type of production function
     * \param key key for _Product type
     */
    template<class _Fn>
    inline typename std::enable_if_t<std::is_function_v<_Fn>, void>
        registrate(const key_type& key)
    {
        using registeter_type = registerer<_Fn>;
        __m_creators.emplace(registeter_type::make_identifier(key), registeter_type::make_creator(__m_creators.get_allocator()));
    }


    /*!
     * \brief Allocates and constructs derived class for specified key \a key.
     * \tparam _Allocator type of allocator
     * \param al allocator
     * \param key key of registered type
     * \param args arguments for the constructor
     * \return proxy-object convertible to raw/unique/shared pointer types
     */
    template<class _Allocator, class... _Args>
    inline base_type* allocate_object(const _Allocator& al, const _Key& key, _Args&&... args) const
    {
        using namespace std;
        using creator_type = basic_creator<_Args...> ;
        static const type_info& args_ti = typeid(tuple<typename decay<_Args>::type...>);

        auto it = __m_creators.find({ key, args_ti });
        return (it != __m_creators.end() ?
                    static_cast<creator_type*>(it->second)->create(al, forward<_Args>(args)...) :
                    nullptr);
    }

    /*!
     * \brief Allocates and constructs derived class for specified key __key.
     * \param __key key of registered type
     * \param __args arguments for the constructor
     * \return proxy-object convertible to raw/unique/shared pointer types
     */
    template<class... _Args>
    inline base_type* create_object(const _Key& key, _Args&&... args) const {
        return allocate_object(std::allocator<void>(), key, std::forward<_Args>(args)...);
    }

    template<class... _Args>
    inline base_type* operator()(const _Key& key, _Args&&... args) const {
        return create_object(key, std::forward<_Args>(args)...);
    }

    /// Iterators

    /*!
     * \brief Iterator pointing to first key
     * \return const key iterator
     */
    inline iterator begin() const {
        return _Iterator(__m_creators.begin());
    }
    /*!
     * \brief Iterator pointing to one-past-last key
     * \return const key iterator
     */
    inline iterator end() const {
        return _Iterator(__m_creators.end());
    }

    /*!
     * \brief Check if specified key contained in factory
     * \param __key key of registered type
     * \return true if key contains, otherwise false
     */
    inline bool contains(const key_type& key) const {
        return (__m_creators.find(key) != __m_creators.end());
    }

    /*!
     * \brief Retrieve number of registered creators
     * \return number of registered keys
     */
    inline size_t size() const {
        return __m_creators.size();
    }

    /*!
     * \brief Retrieve number of registered creators for specified key
     * \param key key of registered type
     * \return number of registered creators for specified key
     */
    inline size_t count(const key_type& key) const {
        return __m_creators.count(key);
    }

    /*!
     * \brief Test if class_factory is empty
     * \return true if class_factory hasn't got any registered creators, otherwise return false
     */
    inline bool empty() const {
        return __m_creators.empty();
    }

    /*!
     * \brief typeinfo
     * \return type_info for base class type
     */
    inline const std::type_info* typeinfo() const {
        return &typeid(_Base);
    }

protected:
    creator_map __m_creators;

};


_STDX_END


