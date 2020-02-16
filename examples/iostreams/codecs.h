#pragma once
#include <iostream>
#include <string>
#include <stlext/iostreams/codecs.hpp>
#include <stlext/iostreams/base16.hpp>
#include <stlext/iostreams/base64.hpp>


static void codecs_example()
{
	using namespace std;
	
	using stdx::make_decoder;
	using stdx::make_encoder;

	string source = "This is a long string of text!";
	string b16str, b64str, b16res, b64res;

	// encode using Base16 encoder
	stdx::encode(source.begin(), source.end(), make_encoder<stdx::base16>(back_inserter(b16str)));
	// decode using Base16 decoder
	stdx::decode(b16str.begin(), b16str.end(), make_decoder<stdx::base16>(back_inserter(b16res)));


	// encode using Base64 encoder
	stdx::encode(source.begin(), source.end(), make_encoder<stdx::base64>(back_inserter(b64str)));
	// decode using Base64 decoder
	stdx::decode(b64str.begin(), b64str.end(), make_decoder<stdx::base64>(back_inserter(b64res)));

	// Print out the results
	cout << "Source string:  " << source << endl << endl;

	cout << "Base16 encoded: " << b16str << endl;
	cout << "Base16 decoded: " << b16res << endl << endl;

	cout << "Base64 encoded: " << b64str << endl;
	cout << "Base64 decoded: " << b64res << endl;
	
}


