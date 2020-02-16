#include <cstring>
#include <vector>
#include <sstream>

#include <catch.hpp>

#include <stlext/compact/string.hpp>

TEST_CASE("compact/wstring.creation", "[compact]")
{
    compact::wstring s;
    REQUIRE(s.size() == 0);
    REQUIRE(s.empty());

    compact::wstring s0 = { L'0', L'1', L'2', L'3' };
    REQUIRE(s0.size() == 4);
    REQUIRE(std::wmemcmp(s0.data(), L"0123", 5) == 0);

    std::wstring other = L"std";

    compact::wstring s1(other);
    REQUIRE(s1.size() == 3);
    REQUIRE(std::wmemcmp(s1.c_str(), other.c_str(), other.size() + 1) == 0);


    compact::tagged_wstring<2> tstr(L"432");
    REQUIRE(tstr.size() == 3);
    REQUIRE(std::wmemcmp(tstr.c_str(), L"432", 4) == 0);


    compact::wstring s2(tstr);
    REQUIRE(s2.size() == tstr.size());
    REQUIRE(std::wmemcmp(s2.c_str(), tstr.data(), tstr.size() + 1) == 0);

    compact::wstring s3 = L"abc";
    REQUIRE(s3.size() == 3);
    REQUIRE(std::wmemcmp(s3.data(), L"abc", 4) == 0);

    compact::wstring s4(L"aa-aa-aa", 5);
    REQUIRE(s4.size() == 5);
    REQUIRE(std::wmemcmp(s4.c_str(), L"aa-aa", 6) == 0);
}

TEST_CASE("compact/wstring.assign", "[compact]")
{
    std::wstring stdstr = L"abcde";
    compact::tagged_wstring<1> tstr = L"432";
    const wchar_t* cstr = L"0123";

    compact::wstring s;
    s = stdstr;
    REQUIRE(s.size() == stdstr.size());
    REQUIRE(std::wmemcmp(s.data(), stdstr.data(), stdstr.size() + 1) == 0);

    s = tstr;
    REQUIRE(s.size() == tstr.size());
    REQUIRE(std::wmemcmp(s.data(), tstr.data(), tstr.size() + 1) == 0);

    s = cstr;
    REQUIRE(s.size() == std::wcslen(cstr));
    REQUIRE(std::wmemcmp(s.data(), cstr, std::wcslen(cstr) + 1) == 0);

    s = s;
    REQUIRE(s.size() == std::wcslen(cstr));
    REQUIRE(std::wmemcmp(s.data(), cstr, std::wcslen(cstr) + 1) == 0);

    s.assign(cstr, 2);
    REQUIRE(s.size() == 2);
    REQUIRE(std::wmemcmp(s.data(), L"01", 3) == 0);
    REQUIRE(s.front() == L'0');
    REQUIRE(s.back() == L'1');

    s.assign(3, 'a');
    REQUIRE(s.size() == 3);
    REQUIRE(std::wmemcmp(s.data(), L"aaa", 4) == 0);

    s.assign({L'5', L'4', L'3', L'2', L'1'});
    REQUIRE(s.size() == 5);
    REQUIRE(std::wmemcmp(s.data(), L"54321", 6) == 0);

    s.assign(8, 'x');
    REQUIRE(s.size() == 8);
    REQUIRE(std::wmemcmp(s.data(), L"xxxxxxxx", 9) == 0);

    s.assign(8, 'a');
    REQUIRE(s.size() == 8);
    REQUIRE(std::wmemcmp(s.data(), L"aaaaaaaa", 9) == 0);

    s.assign(L"01234567");
    REQUIRE(s.size() == 8);
    REQUIRE(std::wmemcmp(s.data(), L"01234567", 9) == 0);

    s.assign(L"");
    REQUIRE(s.empty());

    s.assign(L"1");
    REQUIRE(s.front() == '1');

    s.assign(0, L'\0'); // clear
    REQUIRE(s.empty());

    REQUIRE_THROWS(s.assign(1 << 17, 'a'));
    REQUIRE_THROWS(s.assign(std::wstring(1 << 17, 'a')));
}

TEST_CASE("compact/wstring.resize", "[compact]")
{
    compact::wstring s(L"ha");
    REQUIRE(s.size() == 2);

    s += s;
    REQUIRE(s.size() == 4);
    REQUIRE(std::wmemcmp(s.data(), L"haha", 5) == 0);

    s += s;
    REQUIRE(s.size() == 8);
    REQUIRE(std::wmemcmp(s.data(), L"hahahaha", 9) == 0);


    s.erase(0, 4);
    REQUIRE(s.size() == 4);
    REQUIRE(std::wmemcmp(s.data(), L"haha", 5) == 0);

    s.insert(3, 0, L'\0'); // does nothing
    s.insert(3, L""); // does nothing
    s.erase(0, compact::wstring::size_type(0)); // does nothing

    s.insert(4, L"-haha");
    REQUIRE(s.size() == 9);
    REQUIRE(std::wmemcmp(s.data(), L"haha-haha", 10) == 0);

    std::wstring str(L"123");
    s.insert(0, str);
    REQUIRE(s.size() == 12);
    REQUIRE(std::wmemcmp(s.data(), L"123haha-haha", 13) == 0);

    s.erase(s.begin() + 3, s.begin() + 7);
    REQUIRE(s.size() == 8);
    REQUIRE(std::wmemcmp(s.data(), L"123-haha", 9) == 0);

    s.resize(12, 'x');
    REQUIRE(s.size() == 12);
    REQUIRE(std::wmemcmp(s.data(), L"123-hahaxxxx", 13) == 0);

    s.resize(3);
    REQUIRE(s.size() == 3);
    REQUIRE(std::wmemcmp(s.data(), L"123", 4) == 0);

    s.resize(5);
    REQUIRE(s.size() == 5);
    REQUIRE(std::wmemcmp(s.data(), L"123\0\0", 6) == 0);

    s.resize(2, 'a');
    REQUIRE(s.size() == 2);
    REQUIRE(std::wmemcmp(s.data(), L"12", 3) == 0);

    s.clear();
    REQUIRE(s.size() == 0);
    REQUIRE(s.empty());

    REQUIRE_THROWS(s.insert(0, std::wstring(1ULL << 17, L'a')));
    REQUIRE_THROWS(s.resize(1<<17));
}

TEST_CASE("compact/wstring.front", "[compact]")
{
    compact::wstring s1 = L"h";
    REQUIRE(s1.front() == L'h');
    s1 = { L'h', L'a' };
    REQUIRE(s1.front() == L'h');
}

TEST_CASE("compact/wstring.back", "[compact]")
{
    compact::wstring s1 = L"h";
    REQUIRE(s1.back() == L'h');
    s1 = { L'h', L'a' };
    REQUIRE(s1.back() == L'a');
}

TEST_CASE("compact/wstring.push_back", "[compact]")
{
     compact::wstring s;
     s.push_back(L'0'); REQUIRE(s.size() == 1);  REQUIRE(std::wmemcmp(s.data(), L"0", 2) == 0);
     s.push_back(L'1'); REQUIRE(s.size() == 2);  REQUIRE(std::wmemcmp(s.data(), L"01", 3) == 0);
     s.push_back(L'2'); REQUIRE(s.size() == 3);  REQUIRE(std::wmemcmp(s.data(), L"012", 4) == 0);
     s.push_back(L'3'); REQUIRE(s.size() == 4);  REQUIRE(std::wmemcmp(s.data(), L"0123", 5) == 0);
     s.push_back(L'4'); REQUIRE(s.size() == 5);  REQUIRE(std::wmemcmp(s.data(), L"01234", 6) == 0);
     s.push_back(L'5'); REQUIRE(s.size() == 6);  REQUIRE(std::wmemcmp(s.data(), L"012345", 7) == 0);
     s.push_back(L'6'); REQUIRE(s.size() == 7);  REQUIRE(std::wmemcmp(s.data(), L"0123456", 8) == 0);
     s.push_back(L'7'); REQUIRE(s.size() == 8);  REQUIRE(std::wmemcmp(s.data(), L"01234567", 9) == 0);
     s.push_back(L'8'); REQUIRE(s.size() == 9);  REQUIRE(std::wmemcmp(s.data(), L"012345678", 10) == 0);
     s.push_back(L'9'); REQUIRE(s.size() == 10); REQUIRE(std::wmemcmp(s.data(), L"0123456789", 11) == 0);
}

TEST_CASE("compact/wstring.pop_back", "[compact]")
{
     compact::wstring s = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
     REQUIRE(s.size() == 10); REQUIRE(std::wmemcmp(s.data(), L"0123456789", 11) == 0);

     s.pop_back(); REQUIRE(s.size() == 9);  REQUIRE(std::wmemcmp(s.data(), L"012345678", 10) == 0);
     s.pop_back(); REQUIRE(s.size() == 8);  REQUIRE(std::wmemcmp(s.data(), L"01234567", 9) == 0);
     s.pop_back(); REQUIRE(s.size() == 7);  REQUIRE(std::wmemcmp(s.data(), L"0123456", 8) == 0);
     s.pop_back(); REQUIRE(s.size() == 6);  REQUIRE(std::wmemcmp(s.data(), L"012345", 7) == 0);
     s.pop_back(); REQUIRE(s.size() == 5);  REQUIRE(std::wmemcmp(s.data(), L"01234", 6) == 0);
     s.pop_back(); REQUIRE(s.size() == 4);  REQUIRE(std::wmemcmp(s.data(), L"0123", 5) == 0);
     s.pop_back(); REQUIRE(s.size() == 3);  REQUIRE(std::wmemcmp(s.data(), L"012", 4) == 0);
     s.pop_back(); REQUIRE(s.size() == 2);  REQUIRE(std::wmemcmp(s.data(), L"01", 3) == 0);
     s.pop_back(); REQUIRE(s.size() == 1);  REQUIRE(std::wmemcmp(s.data(), L"0", 2) == 0);
     s.pop_back(); REQUIRE(s.empty());
}


TEST_CASE("compact/wstring.concat", "[compact]")
{
    compact::wstring s = L"123";
    size_t n = s.size();

    SECTION("wchar_t + compact::wstring")
    {
        compact::wstring result = L'0' + s;
        REQUIRE(result.size() == n + 1);
        REQUIRE(std::memcmp(result.data(), L"0123", sizeof(L"0123")) == 0);
    }
    SECTION("compact::wstring + wchar_t")
    {
        compact::wstring result = s + L'4';
        REQUIRE(result.size() == n + 1);
        REQUIRE(std::memcmp(result.data(), L"1234", sizeof(L"1234")) == 0);
    }


    SECTION("C-wstring + compact::wstring")
    {
        const wchar_t* cstr = L"haha";
        compact::wstring result = cstr + s;
        REQUIRE(result.size() == std::wcslen(cstr) + n);
        REQUIRE(std::memcmp(result.data(), L"haha123", sizeof(L"haha123")) == 0);
    }
    SECTION("compact::wstring + C-wstring")
    {
        const wchar_t* cstr = L"haha";
        compact::wstring result = s + cstr;
        REQUIRE(result.size() == std::wcslen(cstr) + n);
        REQUIRE(std::memcmp(result.data(), L"123haha", sizeof(L"123haha")) == 0);

    }


    SECTION("std::wstring + compact::wstring")
    {
        std::wstring str(L"haha");
        compact::wstring result = str + s;
        REQUIRE(result.size() == str.size() + n);
        REQUIRE(std::memcmp(result.data(), L"haha123", sizeof(L"haha123")) == 0);

    }
    SECTION("compact::wstring + std::wstring")
    {
        std::wstring str(L"haha");
        compact::wstring result = s + str;
        REQUIRE(result.size() == str.size() + n);
        REQUIRE(std::memcmp(result.data(), L"123haha", sizeof(L"123haha")) == 0);
    }

    SECTION("std::wstring + compact::wstring : exceptions")
    {
        std::wstring str(1 << 17, L'a');
        compact::wstring s;
        compact::wstring result;
        REQUIRE_THROWS(result = str + s);
        REQUIRE_THROWS(result = str.c_str() + s);
    }
    SECTION("compact::wstring + std::wstring : exceptions")
    {
        std::wstring str(1 << 17, L'a');
        compact::wstring s;
        compact::wstring result;
        REQUIRE_THROWS(result = s + str);
    }


    SECTION("compact::wstring + compact::wstring")
    {
        compact::wstring str(L"haha");
        compact::wstring result = s + str;
        REQUIRE(result.size() == str.size() + n);
        REQUIRE(std::memcmp(result.data(), L"123haha", sizeof(L"123haha")) == 0);
    }
    SECTION("compact::wstring + compact::wstring : exceptions")
    {
        compact::wstring s1((1 << 15) + 256, 'a');
        compact::wstring s2((1 << 15) + 256, 'b');
        compact::wstring result;
        REQUIRE_THROWS(result = s1 + s2);
    }


    SECTION("compact::tagged_wstring + compact::wstring")
    {
        compact::tagged_wstring<2> tstr(L"haha");
        tstr.tag(2);
        s += tstr;
        REQUIRE(s.size() == tstr.size() + n);
        REQUIRE(std::memcmp(s.data(), L"123haha", sizeof(L"123haha")) == 0);
    }



}


TEST_CASE("compact/wstring.swap", "[compact]")
{
    std::vector<compact::wstring> strings = {
        L"1-one",
        L"2-two",
        L"3-three",
        L"4-four",
        L"5-five",
        L"6-six",
        L"7-seven"
    };

    std::vector<compact::wstring> expected = {
        L"7-seven",
        L"6-six",
        L"5-five",
        L"4-four",
        L"3-three",
        L"2-two",
        L"1-one"
    };

    std::swap(strings[0], strings[0]);
    std::reverse(strings.begin(), strings.end());
    REQUIRE(strings == expected);
}


TEST_CASE("compact/wstring.compare", "[compact]")
{
    compact::wstring s1 = L"aaa";
    compact::wstring eq = L"aaa";
    compact::wstring lt = L"00";
    compact::wstring gt = L"bbc";

    compact::tagged_wstring<2> s2x = L"aaa";
    compact::tagged_wstring<2> eqx = L"aaa";
    compact::tagged_wstring<2> ltx = L"00";
    compact::tagged_wstring<2> gtx = L"bbc";

    const wchar_t* ceq = L"aaa";
    const wchar_t* clt = L"00";
    const wchar_t* cgt = L"bbc";

    std::wstring streq = L"aaa";
    std::wstring strlt = L"00";
    std::wstring strgt = L"bbc";

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

TEST_CASE("compact/wstring.streaming", "[compact]")
{
    compact::wstring s1 = L"aaa";
    std::wostringstream oss;
    oss << s1;
    REQUIRE(oss.str() == s1);
    REQUIRE(oss.str() == L"aaa");
}

