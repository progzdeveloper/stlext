#include <catch.hpp>

#include <cstring>
#include <vector>
#include <sstream>


#include <stlext/compact/string.hpp>

TEST_CASE("compact/string.creation", "[compact]")
{
    compact::string s;
    REQUIRE(s.size() == 0);
    REQUIRE(s.empty());

    compact::string s0 = { '0', '1', '2', '3' };
    REQUIRE(s0.size() == 4);
    REQUIRE(std::memcmp(s0.data(), "0123", 5) == 0);

    std::string other = "std";

    compact::string s1(other);
    REQUIRE(s1.size() == 3);
    REQUIRE(std::memcmp(s1.c_str(), other.c_str(), other.size() + 1) == 0);


    compact::tagged_string<2> tstr("432");
    REQUIRE(tstr.size() == 3);
    REQUIRE(std::memcmp(tstr.c_str(), "432", 4) == 0);

    const char* text = "Some text: Hello C++ World!";
    std::stringstream buf;
    buf << text;
    compact::string str(std::istreambuf_iterator<char>(buf), std::istreambuf_iterator<char>{});
    REQUIRE(str == text);
    
    compact::string sstr(4, 'a');
    REQUIRE(sstr == "aaaa");


    compact::string s2(tstr);
    REQUIRE(s2.size() == tstr.size());
    REQUIRE(std::memcmp(s2.c_str(), tstr.data(), tstr.size() + 1) == 0);

    compact::string s3 = "abc";
    REQUIRE(s3.size() == 3);
    REQUIRE(std::memcmp(s3.data(), "abc", 4) == 0);

    compact::string s4("aa-aa-aa", 5);
    REQUIRE(s4.size() == 5);
    REQUIRE(std::memcmp(s4.c_str(), "aa-aa", 6) == 0);
}

TEST_CASE("compact/string.assign", "[compact]")
{
    std::string stdstr = "abcde";
    compact::tagged_string<1> tstr = "432";
    const char* cstr = "0123";

    compact::string s;
    s = stdstr;
    REQUIRE(s.size() == stdstr.size());
    REQUIRE(std::memcmp(s.data(), stdstr.data(), stdstr.size() + 1) == 0);

    s = tstr;
    REQUIRE(s.size() == tstr.size());
    REQUIRE(std::memcmp(s.data(), tstr.data(), tstr.size() + 1) == 0);

    s = cstr;
    REQUIRE(s.size() == std::strlen(cstr));
    REQUIRE(std::memcmp(s.data(), cstr, std::strlen(cstr) + 1) == 0);

    s = s;
    REQUIRE(s.size() == std::strlen(cstr));
    REQUIRE(std::memcmp(s.data(), cstr, std::strlen(cstr) + 1) == 0);

    s.assign(cstr, 2);
    REQUIRE(s.size() == 2);
    REQUIRE(std::memcmp(s.data(), "01", 3) == 0);
    REQUIRE(s.front() == '0');
    REQUIRE(s.back() == '1');

    s.assign(3, 'a');
    REQUIRE(s.size() == 3);
    REQUIRE(std::memcmp(s.data(), "aaa", 4) == 0);

    s.assign({'5', '4', '3', '2', '1'});
    REQUIRE(s.size() == 5);
    REQUIRE(std::memcmp(s.data(), "54321", 6) == 0);

    s.assign(8, 'x');
    REQUIRE(s.size() == 8);
    REQUIRE(std::memcmp(s.data(), "xxxxxxxx", 9) == 0);

    s.assign(8, 'a');
    REQUIRE(s.size() == 8);
    REQUIRE(std::memcmp(s.data(), "aaaaaaaa", 9) == 0);

    s.assign("01234567");
    REQUIRE(s.size() == 8);
    REQUIRE(std::memcmp(s.data(), "01234567", 9) == 0);

    s.assign("");
    REQUIRE(s.empty());

    s.assign("1");
    REQUIRE(s.front() == '1');

    s.assign(0, '\0'); // clear
    REQUIRE(s.empty());

    REQUIRE_THROWS(s.assign(1 << 17, 'a'));
    REQUIRE_THROWS(s.assign(std::string(1 << 17, 'a')));
}

TEST_CASE("compact/string.resize", "[compact]")
{
    compact::string s("ha");
    REQUIRE(s.size() == 2);

    s += s;
    REQUIRE(s.size() == 4);
    REQUIRE(std::memcmp(s.data(), "haha", 5) == 0);

    s += s;
    REQUIRE(s.size() == 8);
    REQUIRE(std::memcmp(s.data(), "hahahaha", 9) == 0);


    s.erase(0, 4);
    REQUIRE(s.size() == 4);
    REQUIRE(std::memcmp(s.data(), "haha", 5) == 0);

    s.insert(3, 0, '\0'); // does nothing
    s.insert(3, ""); // does nothing
    s.erase(0, compact::string::size_type(0)); // does nothing

    s.insert(4, "-haha");
    REQUIRE(s.size() == 9);
    REQUIRE(std::memcmp(s.data(), "haha-haha", 10) == 0);

    std::string str("123");
    s.insert(0, str);
    REQUIRE(s.size() == 12);
    REQUIRE(std::memcmp(s.data(), "123haha-haha", 13) == 0);

    s.erase(s.begin() + 3, s.begin() + 7);
    REQUIRE(s.size() == 8);
    REQUIRE(std::memcmp(s.data(), "123-haha", 9) == 0);

    s.resize(12, 'x');
    REQUIRE(s.size() == 12);
    REQUIRE(std::memcmp(s.data(), "123-hahaxxxx", 13) == 0);

    s.resize(3);
    REQUIRE(s.size() == 3);
    REQUIRE(std::memcmp(s.data(), "123", 4) == 0);

    s.resize(5);
    REQUIRE(s.size() == 5);
    REQUIRE(std::memcmp(s.data(), "123\0\0", 6) == 0);

    s.resize(2, 'a');
    REQUIRE(s.size() == 2);
    REQUIRE(std::memcmp(s.data(), "12", 3) == 0);

    s.clear();
    REQUIRE(s.size() == 0);
    REQUIRE(s.empty());

    REQUIRE_THROWS(s.insert(0, std::string(1ULL << 17, 'a')));
    REQUIRE_THROWS(s.resize(1<<17));
}

TEST_CASE("compact/string.front", "[compact]")
{
    compact::string s1 = "h";
    REQUIRE(s1.front() == 'h');
    s1 = { 'h', 'a' };
    REQUIRE(s1.front() == 'h');
}

TEST_CASE("compact/string.back", "[compact]")
{
    compact::string s1 = "h";
    REQUIRE(s1.back() == 'h');
    s1 = { 'h', 'a' };
    REQUIRE(s1.back() == 'a');
}

TEST_CASE("compact/string.push_back", "[compact]")
{
     compact::string s;
     s.push_back('0'); REQUIRE(s.size() == 1);  REQUIRE(std::memcmp(s.data(), "0", 2) == 0);
     s.push_back('1'); REQUIRE(s.size() == 2);  REQUIRE(std::memcmp(s.data(), "01", 3) == 0);
     s.push_back('2'); REQUIRE(s.size() == 3);  REQUIRE(std::memcmp(s.data(), "012", 4) == 0);
     s.push_back('3'); REQUIRE(s.size() == 4);  REQUIRE(std::memcmp(s.data(), "0123", 5) == 0);
     s.push_back('4'); REQUIRE(s.size() == 5);  REQUIRE(std::memcmp(s.data(), "01234", 6) == 0);
     s.push_back('5'); REQUIRE(s.size() == 6);  REQUIRE(std::memcmp(s.data(), "012345", 7) == 0);
     s.push_back('6'); REQUIRE(s.size() == 7);  REQUIRE(std::memcmp(s.data(), "0123456", 8) == 0);
     s.push_back('7'); REQUIRE(s.size() == 8);  REQUIRE(std::memcmp(s.data(), "01234567", 9) == 0);
     s.push_back('8'); REQUIRE(s.size() == 9);  REQUIRE(std::memcmp(s.data(), "012345678", 10) == 0);
     s.push_back('9'); REQUIRE(s.size() == 10); REQUIRE(std::memcmp(s.data(), "0123456789", 11) == 0);
}

TEST_CASE("compact/string.pop_back", "[compact]")
{
     compact::string s = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
     REQUIRE(s.size() == 10); REQUIRE(std::memcmp(s.data(), "0123456789", 11) == 0);

     s.pop_back(); REQUIRE(s.size() == 9);  REQUIRE(std::memcmp(s.data(), "012345678", 10) == 0);
     s.pop_back(); REQUIRE(s.size() == 8);  REQUIRE(std::memcmp(s.data(), "01234567", 9) == 0);
     s.pop_back(); REQUIRE(s.size() == 7);  REQUIRE(std::memcmp(s.data(), "0123456", 8) == 0);
     s.pop_back(); REQUIRE(s.size() == 6);  REQUIRE(std::memcmp(s.data(), "012345", 7) == 0);
     s.pop_back(); REQUIRE(s.size() == 5);  REQUIRE(std::memcmp(s.data(), "01234", 6) == 0);
     s.pop_back(); REQUIRE(s.size() == 4);  REQUIRE(std::memcmp(s.data(), "0123", 5) == 0);
     s.pop_back(); REQUIRE(s.size() == 3);  REQUIRE(std::memcmp(s.data(), "012", 4) == 0);
     s.pop_back(); REQUIRE(s.size() == 2);  REQUIRE(std::memcmp(s.data(), "01", 3) == 0);
     s.pop_back(); REQUIRE(s.size() == 1);  REQUIRE(std::memcmp(s.data(), "0", 2) == 0);
     s.pop_back(); REQUIRE(s.empty());
}


TEST_CASE("compact/string.concat", "[compact]")
{
    compact::string s = "123";
    size_t n = s.size();

    SECTION("char + compact::string")
    {
        compact::string result = '0' + s;
        REQUIRE(result.size() == n + 1);
        REQUIRE(std::memcmp(result.data(), "0123", sizeof("0123")) == 0);
    }
    SECTION("compact::string + char")
    {
        compact::string result = s + '4';
        REQUIRE(result.size() == n + 1);
        REQUIRE(std::memcmp(result.data(), "1234", sizeof("1234")) == 0);
    }


    SECTION("C-string + compact::string")
    {
        const char* cstr = "haha";
        compact::string result = cstr + s;
        REQUIRE(result.size() == std::strlen(cstr) + n);
        REQUIRE(std::memcmp(result.data(), "haha123", sizeof("haha123")) == 0);
    }
    SECTION("compact::string + C-string")
    {
        const char* cstr = "haha";
        compact::string result = s + cstr;
        REQUIRE(result.size() == std::strlen(cstr) + n);
        REQUIRE(std::memcmp(result.data(), "123haha", sizeof("123haha")) == 0);

    }


    SECTION("std::string + compact::string")
    {
        std::string str("haha");
        compact::string result = str + s;
        REQUIRE(result.size() == str.size() + n);
        REQUIRE(std::memcmp(result.data(), "haha123", sizeof("haha123")) == 0);

    }
    SECTION("compact::string + std::string")
    {
        std::string str("haha");
        compact::string result = s + str;
        REQUIRE(result.size() == str.size() + n);
        REQUIRE(std::memcmp(result.data(), "123haha", sizeof("123haha")) == 0);
    }

    SECTION("std::string + compact::string : exceptions")
    {
        std::string str(1 << 17, 'a');
        compact::string s;
        compact::string result;
        REQUIRE_THROWS(result = str + s);
        REQUIRE_THROWS(result = str.c_str() + s);
    }
    SECTION("compact::string + std::string : exceptions")
    {
        std::string str(1 << 17, 'a');
        compact::string s;
        compact::string result;
        REQUIRE_THROWS(result = s + str);
    }


    SECTION("compact::string + compact::string")
    {
        compact::string str("haha");
        compact::string result = s + str;
        REQUIRE(result.size() == str.size() + n);
        REQUIRE(std::memcmp(result.data(), "123haha", sizeof("123haha")) == 0);
    }
    SECTION("compact::string + compact::string : exceptions")
    {
        compact::string s1((1 << 15) + 256, 'a');
        compact::string s2((1 << 15) + 256, 'b');
        compact::string result;
        REQUIRE_THROWS(result = s1 + s2);
    }


    SECTION("compact::tagged_string + compact::string")
    {
        compact::tagged_string<2> tstr("haha");
        tstr.tag(2);
        s += tstr;
        REQUIRE(s.size() == tstr.size() + n);
        REQUIRE(std::memcmp(s.data(), "123haha", sizeof("123haha")) == 0);
    }



}


TEST_CASE("compact/string.swap", "[compact]")
{
    std::vector<compact::string> strings = {
        "1-one",
        "2-two",
        "3-three",
        "4-four",
        "5-five",
        "6-six",
        "7-seven"
    };

    std::vector<compact::string> expected = {
        "7-seven",
        "6-six",
        "5-five",
        "4-four",
        "3-three",
        "2-two",
        "1-one"
    };

    std::swap(strings[0], strings[0]);
    std::reverse(strings.begin(), strings.end());
    REQUIRE(strings == expected);
}


TEST_CASE("compact/string.compare", "[compact]")
{
    compact::string s1 = "aaa";
    compact::string eq = "aaa";
    compact::string lt = "00";
    compact::string gt = "bbc";

    compact::tagged_string<2> s2x = "aaa";
    compact::tagged_string<2> eqx = "aaa";
    compact::tagged_string<2> ltx = "00";
    compact::tagged_string<2> gtx = "bbc";

    const char* ceq = "aaa";
    const char* clt = "00";
    const char* cgt = "bbc";

    std::string streq = "aaa";
    std::string strlt = "00";
    std::string strgt = "bbc";

    REQUIRE(s1 == eq);
    REQUIRE(s1 == eqx);
    REQUIRE(s1 == ceq);
    REQUIRE(s1 == streq);

    REQUIRE(eq == s1);
    REQUIRE(eqx == s1);
    REQUIRE(ceq == s1);
    REQUIRE(streq == s1);



    REQUIRE(s1 != lt);
    REQUIRE(s1 != ltx);
    REQUIRE(s1 != clt);
    REQUIRE(s1 != strlt);

    REQUIRE(lt != s1);
    REQUIRE(ltx != s1);
    REQUIRE(clt != s1);
    REQUIRE(strlt != s1);



    REQUIRE(s1 > lt);
    REQUIRE(s1 > ltx);
    REQUIRE(s1 > clt);
    REQUIRE(s1 > strlt);

    REQUIRE(gt > s1);
    REQUIRE(gtx > s1);
    REQUIRE(cgt > s1);
    REQUIRE(strgt > s1);



    REQUIRE(s1 < gt);
    REQUIRE(s1 < gtx);
    REQUIRE(s1 < cgt);
    REQUIRE(s1 < strgt);

    REQUIRE(lt < s1);
    REQUIRE(ltx < s1);
    REQUIRE(clt < s1);
    REQUIRE(strlt < s1);
}

TEST_CASE("compact/string.streaming", "[compact]")
{
    compact::string s1 = "aaa";
    std::ostringstream oss;
    oss << s1;
    REQUIRE(oss.str() == s1);
    REQUIRE(oss.str() == "aaa");
}

