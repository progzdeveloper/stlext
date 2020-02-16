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
#include "option_source.hpp"

_STDX_BEGIN

#if 0
class enviroment_map : 
	public option_source
{
public:

	typedef std::unordered_map<string_type, value> value_map;

	enviroment_map(char** env) : 
		_env(env) {
	}

	inline void push(const option& opt) {
		push_option(opt);
	}

	inline void push(const string_type& _Key) {
		emplace_option(_Key, string_type(), string_type(), string_type());
	}

	template<typename T>
	inline void push(const string_type& _Key, const T& _DefaultValue = T()) {
		emplace_option(_Key, _DefaultValue, string_type(), string_type());
	}

protected:
	void parse(flags _Flags = toe) 
	{
		if (!_env)
			return;

		string_set required(reqkeyset);
		const option* opt;
		for (; *_env != 0; ++_env) 
		{
			string_type s(*_env);
			string_type::size_type n = s.find('=');
			if (n == string_type::npos)
				continue;

			string_type key = s.substr(0, n);
			string_type value = s.substr(n + 1);
			args_map::const_iterator it = optmap.find(key);
			if (it == optmap.end()) {
				continue;
			}
			opt = it->second;
			put_value(opt, value);
			required.erase(opt->keys());
		}

		if (!required.empty()) {
			string_type errtext, err;
			for_each(required.begin(), required.end(), [&](const string_type& rkey) {
				format_error(err, "required enviroment variable '%s' was not found...\n", rkey.c_str());
				errtext += err;
			});
			throw std::invalid_argument(errtext);
		}
	}

private:
	char** _env;
};
#endif

_STDX_END
