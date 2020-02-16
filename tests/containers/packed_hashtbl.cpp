#include <catch.hpp>

#include <string>
#include <algorithm>

#include <stlext/containers/packed_hashtbl.hpp>


TEST_CASE("packed_hashset/clear", "[containers]")
{
	using namespace stdx;
	typedef packed_hashset<std::string, 16> strset;
	strset stringset;
	stringset.insert("very very long string that cannot fit into local memory of std::basic_string<_Char, _Traits, _Alloc> class");
	stringset.insert("1234567890");
	stringset.insert("0123456789ABCDEF");
	REQUIRE_FALSE(stringset.empty());
	REQUIRE(stringset.size() == 3);

	stringset.clear();
	REQUIRE(stringset.empty());
}


TEST_CASE("packed_hashset/insert", "[containers]")
{
	using namespace stdx;
	typedef packed_hashset<std::string, 32> strset;
	strset stringset;
	stringset.insert("very very long string that cannot fit into local memory of std::basic_string<_Char, _Traits, _Alloc> class");
	stringset.insert("1234567890");
	stringset.insert("0123456789ABCDEF");
	REQUIRE(stringset.size() == 3);
	REQUIRE_FALSE(stringset.empty());
	REQUIRE(stringset.find("1234567890") != stringset.end());
	REQUIRE(stringset.find("0123456789ABCDEF") != stringset.end());

	for (size_t i = stringset.size(); i < strset::max_capacity; i++) {
		stringset.insert("abc" + std::to_string(i));
	}
	REQUIRE(stringset.full());
	REQUIRE(stringset.find("abc8") != stringset.end());
	REQUIRE(stringset.count("abc23") > 0);
	REQUIRE_FALSE(stringset.count("ccc23") > 0);
}


TEST_CASE("packed_hashset/erase", "[containers]")
{
	using namespace stdx;
	typedef packed_hashset<std::string, 32> strset;
	strset stringset;
	stringset.insert("very very long string that cannot fit into local memory of std::basic_string<_Char, _Traits, _Alloc> class");
	stringset.insert("1234567890");
	stringset.insert("0123456789ABCDEF");
	REQUIRE(stringset.size() == 3);
	REQUIRE_FALSE(stringset.empty());
	REQUIRE(stringset.find("1234567890") != stringset.end());
	REQUIRE(stringset.find("0123456789ABCDEF") != stringset.end());

	for (size_t i = stringset.size(); i < strset::max_capacity; i++) {
		stringset.insert("abc" + std::to_string(i));
	}

	stringset.erase("1234567890");
	REQUIRE(stringset.find("1234567890") == stringset.end());
	stringset.erase("abc21");
	REQUIRE(stringset.find("abc21") == stringset.end());
	stringset.erase("abc20");
	REQUIRE(stringset.find("abc20") == stringset.end());
	stringset.erase("abc30");
	REQUIRE(stringset.find("abc30") == stringset.end());
	stringset.erase("abc8");
	REQUIRE(stringset.find("abc8") == stringset.end());
	stringset.erase("0123456789ABCDEF");
	REQUIRE(stringset.find("0123456789ABCDEF") == stringset.end());
}

