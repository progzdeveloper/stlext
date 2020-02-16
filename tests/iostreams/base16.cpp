#include <catch.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#include <stlext/iostreams/base16.hpp>
#include <stlext/iostreams/codecs.hpp>

TEST_CASE("iostreams/base16", "[iostreams]")
{
	using namespace stdx;
	using namespace std;
	const char* hexline = "33343536370a0d";
	size_t n = (strlen(hexline) / 2) + 1;
	char* byteline = (char*)malloc(n);
	::memset(byteline, 0, n);
	decode(hexline, hexline + strlen(hexline), make_decoder<base16>(byteline));

	REQUIRE(strcmp("34567\n\r", byteline) == 0);
	::free(byteline); 

	{
		ostringstream buffer;
		string s = "HelloWorld\x1\x2\x3\x4\x5\xff";
		buffer << "Hex: ";
        encode(s.begin(), s.end(), make_encoder<base16>(std::ostreambuf_iterator<char>(buffer), ios::uppercase));
		string line = buffer.str();
		REQUIRE("Hex: 48656C6C6F576F726C640102030405FF" == line);
	}

	{
		ostringstream buffer;
		string s = "HelloWorld\x1\x2\x3\x4\x5\xff";
		buffer << "Hex: ";
        encode(s.begin(), s.end(), make_encoder<base16>(std::ostreambuf_iterator<char>(buffer)));
		string line = buffer.str();
		REQUIRE("Hex: 48656c6c6f576f726c640102030405ff" == line);
	}
}


