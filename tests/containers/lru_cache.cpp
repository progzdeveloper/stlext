#include <catch.hpp>

#include <vector>
#include <string>
#include <algorithm>

#include <stlext/containers/cache.hpp>


TEST_CASE("lru_cacheset/insert", "[containers]")
{
    using stdx::lru_cacheset;
    using namespace std;

    vector<string> strs = {
        "one", "two", "three", "four", "five"
    };

    lru_cacheset<std::string> cache(16);

    REQUIRE(cache.available() == 16);
    REQUIRE(cache.available(2) == 16/2);
    REQUIRE(cache.max_cost() == 16);

    int current_cost = 0;
    for (auto it = strs.begin(); it != strs.end(); ++it) {
        cache.insert(*it, it->size());
        current_cost += it->size();
        current_cost = (std::min)(16, current_cost);
        REQUIRE(cache.cost() == current_cost);
    }

    REQUIRE(cache.find("one") == cache.end());
    for (auto it = strs.begin() + 1; it != strs.end(); ++it) {
        // check existance
        REQUIRE(cache.contains(*it));
    }

    REQUIRE(cache.full());
    REQUIRE(cache.available() == 0);

    REQUIRE(cache.front() == strs.back());
    REQUIRE(cache.back() == *(++strs.begin()));

    auto iit = cache.begin();
    for (auto it = strs.rbegin(); it != strs.rend()-1; ++it, ++iit) {
        // check ordering
        REQUIRE(*iit == *it);
    }

    cache.clear();
    REQUIRE(cache.size() == 0);
    REQUIRE(cache.empty());
    REQUIRE(cache.max_cost() == 16);
    REQUIRE(cache.cost() == 0);
}

TEST_CASE("lru_cacheset/insert_large", "[containers]")
{
    using stdx::lru_cacheset;
    using namespace std;

    vector<string> strs = {
        "one", "two", "three", "four", "five"
    };

    string large(16, '!');

    lru_cacheset<std::string> cache(16);
    for (auto it = strs.begin(); it != strs.end(); ++it) {
        cache.insert(*it, it->size());
    }

    for (auto it = strs.begin() + 1; it != strs.end(); ++it) {
        // check existance
        REQUIRE(cache.contains(*it));
    }

    for (size_t i = 0; i < 4; i++)
        cache.insert(large, large.size());

    REQUIRE(cache.size() == 1);
    REQUIRE(!cache.empty());
    REQUIRE(cache.contains(large));
    REQUIRE(cache.available() == 0);
    REQUIRE(cache.full());
    REQUIRE(cache.front() == large);

    for (auto it = strs.begin() + 1; it != strs.end(); ++it) {
        // check non-existance
        REQUIRE(!cache.contains(*it));
    }
}

TEST_CASE("lru_cacheset/reorder", "[containers]")
{
    using stdx::lru_cacheset;
    using namespace std;

    vector<string> strs = {
        "one", "two", "three", "four", "five"
    };

    lru_cacheset<std::string> cache(16);
    for (auto it = strs.begin(); it != strs.end(); ++it) {
        cache.insert(*it, it->size());
    }

    cache.touch("three");
    REQUIRE(cache.front() == "three");
    REQUIRE(cache.back() == "two");

    cache.touch("four");
    REQUIRE(cache.front() == "four");
    REQUIRE(cache.back() == "two");

    cache.touch("two");
    REQUIRE(cache.front() == "two");
    REQUIRE(cache.back() == "five");
}
