#include <catch.hpp>

#include <stlext/range/interval.hpp>


TEST_CASE("range/interval", "[range.value_range]")
{
    using stdx::interval;

    interval<int> r;
    REQUIRE(r.valid() == false);
    REQUIRE(r.empty() == true);
    REQUIRE(r.null() == true);
    REQUIRE(r.size() == 0);
    REQUIRE(((r.lower() == 0) && (r.upper() == 0)));

    r.assign(19, 3);
    REQUIRE(((r.lower() == 19) && (r.upper() == 3)));
    REQUIRE((!r) == true);
    REQUIRE((bool)r == false);
    REQUIRE(r.valid() == false);
    REQUIRE(r.empty() == false);
    REQUIRE(r.single() == false);
    REQUIRE(r.null() == false);

    r.normalize();
    REQUIRE(((r.lower() == 3) && (r.upper() == 19)));
    REQUIRE(r.valid() == true);
    REQUIRE(r.empty() == false);
    REQUIRE(r.null() == false);
    REQUIRE(r.single() == false);
    REQUIRE(r.size() == ((19-3)+1));


    REQUIRE(r.contains(3) == true);
    REQUIRE(r.contains(19) == true);
    REQUIRE(r.contains(10) == true);
    REQUIRE(r.contains(2) == false);
    REQUIRE(r.contains(20) == false);
    REQUIRE(r.contains(-1) == false);

    REQUIRE(r.contains({3, 3}) == true);
    REQUIRE(r.contains({3, 6}) == true);
    REQUIRE(r.contains({3, 19}) == true);
    REQUIRE(r.contains({3, 20}) == false);
    REQUIRE(r.contains({2, 19}) == false);
    REQUIRE(r.contains({2, 20}) == false);

    REQUIRE(r.touches({-1, 3}) == true);
    REQUIRE(r.touches({19, 21}) == true);
    REQUIRE(r.touches({20, 23}) == false);

    REQUIRE(r.overlaps({-1, 6}) == true);
    REQUIRE(r.overlaps({18, 23}) == true);
    REQUIRE(r.overlaps({3, 4}) == true);
    REQUIRE(r.overlaps({10, 19}) == true);
    REQUIRE(r.overlaps({-10, 2}) == false);
    REQUIRE(r.overlaps({20, 29}) == false);

    /*
    -1  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
                 ^-----------------------------------------------^
    ^---------------------^        ^---------------------^
                 ^--^
                                                               ^-------------^
    */
    REQUIRE((r & interval<int>{-1, 6}) == interval<int>(3, 6));
    REQUIRE(r.intersect({18, 23}) == interval<int>(18, 19));
    REQUIRE(r.intersect({3, 4})   == interval<int>(3, 4));
    REQUIRE(r.intersect({9, 16})  == interval<int>(9, 16));

    REQUIRE((r | interval<int>{-1, 6})  == interval<int>(-1, 19));
    REQUIRE(r.unite({18, 23}) == interval<int>(3, 23));
    REQUIRE(r.unite({3, 4})   == interval<int>(3, 19));
    REQUIRE(r.unite({9, 16})  == interval<int>(3, 19));


    REQUIRE( (r + interval<int>(-1, 1)) == interval<int>(2, 20));
    REQUIRE( (r - interval<int>(-1, 1)) == interval<int>(2, 20));

    REQUIRE( (r * interval<int>(-1, 1)) == interval<int>(-19, 19));
    REQUIRE( (r / interval<int>(-1, 1)) == interval<int>(-19, 19));

    REQUIRE_THROWS((r / interval<int>(0, 1)));
    REQUIRE_THROWS((r / interval<int>(-1, 0)));
}

