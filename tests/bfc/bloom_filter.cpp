#include <catch.hpp>
#include <string>

#include <stlext/bfc/basic_bloom_filter.hpp>
#include <stlext/bfc/counting_bloom_filter.hpp>


TEST_CASE("basic_bloom_filter", "[bfc/basic_bloom_filter]")
{
    std::string strs[] = {
        "0000",
        "0001",
        "a",
        "c"
    };

    stdx::basic_bloom_filter<std::string> filter(0.8, 16);
    filter.insert(std::begin(strs), std::end(strs));


    REQUIRE(filter.count("a") == 1);
    REQUIRE(filter.all_of(std::begin(strs), std::end(strs)) == std::end(strs));

    std::string test[] = {"b", "c"};
    REQUIRE(filter.any_of(std::begin(test), std::end(test)) == &test[1]);
}

TEST_CASE("basic_bloom_filter/union", "[bfc/basic_bloom_filter]")
{
    std::string strs[] = {
        "0000",
        "0001",
        "a",
        "c"
    };
    stdx::basic_bloom_filter<std::string> f1(0.8, 10);
    f1.insert(std::begin(strs), std::end(strs));

    stdx::basic_bloom_filter<std::string> f2(0.8, 10);
    f2.insert({"foo", "bar", "baz"});

    stdx::basic_bloom_filter<std::string> res = f1 | f2;
    REQUIRE(res.all_of(std::begin(strs), std::end(strs)) == std::end(strs));
    REQUIRE(res.count("foo") == 1);
    REQUIRE(res.count("bar") == 1);
    REQUIRE(res.count("baz") == 1);
    REQUIRE(res.count("c") == 0);
    REQUIRE(res.count("0000") == 0);
}


TEST_CASE("basic_bloom_filter/intersect", "[bfc/basic_bloom_filter]")
{
    std::string strs[] = {
        "foo",
        "bar",
        "a",
        "c"
    };
    stdx::basic_bloom_filter<std::string> f1(0.8, 10);
    f1.insert(std::begin(strs), std::end(strs));

    stdx::basic_bloom_filter<std::string> f2(0.8, 10);
    f2.insert({"foo", "bar", "baz"});

    stdx::basic_bloom_filter<std::string> res = f1 & f2;
    REQUIRE(res.count("foo") == 1);
    REQUIRE(res.count("bar") == 1);
    REQUIRE(res.count("baz") == 0);
}


TEST_CASE("counting_bloom_filter", "[bfc/counting_bloom_filter]")
{
    std::string strs[] = {
        "0000",
        "0001",
        "a",
        "c"
    };
    stdx::counting_bloom_filter<std::string> filter(0.8, 10);
    filter.insert({"foo", "bar", "baz"});
    filter.insert({"foo", "bar", "baz"});

    REQUIRE(filter.count("foo") == 2);
    REQUIRE(filter.count("bar") == 2);
    REQUIRE(filter.count("baz") == 2);

    filter.insert(std::begin(strs), std::end(strs));

    filter.erase("foo");
    filter.erase("foo");
    filter.erase("foo");

    REQUIRE(filter.count("foo") == 0);
}
