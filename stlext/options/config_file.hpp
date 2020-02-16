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
#include <fstream>
#include <stdexcept>
#include "option_source.hpp"

_STDX_BEGIN

class config_file :
    public option_source
{

public:
    config_file(const string_type& filepath = string_type()) :
        __m_path(filepath), __m_src(nullptr)
    {
    }

    void bind(const option_source& source, const char* key)
    {
        __m_src = std::addressof(source);
        __m_optkey = key;
    }

	const string_type& filepath() const {
		return __m_path;
	}

    // option_source interface
protected:
    void __do_parse()
    {
        using namespace std;
#if defined(STDX_CMPLR_GNU) && STDX_CMPLR_GNU < 500
        using namespace std::tr1;
#endif
        //static const regex rx("(?<!\\S)([a-zA-Z0-9_\\.\\-\\/]+)\\s+=\\s+(?:\"(.*)\")(?!\\S)");
        //static const regex rx("(?:^\\s*)([a-zA-Z0-9_\\/\\.\\-]+)\\s+=\\s+(?:\"(.*)\")");
        static const regex rx("(\\w+)=(.*)");

        smatch results;
        string_type line;

        if (__m_path.empty()) {
            throw std::runtime_error("no configuration file path was specified");
        }

        ifstream ifs(__m_path);
        if (!ifs) {
            throw std::runtime_error("failed to read file '" + __m_path +
                                     "': " + system_category().message(errno));
            return;
        }

        std::unordered_set<const option*> required_set;
        for (auto it = __m_options.begin(); it != __m_options.end(); ++it) {
            if (it->required()) {
                required_set.emplace(addressof(*it));
            }
        }

        while (getline(ifs, line))
        {
            if (line.empty())
                continue;

            if (line == "#!@end")
                break;

            if (!regex_search(line, results, rx))
                continue;

            string_ref key(line.data() + results.position(1), results.length(1));
            string_ref val(line.data() + results.position(2), results.length(2));
            auto it = __m_option_lut.find(key);
            if (it == __m_option_lut.end()) {
                // ### handle unrecognized options
                continue;
            }

            const option* opt = it->second;
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
                errtext += o->keys();
                errtext += "' was not found...\n";
            });
            throw std::runtime_error(errtext);
        }

		__do_read(ifs);
    }

	virtual void __do_read(std::istream&) {}

    void __notify(const option_source& source) {
        if (std::addressof(source) == __m_src && !__m_optkey.empty()) {
            const value& val = source.find(__m_optkey.c_str());
            if (!val.empty())
                __m_path = val.as<string_type>();
        }
    }

	void reset_path(const std::string& path) {
		__m_path = path;
	}

private:
	std::string __m_path;
	std::string __m_optkey;
	const option_source* __m_src;
};

_STDX_END

