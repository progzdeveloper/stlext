#include <catch.hpp>

#include <iostream>
#include <sstream>
#include <string>

#include <stlext/iostreams/base64.hpp>
#include <stlext/iostreams/codecs.hpp>


TEST_CASE("iostreams/base64", "[iostreams]")
{
	using namespace stdx;
	using namespace std;

    /*string line;
	string b64 = "MzU0Niwgz9TL0i7TIDE4LjAzLjE3INfJ0szQ1cwucmFy";
	decode(b64.begin(), b64.end(), make_decoder<base64>(back_inserter(line)));
    REQUIRE(line == "3546, 18.03.17.rar");

	string s = "= ===@SGVs  b  G8?gR29!vZCBXb  3Js ZA==";
    s.erase((string::iterator)decode(s.begin(), s.end(), make_decoder<base64>(s.begin())), s.end());
    REQUIRE(s == "Hello Good World");*/
}


