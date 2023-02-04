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
#include <string_view>
#include <iterator>

_STDX_BEGIN

template<class _Char>
struct cmdline_argset
{
public:
    using value_type = std::basic_string_view<_Char>;

    class iterator
    {
        friend class cmdline_argset;
        iterator(_Char const* const* argv)
            : argp_(argv)
        {
            if (argp_ && *argp_)
                argval_ = *argp_;
        }

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using value_type = std::basic_string_view<_Char>;

        using const_reference = value_type;
        using reference = value_type;

        using const_pointer = const value_type*;
        using pointer = const_pointer;

        const_reference operator*() const { return argval_; }
        const_pointer operator ->() const { return &argval_; }

        iterator& operator++() { return advance(1); }
        iterator operator++(int) { iterator t(*this); advance(1); return t; }

        iterator& operator--() { return advance(-1); }
        iterator operator--(int) { iterator t(*this); advance(-1); return t; }

        iterator& operator+=(difference_type n) { return advance(n); }
        iterator operator+(difference_type n) const { std::next(*this, n); }

        iterator& operator-=(difference_type n) { return advance(-n); }
        iterator operator-(difference_type n) const { std::prev(*this, n); }


        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return (lhs.argp_ == rhs.argp_ && (*lhs.argp_ == *rhs.argp_));
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        constexpr iterator& advance(std::ptrdiff_t d)
        {
            argp_ += d;
            argval_ = (argp_ && *argp_) ? value_type{*argp_} : value_type{};
            return (*this);
        }

    private:
        value_type argval_;
        _Char const* const* argp_ = nullptr;
    };

    cmdline_argset() = delete;

    explicit cmdline_argset(const int argc, _Char const* const* argv)
        : argc_(static_cast<size_t>(argc))
        , argv_(argv)
    {
    }

    value_type operator[](size_t i) const { return value_type{ *(argv_ + i) }; }

    iterator begin() const { return iterator(argv_); }
    iterator end() const { return iterator(argv_ + argc_); }

    size_t size() const { return argc_; }
    size_t count() const { return argc_ > 0 ? (argc_ - 1) : 0; }

    value_type path() const { return (argv_ && *argv_)  ? value_type{ *argv_ } : value_type{}; }

private:
    size_t argc_;
    _Char const* const* argv_;
};

using ccmdline_argset = cmdline_argset<char>;
using wcmdline_argset = cmdline_argset<wchar_t>;

_STDX_END


