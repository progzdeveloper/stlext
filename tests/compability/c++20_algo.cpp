#include <catch.hpp>
#include <stlext/compability/cxx20/span.hpp>


TEST_CASE("span", "[compability.cpp20]")
{
	using namespace stdx;
	std::vector<int> x(10, 0);
    for (size_t i = 0; i < x.size(); i++)
		x[i] = i;

	span<int> range(x);
	REQUIRE(range.at(1) == 1);
	REQUIRE(range(0) == 0);
	REQUIRE(range[2] == 2);

	auto sub1 = range.subspan(1, 5);
	REQUIRE(std::equal(sub1.begin(), sub1.end(), x.begin() + 1, x.begin() + 6));

	auto sub2 = range.subspan<3, 3>();
	REQUIRE(std::equal(sub2.begin(), sub2.end(), x.begin() + 3, x.begin() + 6));

	REQUIRE_FALSE(sub1.empty());
	REQUIRE(sub1.front() == 1); 
	REQUIRE(sub1.back() == 5);

	REQUIRE(sub2.front() == 3);
	REQUIRE(sub2.back() == 5);
}
