#include <sstream>
#include <vector>
#include <list>
#include <forward_list>
#include <algorithm>
#include <iterator>
#include <catch.hpp>

#include <stlext/compact/vector.hpp>

TEST_CASE("compact/vector.creation", "[compact]")
{
    compact::vector<int> v1;
    REQUIRE(v1.size() == 0);
    REQUIRE(v1.empty());

    compact::vector<int> v2(3);
    REQUIRE(v2.size() == 3);
    REQUIRE(!v2.empty());
    REQUIRE(std::all_of(v2.begin(), v2.end(), [](int x) { return (x == 0); }));


    compact::vector<int> v3(7, -1);
    REQUIRE(v3.size() == 7);
    REQUIRE(!v3.empty());
    REQUIRE(std::all_of(v3.begin(), v3.end(), [](int x) { return (x == -1); }));

    compact::vector<int> v4 = { 0, 1, 2, 3, 4 };
    std::vector<int> sv = { 0, 1, 2, 3, 4 };
    REQUIRE(v4.size() == 5);
    REQUIRE(std::equal(v4.begin(), v4.end(), sv.begin(), sv.end()));

    compact::vector<int> v5 = sv;
    REQUIRE(v5.size() == 5);
    REQUIRE(std::equal(v5.begin(), v5.end(), sv.begin(), sv.end()));

    int desired[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
    std::stringstream buf;
    std::copy(std::begin(desired), std::end(desired), std::ostream_iterator<int>(buf, " "));
    compact::vector<int> v6(std::istream_iterator<int>(buf),
                            std::istream_iterator<int>{});
    REQUIRE(v6.size() == size_t(10));
    REQUIRE(std::equal(std::begin(desired), std::end(desired), v6.begin(), v6.end()));
    
    std::list<int> dllst = { 0, 1, 2, 3 };
    compact::vector<int> v7(dllst.begin(), dllst.end());
    REQUIRE(std::equal(dllst.begin(), dllst.end(), v7.begin(), v7.end()));
    
    std::forward_list<int> fwdlst = { 4, 3, 2, 1 };
    compact::vector<int> v8(fwdlst.begin(), fwdlst.end());
    REQUIRE(std::equal(fwdlst.begin(), fwdlst.end(), v8.begin(), v8.end()));


    v5.assign(0, 0); // clear
    REQUIRE(v5.empty());

    v5.assign({1, 2});
    REQUIRE(v5.size() == 2);

    v5.assign(std::vector<int>()); // clear
    REQUIRE(v5.empty());
}

TEST_CASE("compact/vector.resize", "[compact]")
{
    typedef compact::vector<int>::iterator iterator;
    compact::vector<int> v1 = { 0, 1, 2, 3, 4 };
    v1.resize(7, -1); // 0, 1, 2, 3, 4, -1, -1
    REQUIRE(v1.size() == 7);
    REQUIRE(std::all_of(v1.begin() + 5, v1.end(), [](int x) { return (x == -1); }));

    iterator it;
    it = v1.erase(v1.begin(), v1.begin() + 2); // 2, 3, 4, -1, -1
    REQUIRE(v1.size() == 5);
    REQUIRE(*it == 2);

    it = v1.insert(v1.begin(), -100); // -100, 2, 3, 4, -1, -1
    REQUIRE(v1.size() == 6);
    REQUIRE(*it == -100);

    REQUIRE(v1.insert(v1.begin(), 0, 0) == v1.begin()); // does nothing
    REQUIRE(v1.erase(v1.begin(), v1.begin()) == v1.begin()); // does nothing
    REQUIRE(v1.insert(v1.begin(), v1.begin(), v1.begin()) == v1.begin()); // does nothing

    it = v1.insert(v1.begin(), 3, 0); // 0, 0, 0, -100, 2, 3, 4, -1, -1
    REQUIRE(v1.size() == 9);
    REQUIRE(std::all_of(it, it + 3, [](int x) { return (x == 0); }));

    it = v1.erase(v1.end()-1); // 0, 0, 0, -100, 2, 3, 4, -1
    REQUIRE(v1.size() == 8);
    REQUIRE(it == v1.end());

    v1.resize(5); // 0, 0, 0, -100, 2
    REQUIRE(v1.size() == 5);
    REQUIRE(v1.front() == 0);
    REQUIRE(v1.back() == 2);

    v1.resize(2, 0); // 0, 0
    REQUIRE(v1.size() == 2);
    REQUIRE(v1.front() == 0);
    REQUIRE(v1.back() == 0);

    v1 = { 1, 2 }; // 1, 2
    REQUIRE(v1.size() == 2);
    REQUIRE(v1.front() == 1);
    REQUIRE(v1.back() == 2);

    v1.assign(2, 0); // 0, 0
    REQUIRE(v1.size() == 2);
    REQUIRE(v1.front() == 0);
    REQUIRE(v1.back() == 0);

    v1 = compact::vector<int>(); // -
    REQUIRE(v1.empty());

    v1.assign(2, 0); //  0, 0
    REQUIRE(v1.size() == 2);
    REQUIRE(v1.front() == 0);
    REQUIRE(v1.back() == 0);


    v1.insert(v1.end(), { 1, 2, 3 }); // 0, 0, 1, 2, 3
    REQUIRE(v1.size() == 5);
    REQUIRE(v1[0] == 0);
    REQUIRE(v1[1] == 0);
    REQUIRE(v1[2] == 1);
    REQUIRE(v1[3] == 2);
    REQUIRE(v1[4] == 3);
    REQUIRE_THROWS(v1.at(5));

    int arr[] = { 1, 2, 3 };
    v1.insert(v1.end(), arr, arr + 3); // 0, 0, 1, 2, 3, 1, 2, 3
    REQUIRE(v1[5] == 1);
    REQUIRE(v1[6] == 2);
    REQUIRE(v1[7] == 3);

    v1.insert(v1.end(), v1.begin(), v1.begin() + 2); // 0, 0, 1, 2, 3, 1, 2, 3, 0, 0
    REQUIRE(v1.at(8) == 0);
    REQUIRE(v1.at(9) == 0);
}


TEST_CASE("compact/vector.push_back", "[compact]")
{
    int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    compact::vector<int> v1;

    v1.push_back(0); REQUIRE(v1.size() == 1);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(1); REQUIRE(v1.size() == 2);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(2); REQUIRE(v1.size() == 3);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(3); REQUIRE(v1.size() == 4);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(4); REQUIRE(v1.size() == 5);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(5); REQUIRE(v1.size() == 6);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(6); REQUIRE(v1.size() == 7);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(7); REQUIRE(v1.size() == 8);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(8); REQUIRE(v1.size() == 9);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.push_back(9); REQUIRE(v1.size() == 10); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
}

TEST_CASE("compact/vector.emplace_back", "[compact]")
{
    int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    compact::vector<int> v1;

    v1.emplace_back(0); REQUIRE(v1.size() == 1);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(1); REQUIRE(v1.size() == 2);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(2); REQUIRE(v1.size() == 3);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(3); REQUIRE(v1.size() == 4);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(4); REQUIRE(v1.size() == 5);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(5); REQUIRE(v1.size() == 6);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(6); REQUIRE(v1.size() == 7);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(7); REQUIRE(v1.size() == 8);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(8); REQUIRE(v1.size() == 9);  REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.emplace_back(9); REQUIRE(v1.size() == 10); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
}


TEST_CASE("compact/vector.pop_back", "[compact]")
{
    int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    compact::vector<int> v1;
    v1.resize(std::distance(std::begin(expected), std::end(expected)));
    std::copy(std::begin(expected), std::end(expected), v1.begin());

    v1.pop_back(); REQUIRE(v1.size() == 9); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 8); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 7); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 6); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 5); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 4); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 3); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 2); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 1); REQUIRE(std::equal(v1.begin(), v1.end(), std::begin(expected), expected + v1.size()));
    v1.pop_back(); REQUIRE(v1.size() == 0); REQUIRE(v1.empty());
}


inline void emplace_overflow(compact::vector<int>& v) {
    for (;;) {
        v.emplace_back(1);
    }
}


TEST_CASE("compact/vector.overflow", "[compact]")
{
    compact::vector<int> v1;
    std::vector<int> d(1ULL << 17, -127);
    REQUIRE_THROWS(v1.insert(v1.begin(), d.data(), d.data() + d.size()));
    REQUIRE_THROWS(v1.resize(1ULL << 17));
    REQUIRE_THROWS(emplace_overflow(v1));
}

TEST_CASE("compact/vector.swap", "[compact]")
{
    std::vector< compact::vector<int> > expected = {
        {  0,  1,  2 },
        { -1, -2, -3 },
        {  1,  0, -1 },
        {  1,  1,  1 },
        { -1, -1, -1 }
    };


    std::vector< compact::vector<int> > actual = expected;
    std::reverse(actual.begin(), actual.end());
    std::swap(actual[0], actual[0]);

    REQUIRE(std::equal(expected.rbegin(), expected.rend(), actual.begin(), actual.end()));

}

TEST_CASE("compact/vector.compare", "[compact]")
{
    compact::vector<int> v = { 0, 1, 2, 3, 4, 5, 6 };
    compact::vector<int> neq1 = { 0, 1, 2, 3, 4, 5, 7 };
    compact::vector<int> neq2;
    std::vector<int> stdneq1(neq1.begin(), neq1.end());
    std::vector<int> stdneq2;

    compact::tagged_vector<int, 2> tagneq1 = neq1;
    compact::tagged_vector<int, 2> tagneq2;

    compact::vector<int> eq = v;
    std::vector<int> stdeq(v.begin(), v.end());
    compact::tagged_vector<int, 2> tageq = v;

    REQUIRE(v == v);
    REQUIRE(v == stdeq);
    REQUIRE(v == tageq);
    REQUIRE(stdeq == v);
    REQUIRE(tageq == v);


    REQUIRE(v != neq1);
    REQUIRE(v != neq2);
    REQUIRE(v != stdneq1);
    REQUIRE(v != stdneq2);
    REQUIRE(v != tagneq1);
    REQUIRE(v != tagneq2);
    REQUIRE(stdneq1 != v);
    REQUIRE(stdneq2 != v);
    REQUIRE(tagneq1 != v);
    REQUIRE(tagneq2 != v);
}

