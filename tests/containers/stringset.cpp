#include <catch.hpp>

#include <vector>
#include <string>

#include <stlext/containers/stringset.hpp>


TEST_CASE("containers/stringset", "[containers]")
{
    std::vector<int> vals = { -1, 0, 34, 12, 11, -888, 5, 26 };

    stdx::stringset ss;
    for (uint64_t i = 0; i < vals.size(); i++) {
        ss.insert(std::to_string(vals[i]));
    }
    REQUIRE(ss.size() == vals.size());

    for (uint64_t i = 0; i < vals.size(); i++) {
        std::string s = std::to_string(vals[i]);
        REQUIRE(ss.find(s) != ss.end());
    }

    REQUIRE(ss.erase("0") == 1);
    REQUIRE(ss.erase("-111") == 0);
    REQUIRE(ss.size() == vals.size() - 1);
    REQUIRE(ss.find("0") == ss.end());
    REQUIRE(ss.count("0") == 0);
    REQUIRE(ss.count("11", 2) == 1);
}

