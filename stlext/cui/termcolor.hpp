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

#pragma once

#include <cstdint>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <io.h>
#include <windows.h>
#define _isatty isatty
#else
#include <unistd.h>
#endif

#include "../platform/common.h"

_STDX_BEGIN

enum color : uint64_t {
#if defined(_WIN32) || defined(_WIN64)
    gray    = 0,
    grey    = 0,
    red     = FOREGROUND_RED,
    green   = FOREGROUND_GREEN,
    yellow  = FOREGROUND_GREEN | FOREGROUND_RED,
    blue    = FOREGROUND_BLUE,
    magenta = FOREGROUND_BLUE | FOREGROUND_RED,
    cyan    = FOREGROUND_BLUE | FOREGROUND_GREEN,
    white   = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
#else
    gray    = 0x0000006d30335b1b, // ("\033[30m")   + 0x10000 for background color ("\033[40m")
    grey    = 0x0000006d30335b1b, // ("\033[30m")   + 0x10000 for background color ("\033[40m")
    red     = 0x0000006d31335b1b, // ("\033[31m")   + 0x10000 for background color ("\033[41m")
    green   = 0x0000006d32335b1b, // ("\033[32m")   + 0x10000 for background color ("\033[42m")
    yellow  = 0x0000006d33335b1b, // ("\033[33m")   + 0x10000 for background color ("\033[43m")
    blue    = 0x0000006d34335b1b, // ("\033[34m")   + 0x10000 for background color ("\033[44m")
    magenta = 0x0000006d35335b1b, // ("\033[35m")   + 0x10000 for background color ("\033[45m")
    cyan    = 0x0000006d36335b1b, // ("\033[36m")   + 0x10000 for background color ("\033[46m")
    white   = 0x0000006d37335b1b  // ("\033[37m")   + 0x10000 for background color ("\033[47m")
#endif
};

enum effect : uint16_t {
#if defined(_WIN32) || defined(_WIN64) // only limited subset of effects supported by Windows
    underline =  COMMON_LVB_UNDERSCORE,
    reverse   =  COMMON_LVB_REVERSE_VIDEO
#else
    bold      = 0x01, // bold      = 0x000000006d315b1b, ("\033[1m" )
    dark      = 0x02, // dark      = 0x000000006d325b1b, ("\033[2m" )
    underline = 0x08, // underline = 0x000000006d345b1b, ("\033[4m" )
    blink     = 0x10, // blink     = 0x000000006d355b1b, ("\033[5m" )
    reverse   = 0x40, // reverse   = 0x000000006d375b1b, ("\033[7m" )
    concealed = 0x80, // concealed = 0x000000006d385b1b, ("\033[8m" )
    crossed   = 0x100 // crossed   = 0x000000006d395b1b, ("\033[9m" )
#endif
};

/*!
 * \brief is_atty detect whether a given `std::ostream` object refers
 * to a terminal type device.
 * \param stream stream instance
 * \return true if stream refers to a terminal, otherwise return false
 */
inline bool is_atty(std::ostream& stream);

namespace detail
{
    // An index to be used to access a private storage of I/O streams. See
    // colorize / nocolorize I/O manipulators for details.
    static const int colorize_index = std::ios_base::xalloc();


    /*!
     * \brief is_colorized
     * \param stream
     * \return
     */
    inline bool is_colorized(std::ostream& stream);


#if defined(_WIN32) || defined(_WIN64)
    /*!
     * \brief set_console_attributes change Windows Terminal colors attribute. If some
     * parameter is `-1` then attribute won't changed.
     * \param stream basic_ostream object reference
     * \param foreground foreground value
     * \param background background value
     * \return basic_ostream object reference
     */
    std::ostream& set_console_attributes(std::ostream& stream, int foreground, int background);
#endif
} // end namepsace detail


/*!
 * \brief background change background color of console text
 * \tparam _Color new background color
 * \param stream basic_ostream instance reference
 * \return basic_ostream instance reference
 */
template<color _Color>
inline std::ostream& background(std::ostream& stream)
{
    if (!detail::is_colorized(stream))
        return (stream);

#if !defined(_WIN32) && !defined(_WIN64)
    static constexpr const uint64_t c = _Color + 0x10000;
    return (stream.write(reinterpret_cast<const char*>(&c), sizeof(uint64_t)));
#else
    // N.B. to get background color from foreground on Windows we
    // need shift one bit left i.e. bg = fg << 1;
    return detail::set_console_attribute(stream, -1, _Color << 1);
#endif
}

/*!
 * \brief foreground change foreground color of console text
 * \tparam _Color new background color
 * \param stream basic_ostream instance reference
 * \return basic_ostream instance reference
 */
template<color _Color>
inline std::ostream& foreground(std::ostream& stream)
{
    if (!detail::is_colorized(stream))
        return (stream);

#if !defined(_WIN32) && !defined(_WIN64)
    static constexpr const uint64_t c = _Color;
    return (stream.write(reinterpret_cast<const char*>(&c), sizeof(uint64_t)));
#else
    return detail::set_console_attribute(stream, _Color,  -1);
#endif
}

/*!
 * \brief setcolors change both foreground and background color of console text
 * \tparam _Background new background color
 * \tparam _Foreground new foreground color
 * \param stream basic_ostream instance reference
 * \return basic_ostream instance reference
 */
template<color _Background, color _Foreground>
inline std::ostream& setcolors(std::ostream& stream) {
    if (!detail::is_colorized(stream))
        return (stream);
    return (stream << background<_Background> << foreground<_Foreground>);
}


/*!
 * \brief style change console text effect
 * \tparam _Color new background color
 * \param stream basic_ostream instance reference
 * \return basic_ostream instance reference
 */
template<uint16_t _Effect>
inline std::ostream& style(std::ostream& stream)
{
    if (!detail::is_colorized(stream))
        return (stream);

    // Windows doesn't support all terminal styles
#if !defined(_WIN32) && !defined(_WIN64)
    static constexpr const uint64_t base = 0x000000006d315b1b;
    uint64_t s;
    for (size_t i = 0; i <= 9; i++) {
        if (((1 << i) & _Effect)) {
            s = base + i * 0x10000;
            stream.write(reinterpret_cast<const char*>(&s), sizeof(uint64_t));
        }
    }
    return (stream);
#else
    return set_console_attributes(stream, _Effect, -1);
#endif
}



/*!
 * \brief colorize force colorization on stream
 * \param stream
 * \return
 */
inline std::ostream& colorize(std::ostream& stream)
{
    stream.iword(detail::colorize_index) = 1L;
    return stream;
}


/*!
 * \tparam _Effect text effect
 * \tparam _Background background color
 * \tparam _Foreground foreground color
 * \param stream basic_ostream instance reference
 * \return basic_ostream instance reference
 */
template<uint16_t _Effect, color _Background, color _Foreground>
inline std::ostream& colorize(std::ostream& stream) {
    if (!detail::is_colorized(stream))
        stream << colorize;
    return (stream << style<_Effect> << background<_Background> << foreground<_Foreground>);
}




/*!
 * \brief uncolorize reset all colorization
 * \param stream basic_ostream object reference
 * \return basic_ostream object reference
 */
inline std::ostream& uncolorize(std::ostream& stream)
{
    stream.iword(detail::colorize_index) = 0L;

#if !defined(_WIN32) && !defined(_WIN64)
    static constexpr const uint64_t c = 0x0000006d30305b1b;
    return (stream.write(reinterpret_cast<const char*>(&c), sizeof(uint64_t)));
#else
    return detail::set_console_attributes(stream, -1, -1);
#endif
}

/*!
 * \brief is_atty determine if stream is a terminal type device
 * \param stream basic_ostream object reference
 * \return true if stream is a terminal type device, otherwise return false
 */
inline bool is_atty(std::ostream& stream)
{
    FILE* fd = nullptr;
    if (&stream == &std::cout)
        fd = stdout;
    else if ((&stream == &std::cerr) || (&stream == &std::clog))
        fd = stderr;
    else
        return false;
    return ::isatty(fileno(fd));
}



namespace detail
{

    inline bool is_colorized(std::ostream& stream) {
        return is_atty(stream) || static_cast<bool>(stream.iword(colorize_index));
    }


#if defined(_WIN32) || defined(_WIN64)
    //! Change Windows Terminal colors attribute. If some
    //! parameter is `-1` then attribute won't changed.
    std::ostream& set_console_attributes(std::ostream& stream, int foreground, int background)
    {
        static WORD defaultAttributes = 0;

        // Windows doesn't have ANSI escape sequences and so we use special
        // API to change Terminal output color. That means we can't
        // manipulate colors by means of "std::stringstream" and hence
        // should do nothing in this case.
        if (!is_atty(stream))
            return;

        // get terminal handle
        HANDLE hTerminal = INVALID_HANDLE_VALUE;
        if (&stream == &std::cout || &stream == &std::clog)
            hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
        else if (&stream == &std::cerr)
            hTerminal = GetStdHandle(STD_ERROR_HANDLE);

        // save default terminal attributes if it unsaved
        if (!defaultAttributes)
        {
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(hTerminal, &info))
                return;
            defaultAttributes = info.wAttributes;
        }

        // restore all default settings
        if (foreground == -1 && background == -1)
        {
            SetConsoleTextAttribute(hTerminal, defaultAttributes);
            return;
        }

        // get current settings
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(hTerminal, &info))
            return;

        if (foreground != -1)
        {
            info.wAttributes &= ~(info.wAttributes & 0x0F);
            info.wAttributes |= static_cast<WORD>(foreground);
        }

        if (background != -1)
        {
            info.wAttributes &= ~(info.wAttributes & 0xF0);
            info.wAttributes |= static_cast<WORD>(background);
        }

        SetConsoleTextAttribute(hTerminal, info.wAttributes);

        return (stream);
    }
#endif
} // end namespace detail

_STDX_END

