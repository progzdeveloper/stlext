#include <catch.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include <stlext/functional/predicates.hpp>

TEST_CASE("functional/char_size", "[predicates]")
{
    std::cout << "sizeof(char): " << sizeof(char) << std::endl;
    //std::cout << "sizeof(char8_t): " << sizeof(char8_t) << std::endl;
    std::cout << "sizeof(char16_t): " << sizeof(char16_t) << std::endl;
    std::cout << "sizeof(char32_t): " << sizeof(char32_t) << std::endl;
    std::cout << "sizeof(wchar_t): " << sizeof(wchar_t) << std::endl;
}

TEST_CASE("functional/any_of(char)", "[predicates]")
{
    using namespace std;
    string test = "One,Two|Three.";
    string seps = ".,|";

    auto any_sep_il = stdx::is_any_of({ '.', ',', '|' });
    auto any_sep_cstr = stdx::is_any_of(".,|");
    auto any_sep_cont = stdx::is_any_of(seps);
    auto any_sep_range = stdx::is_any_of(begin(seps), end(seps));

    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_il));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_cstr));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_cont));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_range));
}


TEST_CASE("functional/none_of(char)", "[predicates]")
{
    using namespace std;
    string test = "...|,,,|...";
    string seps = ".,|";

    auto none_sep_il = stdx::is_none_of({ '.', ',', '|' });
    auto none_sep_cstr = stdx::is_none_of(".,|");
    auto none_sep_cont = stdx::is_none_of(seps);
    auto none_sep_range = stdx::is_none_of(begin(seps), end(seps));

    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_il));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_cstr));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_cont));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_range));
}

TEST_CASE("functional/any_of(char16_t)", "[predicates]")
{
    using namespace std;
    u16string test = u"One,Two|Three.";
    u16string seps = u".,|";

    auto any_sep_il = stdx::is_any_of({ u'.', u',', u'|' });
    auto any_sep_cstr = stdx::is_any_of(u".,|");
    auto any_sep_cont = stdx::is_any_of(seps);
    auto any_sep_range = stdx::is_any_of(begin(seps), end(seps));

    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_il));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_cstr));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_cont));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_range));
}


TEST_CASE("functional/none_of(char16_t)", "[predicates]")
{
    using namespace std;
    u16string test = u"...|,,,|...";
    u16string seps = u".,|";

    auto none_sep_il = stdx::is_none_of({ u'.', u',', u'|' });
    auto none_sep_cstr = stdx::is_none_of(u".,|");
    auto none_sep_cont = stdx::is_none_of(seps);
    auto none_sep_range = stdx::is_none_of(begin(seps), end(seps));

    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_il));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_cstr));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_cont));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_range));
}



TEST_CASE("functional/any_of(wchar_t)", "[predicates]")
{
    using namespace std;
    wstring test = L"One,Two|Three.";
    wstring seps = L".,|";

    auto any_sep_il = stdx::is_any_of({ L'.', L',', L'|' });
    auto any_sep_cstr = stdx::is_any_of(L".,|");
    auto any_sep_cont = stdx::is_any_of(seps);
    auto any_sep_range = stdx::is_any_of(begin(seps), end(seps));

    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_il));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_cstr));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_cont));
    REQUIRE_FALSE(test.end() == find_if(test.begin(), test.end(), any_sep_range));
}


TEST_CASE("functional/none_of(wchar_t)", "[predicates]")
{
    using namespace std;
    wstring test = L"...|,,,|...";
    wstring seps = L".,|";

    auto none_sep_il = stdx::is_none_of({ L'.', L',', L'|' });
    auto none_sep_cstr = stdx::is_none_of(L".,|");
    auto none_sep_cont = stdx::is_none_of(seps);
    auto none_sep_range = stdx::is_none_of(begin(seps), end(seps));

    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_il));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_cstr));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_cont));
    REQUIRE(test.end() == find_if(test.begin(), test.end(), none_sep_range));
}
