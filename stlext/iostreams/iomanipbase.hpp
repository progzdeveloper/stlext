// Copyright (c) 2020, Michael Polukarov (Russia).
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


#ifndef IOMANIPBASE_HPP
#define IOMANIPBASE_HPP

#include <iostream>

#include "../platform/common.h"

_STDX_BEGIN

class iomanip_base
{
public:
    iomanip_base() :
        error_m(std::ios_base::goodbit) {}

protected:

    template <typename StreamType>
    std::ios_base::iostate handle_error(StreamType& stream) const
    {
        std::ios_base::iostate err(error_m);
        try { throw; }
        catch (std::bad_alloc&)
        {
            error_m |= std::ios_base::badbit;

            std::ios_base::iostate exception_mask(stream.exceptions());

            if (exception_mask & std::ios_base::failbit && !(exception_mask & std::ios_base::badbit))
                stream.setstate(err);
            else if (exception_mask & std::ios_base::badbit) {
                try { stream.setstate(err); }
                catch (std::ios_base::failure&) { }
                throw;
            }
        }
        catch (...)
        {
            error_m |= std::ios_base::failbit;

            std::ios_base::iostate exception_mask(stream.exceptions());

            if ((exception_mask & std::ios_base::badbit) && (err & std::ios_base::badbit))
                stream.setstate(err);
            else if (exception_mask & std::ios_base::failbit) {
                try { stream.setstate(err); }
                catch (std::ios_base::failure&) { }
                throw;
            }
        }
        return err;
    }

    mutable std::ios_base::iostate  error_m;
};



template<class _Impl>
class imanip : virtual protected iomanip_base
{
public:

    imanip() : iomanip_base() {}

    template <class _Elem, class _Traits>
    friend inline std::basic_istream<_Elem, _Traits>&
    operator>> ( std::basic_istream<_Elem, _Traits>& stream,
                 imanip& manip )
    {
        return (stream.good() ? manip(stream) : stream);
    }
protected:
    template <class _Elem, class _Traits>
    inline std::basic_istream<_Elem, _Traits>& operator()(std::basic_istream<_Elem, _Traits>& stream)
    {
        if (error_m != std::ios_base::goodbit) {
            stream.setstate(error_m);
        } else {
            std::ios_base::iostate err(error_m);
            try {
                static_cast<_Impl*>(this)->do_get(stream);
            }
            catch (...) {
                err = handle_error(stream);
            }
            if (err)
                stream.setstate(err);
        }
        return stream;
    }
};





template<class _Impl>
class omanip : virtual protected iomanip_base
{
public:
    omanip() : iomanip_base() {}

    template <class _Elem, class _Traits>
    friend inline std::basic_ostream<_Elem, _Traits>&
    operator<< ( std::basic_ostream<_Elem, _Traits>& stream,
                 const omanip& manip )
    {
        return (stream.good() ? manip(stream) : stream);
    }

protected:
    template<class _Char, class _Traits>
    inline std::basic_ostream<_Char, _Traits>& operator()(std::basic_ostream<_Char, _Traits>& stream) const
    {
        if (error_m != std::ios_base::goodbit) {
            stream.setstate(error_m);
        } else {
            std::ios_base::iostate err(error_m);
            try {
                static_cast<const _Impl*>(this)->do_put(stream);
            }
            catch (...) {
                err = handle_error(stream);
            }
            if (err)
                stream.setstate(err);
        }
        return stream;
    }
};


_STDX_END


#endif // IOMANIPBASE_HPP
