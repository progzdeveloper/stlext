#include <catch.hpp>

#include <random>
#include <bitset>
#include <string>

#include <stlext/compability/cxx11/sample.hpp>
#include <stlext/compability/cxx11/all_of.hpp>
#include <stlext/compability/cxx11/any_of.hpp>
#include <stlext/compability/cxx11/none_of.hpp>
#include <stlext/compability/cxx11/one_of.hpp>

#include <stlext/compability/cxx11/copy_if.hpp>
#include <stlext/compability/cxx11/copy_n.hpp>

#include <stlext/compability/cxx11/is_sorted.hpp>
#include <stlext/compability/cxx11/is_partitioned.hpp>
#include <stlext/compability/cxx11/is_permutation.hpp>

#include <stlext/compability/cxx11/partition_copy.hpp>

TEST_CASE("algorithms/sample", "[compability.cpp11]")
{
	static const size_t CHARSET_SIZE = 1 << CHAR_BIT * sizeof(std::string::value_type);
	std::string alphabet = "0123456789ABCDEF";
	std::string shortstr(4, '\0'), longstr(8, '\0');
	std::bitset<CHARSET_SIZE> bits;
	for (auto it = alphabet.begin() + 10; it != alphabet.end(); ++it)
		bits.set(*it);

	auto is_allowed_char = [&bits](const char c) { return !bits.test(c); };

	std::mt19937 rnd;
	for (size_t i = 0; i < 128; i++)
	{
		stdx::sample(alphabet.begin(), alphabet.begin() + 10, shortstr.begin(), shortstr.size(), rnd);
		stdx::sample(alphabet.begin(), alphabet.begin() + 10, longstr.begin(), longstr.size(), rnd);

		REQUIRE(shortstr != longstr);
		REQUIRE(all_of(shortstr.begin(), shortstr.end(), is_allowed_char));
	}
}





TEST_CASE("algorithms/all_of", "[compability.cpp11]")
{
	std::string str1 = "0000000000";
	std::string str2 = "0123456789";
	REQUIRE(stdx::all_of(str1.begin(), str1.end(), [](char c) { return c == '0'; }));
	REQUIRE_FALSE(stdx::all_of(str2.begin(), str2.end(), [](char c) { return c == '0'; }));
	REQUIRE_FALSE(stdx::all_of(str1.begin(), str1.end(), [](char c) { return c != '0'; }));
	REQUIRE_FALSE(stdx::all_of(str2.begin(), str2.end(), [](char c) { return c != '0'; }));
	REQUIRE(stdx::all_of(str1.begin(), str1.end(), [](char c) { return c != 'a'; }));
	REQUIRE_FALSE(stdx::all_of(str1.begin(), str1.end(), [](char c) { return c == 'a'; }));
}

TEST_CASE("algorithms/any_of", "[compability.cpp11]")
{
	std::string str1 = "0000000000";
	std::string str2 = "0123456789";
	REQUIRE(stdx::any_of(str1.begin(), str1.end(), [](char c) { return c == '0'; }));
	REQUIRE(stdx::any_of(str2.begin(), str2.end(), [](char c) { return c == '0'; }));
	REQUIRE_FALSE(stdx::any_of(str1.begin(), str1.end(), [](char c) { return c != '0'; }));
	REQUIRE(stdx::any_of(str2.begin(), str2.end(), [](char c) { return c != '0'; }));
	REQUIRE(stdx::any_of(str1.begin(), str1.end(), [](char c) { return c != 'a'; }));
	REQUIRE_FALSE(stdx::any_of(str1.begin(), str1.end(), [](char c) { return c == 'a'; }));
}

TEST_CASE("algorithms/none_of", "[compability.cpp11]")
{
	std::string str1 = "0000000000";
	std::string str2 = "0123456789";
	REQUIRE_FALSE(stdx::none_of(str1.begin(), str1.end(), [](char c) { return c == '0'; }));
	REQUIRE_FALSE(stdx::none_of(str2.begin(), str2.end(), [](char c) { return c == '0'; }));
	REQUIRE(stdx::none_of(str1.begin(), str1.end(), [](char c) { return c != '0'; }));
	REQUIRE_FALSE(stdx::none_of(str2.begin(), str2.end(), [](char c) { return c != '0'; }));
	REQUIRE_FALSE(stdx::none_of(str1.begin(), str1.end(), [](char c) { return c != 'a'; }));
	REQUIRE(stdx::none_of(str1.begin(), str1.end(), [](char c) { return c == 'a'; }));
}

TEST_CASE("algorithms/one_of", "[compability.cpp11]")
{
	std::string str1 = "0000000000";
	std::string str2 = "0123456789";
	REQUIRE_FALSE(stdx::one_of(str1.begin(), str1.end(), [](char c) { return c == '0'; }));
	REQUIRE(stdx::one_of(str2.begin(), str2.end(), [](char c) { return c == '0'; }));
	REQUIRE_FALSE(stdx::one_of(str1.begin(), str1.end(), [](char c) { return c != '0'; }));
	REQUIRE_FALSE(stdx::one_of(str2.begin(), str2.end(), [](char c) { return c != '0'; }));
	REQUIRE_FALSE(stdx::one_of(str1.begin(), str1.end(), [](char c) { return c != 'a'; }));
	REQUIRE_FALSE(stdx::one_of(str1.begin(), str1.end(), [](char c) { return c == 'a'; }));
}


TEST_CASE("algorithms/copy_if", "[compability.cpp11]")
{

	std::vector<int> v = { -1, 0, 1, 0, 2, 0, 3, 0, 4, 0 };
	std::vector<int> r;

	stdx::copy_if(v.begin(), v.end(), back_inserter(r), [](int x) { return x == 0; });
	REQUIRE(stdx::all_of(r.begin(), r.end(), [](int x) { return x == 0; }));
}



TEST_CASE("algorithms/copy_n", "[compability.cpp11]")
{
	std::vector<int> v = { -1, 0, 1, 0, 2, 0, 3, 0, 4, 0 };
	std::vector<int> r;

	stdx::copy_n(v.begin(), 5, back_inserter(r));
	REQUIRE(std::equal(r.begin(), r.end(), v.begin()));
}


TEST_CASE("algorithms/is_sorted", "[compability.cpp11]")
{
	std::vector<int> v = { -1, 0, 1, 2, 3, 4, 5 };
	REQUIRE(stdx::is_sorted(v.begin(), v.end()));
}

TEST_CASE("algorithms/is_partitioned", "[compability.cpp11]")
{
	std::vector<int> v = { -1, -2, -3, 1, 3, 4, 5 };
	REQUIRE(stdx::is_partitioned(v.begin(), v.end(), [](int x) { return x < 0; }));
}

TEST_CASE("algorithms/is_permutation", "[compability.cpp11]")
{
	std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6 };

	std::vector<int> p1 = v;
	std::next_permutation(p1.begin(), p1.end());
	REQUIRE(stdx::is_permutation(p1.begin(), p1.end(), v.begin()));

	std::vector<int> p2 = v;
	std::next_permutation(p2.begin(), p2.end(), std::equal_to<int>());
	REQUIRE(stdx::is_permutation(p2.begin(), p2.end(), v.begin(), std::equal_to<int>()));
}


TEST_CASE("algorithms/partition_copy", "[compability.cpp11]")
{
	std::vector<int> v = { -3, 1, -1, 5, -2, 0, 2, 3 };

	std::vector<int> r1;
	std::vector<int> r2;
	stdx::partition_copy(v.begin(), v.end(), 
						 std::back_inserter(r1), 
						 std::back_inserter(r2),
						 [](int x) { return x >= 0; });

	REQUIRE(r1 == std::vector<int>({ 1, 5, 0, 2, 3 }));
	REQUIRE(r2 == std::vector<int>({ -3, -1, -2 }));
}

