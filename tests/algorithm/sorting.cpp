#include <catch.hpp>

#include <bitset>

// Continious storage
#include <string>
#include <array>
#include <vector>
#include <deque>

// Node-based
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

// Adaptors
#include <queue>
#include <stack>

#include <algorithm>

#if _MSC_VER >= 1800
#include <concurrent_queue.h>
#include <concurrent_vector.h>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#endif

#include <stlext/platform/bits.h>
#include <stlext/algorithm/sorting.h>
#include <stlext/compability/cxx11/sample.hpp>


TEST_CASE("algorithms/insertion_sort", "[algorithm.sorting]")
{
	using namespace std;
	vector<int> v(10);
	for (int i = 0; i < 10; i++)
		v[i] = i;

	random_shuffle(v.begin(), v.end());
	stdx::insertion_sort(v.begin(), v.end());
	REQUIRE(std::is_sorted(v.begin(), v.end()));

	string s = "Hello World!";
	stdx::insertion_sort(s.begin(), s.end());
	REQUIRE(std::is_sorted(s.begin(), s.end()));

	deque<string> dq;
	for (int i = 0; i < 10; i++)
		dq.push_back(to_string(i * 0.5f));
	random_shuffle(dq.begin(), dq.end());
	stdx::insertion_sort(dq.begin(), dq.end());
	REQUIRE(std::is_sorted(dq.begin(), dq.end()));
}


TEST_CASE("algorithms/selection_sort", "[algorithm.sorting]")
{
	using namespace std;
	vector<int> v(10);
	for (int i = 0; i < 10; i++)
		v[i] = i;

	random_shuffle(v.begin(), v.end());
	stdx::selection_sort(v.begin(), v.end());
	REQUIRE(std::is_sorted(v.begin(), v.end()));

	string s = "Hello World!";
	stdx::selection_sort(s.begin(), s.end());
	REQUIRE(std::is_sorted(s.begin(), s.end()));

	deque<string> dq;
	for (int i = 0; i < 10; i++)
		dq.push_back(to_string(i * 0.5f));
	random_shuffle(dq.begin(), dq.end());
	stdx::selection_sort(dq.begin(), dq.end());
	REQUIRE(std::is_sorted(dq.begin(), dq.end()));
}



TEST_CASE("algorithms/counting_sort", "[algorithm.sorting]")
{
	using namespace std;

	hash<string> hasher;
	auto string_hash = [&hasher](const string& s) {
		return (uint8_t)hasher(s);
	};
	auto string_hash_cmp = [&string_hash](const string& s1, const string& s2) {
		return string_hash(s1) < string_hash(s2);
	};

	auto string_proj = [](const string& s) {
        return stdx::__byte_swap(*reinterpret_cast<const uint32_t*>(s.data()));
	};
	auto string_proj_cmp = [&string_proj](const string& s1, const string& s2) {
		return string_proj(s1) < string_proj(s2);
	};

	std::mt19937 rnd;
	string s(5, '\0');
	string alpha = "abcdefghijklmnopqrstuvwxyz0123456789";

	vector<int> v(100);
	for (size_t i = 0; i < v.size(); i++)
		v[i] = i % 10;
	random_shuffle(v.begin(), v.end());
	stdx::counting_sort(v.begin(), v.end());
	REQUIRE(std::is_sorted(v.begin(), v.end()));

	v.resize(10);
	for (size_t i = 0; i < v.size(); i++)
		v[i] = static_cast<int>(i * 100);

	random_shuffle(v.begin(), v.end());
	stdx::counting_sort(v.begin(), v.end());
	REQUIRE(std::is_sorted(v.begin(), v.end()));

	vector<string> patterns = {
		"0144",
		"0145",
		"0143",
		"0142",
		"0140",
		"0149",
		"0143",
		"0147",
		"0144",
		"0140"
	};
	vector<string> strs;
	strs.reserve(45000);
	while (strs.size() < strs.capacity()) {
		copy(patterns.begin(), patterns.end(), back_inserter(strs));
	}
	stdx::counting_sort(strs.begin(), strs.end(), string_proj);
	REQUIRE(std::is_sorted(strs.begin(), strs.end(), string_proj_cmp));

	strs.clear();
	while (strs.size() < strs.capacity()) {
		stdx::sample(alpha.begin(), alpha.end(), s.begin(), s.size(), rnd);
		strs.emplace_back(s);
	}
	stdx::counting_sort(strs.begin(), strs.end(), string_hash);
	REQUIRE(std::is_sorted(strs.begin(), strs.end(), string_hash_cmp));

	strs.clear();
	strs.shrink_to_fit();
	while (strs.size() < 512) {
		stdx::sample(alpha.begin(), alpha.end(), s.begin(), s.size(), rnd);
		strs.emplace_back(s);
	}
	stdx::counting_sort(strs.begin(), strs.end(), string_hash);
	REQUIRE(std::is_sorted(strs.begin(), strs.end(), string_hash_cmp));
}



