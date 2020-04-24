#include <catch.hpp>

#include <sstream>
#include <unordered_set>

#include <tuple>

#include <stlext/tuple/tuple_algo.hpp>
#include <stlext/tuple/tuple_utils.hpp>


TEST_CASE("tuple/apply", "[tuple]")
{
    std::string s;
    int a = 0;
    double d = 0.0;
    auto test_fn = [&s, &a, &d](const std::string& x, int y, double z) {
        s = x;
        a = y;
        d = z;
    };

    stdx::apply(test_fn, std::make_tuple("hello", 5, 0.1));

    REQUIRE(s == "hello");
    REQUIRE(a == 5);
    REQUIRE(d == 0.1);
}

TEST_CASE("tuple/foreach", "[tuple]")
{
    const std::string s = "hello 5 0.3 ";
    std::ostringstream oss;
    auto act = [&oss](auto x) {
        oss << x << ' ';
    };

    stdx::foreach_arg(act, "hello", 5, 0.3);
    REQUIRE(s == oss.str());

    oss.str("");
    stdx::foreach_element(act, std::make_tuple("hello", 5, 0.3));
    REQUIRE(s == oss.str());
}


TEST_CASE("tuple/transform", "[tuple]")
{
    using std::get;

    auto src = std::make_tuple(1, 0.4);
    auto res = std::make_tuple(2, 0.8);
    decltype(src) dst;
    stdx::tuple_transform(src, dst, [](auto x) { return (x *= 2);});
    REQUIRE(get<0>(dst) == get<0>(res));
    REQUIRE(get<1>(dst) == get<1>(res));

    auto src1 = std::make_tuple(std::string("hello"), 1, 0.2);
    auto src2 = std::make_tuple(std::string(" stlext"), 4, 0.2);
    auto expected = std::make_tuple(std::string("hello stlext"), 5, 0.4);
    decltype(expected) result;

    stdx::tuple_transform(src1, src2, result, std::plus<>{});
    REQUIRE(get<0>(result) == get<0>(expected));
    REQUIRE(get<1>(result) == get<1>(expected));
    REQUIRE(get<2>(result) == get<2>(expected));

}



TEST_CASE("tuple/utils", "[tuple]")
{
    using namespace std;

    typedef tuple<int, string, float> record_t;
    vector<record_t> records = {
        record_t{ 121, "first",  0.5 },
        record_t{ 123, "second", 0.3 },
        record_t{ 127, "third",  0.1 }
    };

    vector<int>    ids = { 121, 123, 127 };
    vector<float>  ratings = { 0.5, 0.3, 0.1 };
    vector<string> names = { "first", "second", "third"};
    vector<int>    ires;
    vector<float>  fres;
    vector<string> sres;

    std::transform(records.begin(), records.end(), back_inserter(ires), stdx::select_ith<0, record_t>{});
    REQUIRE(ires == ids);

    std::transform(records.begin(), records.end(), back_inserter(fres), stdx::select_ith<2>{});
    REQUIRE(fres == ratings);

    std::transform(records.begin(), records.end(), back_inserter(sres), stdx::select_ith<1>{});
    REQUIRE(sres == names);
}

TEST_CASE("tuple/hash", "[tuple]")
{
    using namespace std;
    typedef tuple<int, string, float> record_t;
    typedef unordered_set< record_t, stdx::tuple_hash<record_t> > record_set;
    record_set records;
    records.emplace(121, "first",  0.5);
    records.emplace(123, "second", 0.3);
    records.emplace(127, "third",  0.1);

    REQUIRE_FALSE(records.find(std::forward_as_tuple(123, "second", 0.3)) == records.end());
}

TEST_CASE("tuple/compare", "[tuple]")
{
    using namespace std;
    typedef tuple<int, string, float> record_t;
    vector<record_t> records = {
        record_t{ 121, "first",  0.5 },
        record_t{ 123, "second", 0.3 },
        record_t{ 127, "third",  0.1 }
    };

    stdx::nth_comparer<0> comp0;
    stdx::nth_comparer<1> comp1;
    stdx::nth_comparer<2> comp2;

    REQUIRE_FALSE(comp0(records[1], records[0]));
    REQUIRE_FALSE(comp1(records[1], records[0]));
    REQUIRE_FALSE(comp2(records[0], records[1]));

    vector< pair<int, string> > pairs = {
        { 0, "one" },
        { 1, "two" }
    };
    REQUIRE_FALSE(comp0(pairs[1], pairs[0]));
    REQUIRE_FALSE(comp1(pairs[1], pairs[0]));

    vector< array<int, 3> > arrs = {
        { 0, 1, 2 },
        { 3, 4, 5 }
    };

    REQUIRE_FALSE(comp0(arrs[1], arrs[0]));
    REQUIRE_FALSE(comp1(arrs[1], arrs[0]));
    REQUIRE_FALSE(comp2(arrs[1], arrs[0]));
}

