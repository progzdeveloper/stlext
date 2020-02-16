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
#include <iomanip>
#include <stdexcept>
#include "option_source.hpp"

_STDX_BEGIN

/*!
\brief class cmdline provides interaction with command line
*/
class command_line :
    public option_source
{
    command_line() = delete;
    command_line(const command_line&) = delete;
    command_line& operator=(const command_line&) = delete;

public:
    command_line(int argc, char** argv,
                 const string_type& version = string_type()) :
        __m_version(version),
        __m_argc(argc),
        __m_argv(argv)
    {
    }

    ~command_line() {
    }


    std::ostream& help(std::ostream& stream) const
    {
        using namespace std;

        for (auto it = __m_options.begin(); it != __m_options.end(); ++it)
        {
            stream << "   " << setw(24) << left << __format_option(*it) << ' ';
            if (it->required()) stream << "*";
            stream << it->text() << endl;
        }
        return (stream);
    }

    std::ostream& usage(std::ostream& stream) const
    {
        using namespace std;
        stream << "Usage: ";
        if (__m_argv && (*__m_argv))
            stream << __application_name(*__m_argv) << ' ';

        for (auto it = __m_options.begin(); it != __m_options.end(); ++it) {
            stream << '[' << __format_option(*it, '|') << ']';
        }
        return (stream << endl);
    }

    std::ostream& version(std::ostream& stream) const
    {
        using namespace std;
        if (__m_version.empty()) {
            if (__m_argv && (*__m_argv))
                return (stream << __application_name(*__m_argv) << endl);
        }
        return (stream << __m_version << endl);
    }

    // option_source interface
protected:
    inline void __do_parse()
    {
        using namespace std;

        string_ref key;
        string_ref val;
        const option* opt = nullptr;
        const char* arg = nullptr;

        if (!__m_argv)
            return;

        std::unordered_set<const option*> required_set;
        for (auto it = __m_options.begin(); it != __m_options.end(); ++it) {
            if (it->required()) {
                required_set.emplace(addressof(*it));
            }
        }

        for (int i = 1; i < __m_argc; i++)
        {
            arg = __m_argv[i];

            size_t s = strlen(arg);
            key = __parse_key(arg, s);
            val = __parse_val(arg, s);

            if (key.empty())
                continue;

            if (key.size() == 1 && strncmp(arg, "-", 1))
                continue;

            if (key.size() > 1 && strncmp(arg, "--", 2))
                continue;


            auto it = __m_option_lut.find(key);
            if (it == __m_option_lut.end()) {
                // ### handle unrecognized options
                throw runtime_error(string("unknown option '") + key.data() + '\'');
            }
            opt = it->second;

            if (opt == addressof(option::help())) {
                version(cout) << endl;
                usage(cout) << endl;
                help(cout) << endl;
                exit(EXIT_SUCCESS);
            }
            if (opt == addressof(option::usage())) {
                usage(cout) << endl;
                exit(EXIT_SUCCESS);
            }
            if (opt == addressof(option::version())) {
                version(cout) << endl;
                exit(EXIT_SUCCESS);
            }
            if (opt->implicit() && val.empty()) {
                 throw runtime_error("required argument for option '" + __format_keys(*opt) + "' is missing. ");
            }
            __put_value(opt, (string_type)val);
            auto rit = required_set.find(opt);
            if (rit != required_set.end()) {
                required_set.erase(rit);
            }
        }

        if (!required_set.empty()) {
            string_type errtext;
            for_each(required_set.begin(), required_set.end(), [&](const option* o) {
                errtext += "required option '";
                errtext += __format_keys(*o);
                errtext += "' was not found...\n";
            });
            throw std::runtime_error(errtext);
        }
    }


private:
    static inline string_ref __parse_key(const char* arg, size_t length)
    {

        using namespace std;
        if (arg == nullptr)
            return string_ref();

        const char* first = std::find_if(arg, arg + length, [](char c) { return (c != '-'); });
        if (first == arg + length)
            return string_ref();

        const char* last = std::find(first, arg + length, '=');
        return string_ref(first, distance(first, last));
    }

    static inline string_ref __parse_val(const char* arg, size_t length)
    {
        using namespace std;
        if (arg == nullptr)
            return string_ref();

        size_t n = std::distance(arg, std::find(arg, arg + length, '='));
        if (n >= length - 1)
            return string_ref();

        const char* first = arg + n + 1;
        const char* last = arg + length;
        return string_ref(first, distance(first, last));
    }

    static inline string_ref __application_name(const char* arg)
    {
        if (!arg)
            return string_ref();

        string_ref appname(arg);
        auto it = find_if(appname.rbegin(), appname.rend(),
                     [](char c) {
            return c == '/' || c == '\\';
        });
        appname.remove_prefix(distance(it, appname.rend()));
        return appname;
    }

private:
    string_type __m_version;
    const int __m_argc;
    const char* const* __m_argv;
};


_STDX_END
