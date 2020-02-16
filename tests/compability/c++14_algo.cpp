#include <catch.hpp>

#include <random>
#include <bitset>
#include <string>

#include <stlext/compability/cxx14/equal.hpp>
#include <stlext/compability/cxx14/mismatch.hpp>

TEST_CASE("algorithms/equal", "[compability.cpp14]")
{
	std::vector<int> v1 = { 0, 1, 2 };
	std::vector<int> v2 = { -1, 0, 1, 2 };
	REQUIRE_FALSE(stdx::equal(v1.begin(), v1.end(), v2.begin(), v2.end()));
}



