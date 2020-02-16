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
#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <list>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string.h>

#if defined(STDX_CMPLR_GNU) && STDX_CMPLR_GNU < 500
#include <boost/tr1/regex.hpp>
#else
#include <regex>
#endif

#include "../compability/string_view"

#include "../platform/common.h"

_STDX_BEGIN

class option
{
public:
    typedef std::string string_type;

    option(const string_type& keys,
           const string_type& text)  :
        __m_keys(keys), __m_text(text),
        __m_required(false)
    {
        __m_required = __get_attribute('*');
        __m_implicit = __get_attribute('=');
    }

    template<class T>
    option(const string_type& keys,
           const string_type& text,
           const T& value) :
        __m_keys(keys), __m_text(text),
        __m_required(false)
    {
        __m_implicit = __get_attribute('=');
        __m_required = __get_attribute('*');
        std::ostringstream buf;
        buf << value;
        __m_value = buf.str();
    }

    const string_type& keys() const { return __m_keys; }
    const string_type& text() const { return __m_text; }
    const string_type& value() const { return __m_value; }

    bool required() const { return __m_required; }
    bool implicit() const { return __m_implicit; }

    static inline const option& help() {
        static const option _instance("h|help", "show help");
        return _instance;
    }

    static inline const option& usage() {
        static const option _instance("usage", "show usage");
        return _instance;
    }

    static inline const option& version() {
        static const option _instance("v|version", "show application version and description");
        return _instance;
    }

private:
    inline bool __get_attribute(char a) {
        size_t pos = __m_keys.find(a);
        bool result = pos != string_type::npos;
        if (result)
            __m_keys.erase(pos, 1);
        return result;
    }
private:
    string_type __m_keys, __m_text, __m_value;
    bool __m_required, __m_implicit;
};






class option_source
{
public:
    typedef std::string string_type;
    typedef stdx::string_view string_ref;

    struct value
    {
        string_type __m_str;

        value() {}
        value(const char* s) : __m_str(s) {}
        value(const string_type& s) : __m_str(s) {}
        inline value& operator= (const value& other) {
            __m_str.assign(other.__m_str);
            return (*this);
        }

        static const value& none() {
            static value _none;
            return _none;
        }

        bool empty() const {
            return __m_str.empty();
        }

        explicit operator bool() const {
            return (!__m_str.empty() &&
                    (__m_str == "on" ||
                     __m_str == "yes" ||
                     __m_str == "enabled"));
        }

        inline value& operator= (const string_type& other) {
            __m_str.assign(other);
            return (*this);
        }

        inline value& operator= (const string_ref& other) {
            __m_str.assign(other.data(), other.size());
            return (*this);
        }

        template<class T>
        inline T as() const {
            T result;
            std::istringstream iss(__m_str);
            iss >> result;
            return result;
        }

        template<class T>
        inline T as(unsigned radix) const {
            static_assert(std::is_integral<T>::value, "T is not an integral type");
            return T();
        }

    };

    typedef std::function<void(const value&)> binder_t;

    typedef std::list<option> option_list;
    typedef std::forward_list<value> value_list;
    typedef std::unordered_map<string_ref, const option*> option_map;
    typedef std::unordered_map<const option*, value*> optval_map;
    typedef std::unordered_map<const option*, binder_t> bind_map;

public:
    // throw on error (toe) or exit on error (eoe)
    enum flags { toe = 0, eoe = 1 };

    option_source() :
        __m_next(nullptr) {
    }

    virtual ~option_source() {
    }

    inline option_source& operator+= (const option_source& source)
    {
        if (__m_next == nullptr) {
            __m_next = const_cast<option_source*>(std::addressof(source));
            return (*this);
        }
        option_source* it = __m_next;
        while (it->__m_next != nullptr) {
            it = it->__m_next;
        }
        it->__m_next = const_cast<option_source*>(std::addressof(source));
        return (*this);
    }

    void parse(flags flags = toe)
    {
        using namespace std;
        try {
            __reset();
            __do_parse();
            __resolve_bindings();
        } catch (exception& e) {
            if (flags == toe)
                throw;
            cerr << e.what() << endl;
            exit(EXIT_FAILURE);
        }

        if (__m_next)
            __m_next->__notify(*this);
        for (option_source* src = __m_next; src != nullptr; src = src->__m_next) {
            src->parse(flags);
            if (src->__m_next)
                src->__m_next->__notify(*this);
        }
    }

    inline void operator()(flags flags = toe) {
        parse(flags);
    }

    inline size_t count(const char* key) const
    {
        auto optIt = __m_option_lut.find(key); // find option
        size_t n = __m_values_lut.count(optIt->second); // find value
        if (n != 0)
            return n;

        // recursively search in all successors
        for (option_source* src = __m_next; src != nullptr; src = src->__m_next) {
            n = src->count(key);
            if (n != 0)
                break;
        }
        return n; // not found
    }


    inline void emplace(const string_type& keys,
                        const string_type& text)
    {
        __emplace_option(binder_t(), keys, text);
    }

    inline void emplace(const string_type& keys,
                        const string_type& text,
                        const binder_t& binder)
    {
        __emplace_option(binder, keys, text);
    }

    template<class T>
    inline void emplace(const std::string& keys,
                        const T& v,
                        const std::string& text) {
        __emplace_option(binder_t(), keys, text, v);
    }

    template<class T>
    inline void emplace(const std::string& keys,
                        const T& v,
                        const std::string& text,
                        const binder_t& binder) {
        __emplace_option(binder, keys, text, v);
    }

    inline void emplace(const option& o) {
        __emplace_option(o);
    }

    template<class T>
    T find(const char* key) const {
        return find(key).as<T>();
    }

    const value& find(const char* key) const
    {
        auto optIt = __m_option_lut.find(key);
        if (optIt == __m_option_lut.end()) // unknown option
            return __m_next ? __m_next->find(key) : value::none();
        auto it = __m_values_lut.find(optIt->second);
        return (it != __m_values_lut.end() ? *(it->second) : value::none());
    }

    const value& operator[](const char* key) const {
        return find(key);
    }


protected:
	option_source& assign(const stdx::option_source& source)
	{
		if (&source == this)
			return (*this); // escape self-assignment

		for (auto it = source.__m_options.begin(); it != source.__m_options.end(); ++it) {
			this->emplace(*it);
		}
		for (auto it = source.__m_values_lut.begin(); it != source.__m_values_lut.end(); ++it) {
			auto optIt = __m_option_lut.find(it->first->keys());
			if (optIt != __m_option_lut.end())
				__put_value(optIt->second, it->second->__m_str);
		}

		return (*this);
	}

    virtual void __do_parse() {
        // does nothing
    }

    virtual void __notify(const option_source&) {
        // does nothing
    }


    template<class... _Args>
    inline void __emplace_option(const binder_t& binder, _Args&&... args)
    {
        using namespace std;
        // workaround known BUG in GCC 4.8
#if defined(STDX_CMPLR_GNU) && STDX_CMPLR_GNU < 500
        using namespace std::tr1;
#endif

        typedef sregex_iterator iter_type;

        //static regex rx("(?:-*)([a-zA-Z0-9_-]+)");
        static regex rx("[a-zA-Z0-9_\\/\\.\\-]+");
        static sregex_iterator __s_end;

        __m_options.emplace_back(forward<_Args>(args)...);
        const option* popt = addressof(__m_options.back());
        if (binder)
            __m_bindings.emplace(popt, binder);

        const string_type& keys = popt->keys();
        for (iter_type it(keys.begin(), keys.end(), rx); it != __s_end; ++it)
        {
            const char* p = addressof(*(keys.begin() + it->position()));
            __m_option_lut.emplace(string_ref(p, it->length()) , popt);
        }
    }

    inline void __emplace_option(const option& o) 
    {
        using namespace std;
        // workaround known BUG in GCC 4.8
#if defined(STDX_CMPLR_GNU) && STDX_CMPLR_GNU < 500
        using namespace std::tr1;
#endif

        typedef sregex_iterator iter_type;

        static regex rx("[a-zA-Z0-9_\\.\\-]+");
        static sregex_iterator __s_end;

         __m_options.emplace_back(o);

        const option* popt = addressof(o);
        if (popt != addressof(option::help()) &&
            popt != addressof(option::usage()) &&
            popt != addressof(option::version()))
        {
            popt = addressof(__m_options.back());
        }

        const string_type& keys = popt->keys();
        for (iter_type it(keys.begin(), keys.end(), rx); it != __s_end; ++it)
        {
            const char* p = addressof(*(keys.begin() + it->position()));
            __m_option_lut.emplace(string_ref(p, it->length()) , popt);
        }
    }

    void __reset()
    {
        __m_values_lut.clear();
        __m_values.clear();
        for (auto it = __m_options.begin(); it != __m_options.end(); ++it) {
            if (!it->value().empty())
                __put_value(std::addressof(*it), it->value());
        }
    }

    void __put_value(const option* opt, const string_type& val)
    {
        optval_map::iterator it = __m_values_lut.find(opt);
        if (it != __m_values_lut.end()) {
            (*(it->second)) = val;
        } else {
            __m_values.emplace_front(val);
            __m_values_lut.emplace(opt, std::addressof(__m_values.front()));
        }
    }

    void __resolve_bindings()
    {
        for (auto it = __m_bindings.begin(); it != __m_bindings.end(); ++it) {
            auto valIt = __m_values_lut.find(it->first); // search for value
            if (it->second)
                it->second(valIt == __m_values_lut.end() ? value::none() : *(valIt->second)); // call handler
        }
    }

    static inline string_ref __substr(string_type::const_iterator first,
                                    string_type::const_iterator last) {
        return {std::addressof(*first), static_cast<size_t>(std::distance(first, last))};
    }

    static string_type __format_keys(const option& opt, const char c = ',')
    {
        using namespace std;
#if defined(STDX_CMPLR_GNU) && STDX_CMPLR_GNU < 500
        using namespace std::tr1;
#endif
        string_type formatted;
        static regex rx("[a-zA-Z0-9_-]+");
        static sregex_iterator __s_end;
        const string_type& keys = opt.keys();
        for (sregex_iterator it(keys.begin(), keys.end(), rx); it != __s_end; )
        {
            formatted += (it->length() < 2 ? "-" : "--");
            formatted.append(keys.c_str() + it->position(), it->length());
            if (++it != __s_end) {
                formatted += c;
            }
        }
        return formatted;
    }

    static string_type __format_option(const option& opt, const char c = ',')
    {
        using namespace std;
        string_type formatted = __format_keys(opt, c);
        if (opt.implicit()) {
            formatted += "=<arg>";
        }
        if (!opt.value().empty()) {
            formatted += "(=";
            formatted += opt.value();
            formatted += ')';
        }
        return formatted;
    }

protected:
    bind_map    __m_bindings;
    option_list __m_options;
    value_list  __m_values;
    option_map  __m_option_lut;
    optval_map  __m_values_lut;

    option_source* __m_next;
};



template<>
inline bool option_source::value::as<bool>() const
{
    return (!__m_str.empty() &&
            (__m_str == "on" ||
             __m_str == "yes" ||
             __m_str == "enabled"));
}


template<>
inline std::string option_source::value::as<std::string>() const {
    return __m_str;
}


template<>
inline float option_source::value::as<float>() const
{
    return std::stof(__m_str, nullptr);
}


template<>
inline double option_source::value::as<double>() const
{
    return std::stod(__m_str, nullptr);
}

template<>
inline long double option_source::value::as<long double>() const
{
    return std::stold(__m_str, nullptr);
}

template<>
inline int16_t option_source::value::as<int16_t>() const
{
    return static_cast<int16_t>(std::stoi(__m_str, nullptr, 10));
}

template<>
inline int32_t option_source::value::as<int32_t>() const
{
    return std::stol(__m_str, nullptr, 10);
}

template<>
inline int64_t option_source::value::as<int64_t>() const
{
    return std::stoll(__m_str, nullptr, 10);
}

template<>
inline uint16_t option_source::value::as<uint16_t>() const
{
     return static_cast<uint16_t>(std::stoul(__m_str, nullptr, 10));
}

template<>
inline uint32_t option_source::value::as<uint32_t>() const
{
     return std::stoul(__m_str, nullptr, 10);
}

template<>
inline uint64_t option_source::value::as<uint64_t>() const
{
    return std::stoull(__m_str, nullptr, 10);
}




template<>
inline int16_t option_source::value::as<int16_t>(unsigned radix) const
{
    return static_cast<int16_t>(std::stoi(__m_str, nullptr, radix));
}

template<>
inline int32_t option_source::value::as<int32_t>(unsigned radix) const
{
    return std::stol(__m_str, nullptr, radix);
}

template<>
inline int64_t option_source::value::as<int64_t>(unsigned radix) const
{
    return std::stoll(__m_str, nullptr, radix);
}

template<>
inline uint16_t option_source::value::as<uint16_t>(unsigned radix) const
{
    return static_cast<uint16_t>(std::stoul(__m_str, nullptr, radix));
}

template<>
inline uint32_t option_source::value::as<uint32_t>(unsigned radix) const
{
    return std::stoul(__m_str, nullptr, radix);
}

template<>
inline uint64_t option_source::value::as<uint64_t>(unsigned radix) const
{
    return std::stoull(__m_str, nullptr, radix);
}

_STDX_END

