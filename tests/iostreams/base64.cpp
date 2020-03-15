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

    string buf, b64;

    string plain(256, '\0');
    for (size_t i = 0; i < plain.size(); i++) {
        plain[i] = std::string::traits_type::to_char_type(i);
    }

    b64.reserve(256 * 1.5);
    encode(plain.begin(), plain.end(), make_encoder<base64>(back_inserter(b64)));
    decode(b64.begin(), b64.end(), make_decoder<base64>(back_inserter(buf)));
    REQUIRE(buf == plain);


    /*
    string b64 = "MzU0Niwgz9TL0i7TIDE4LjAzLjE3INfJ0szQ1cwucmFy";
    decode(b64.begin(), b64.end(), make_decoder<base64>(back_inserter(line)));
    REQUIRE(line == "3546, 18.03.17.rar");

    s.erase((string::iterator)decode(s.begin(), s.end(), make_decoder<base64>(s.begin())), s.end());
    REQUIRE(s == "Hello Good World");*/
}


