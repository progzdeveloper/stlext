#include <catch.hpp>

#include <unordered_set>
#include <list>
#include <forward_list>
#include <vector>
#include <sstream>
#include <numeric>
#include <algorithm>

#include <stlext/iterator/any_iterator.hpp>
#include <stlext/iterator/istring_iterator.hpp>
#include <stlext/iterator/uinserter.hpp>
#include <stlext/iterator/circular_iterator.hpp>


namespace helpers
{
	int accumulate_any(stdx::any_iterator<int, std::random_access_iterator_tag> first,
					   stdx::any_iterator<int, std::random_access_iterator_tag> second)
	{
		int result = 0;
		for (auto it = first; it != second; ++it) {
			result += *it;
		}
		return result;
	}
}

TEST_CASE("iterators/istring_iterator", "[iterators]")
{
	using namespace std;
	string text = "one two\nthree\tfour\t five\r\nsix\n\nseven";
	stringstream buf(text, ios::in | ios::out);
	list<string> results;
	copy(stdx::istring_iterator<char>(buf), stdx::istring_iterator<char>(), back_inserter(results));
	REQUIRE(results.size() == 5);
}


TEST_CASE("iterators/uinsert_iterator", "[iterators]")
{
	using namespace std;
	string text = "one two\nthree\tfour\t five\r\nsix\n\nseven";
	stringstream buf(text, ios::in | ios::out);
	unordered_set<string> results;
	copy(stdx::istring_iterator<char>(buf, 8), stdx::istring_iterator<char>(), stdx::uinserter(results));
	REQUIRE(results.size() == 5);
}


TEST_CASE("iterators/any_iterator", "[iterators]")
{
	using namespace std;
	vector<int> iv;
	list<int> il;

	for (int i = 0; i < 10; i++)
		iv.push_back(i);
	copy(iv.begin(), iv.end(), back_inserter(il));

	int iv_res = helpers::accumulate_any(iv.begin(), iv.end());
	int il_res = helpers::accumulate_any(il.begin(), il.end());
	int result = std::accumulate(iv.begin(), iv.end(), 0);

	REQUIRE(iv_res == result);
	REQUIRE(il_res == result);
}


template<class _It>
static inline void __circular_iterator_inc(_It first, _It last)
{
	stdx::circular_iterator<_It> it(first, last);
	auto d = std::distance(first, last);
	while (d-- > 0) {
		REQUIRE(*it == *first);
		++it, ++first;
	}
}

template<class _It>
static inline void __circular_iterator_overflow(_It first, _It last)
{
	stdx::circular_iterator<_It> it(first, last);
	auto d = std::distance(first, last);

	it = std::next(it, 2 * d);
	REQUIRE(*it == *first);

	it = first;
	it = std::prev(it, 2 * d);
	REQUIRE(*it == *first);

	it = first;
	it = std::next(it, 3 * d);
	REQUIRE(*it == *first);

	it = first;
	it = std::prev(it, 3 * d);
	REQUIRE(*it == *first);

	it = first;
	it = std::next(it, 4 * d);
	REQUIRE(*it == *first);

	it = first;
	it = std::prev(it, 4 * d);
	REQUIRE(*it == *first);

	it = first;
	it = std::next(it, 2 * d + 1);
	REQUIRE(*it == *next(first));

	it = first;
	it = std::prev(it, 5 * d + 2);
	first = std::prev(last);
	REQUIRE(*it == *std::prev(first));
}



TEST_CASE("iterators/circular_iterator", "[iterators]")
{
	using namespace std;
	std::vector<int> v = { 0, 1, 2, 3, 4, 5, 6 };
	std::list<int> lst = { 0, 1, 2, 3, 4, 5, 6 };
	std::forward_list<int> fwd_lst = { 0, 1, 2, 3, 4, 5, 6 };

	__circular_iterator_inc(v.begin(), v.end());
	__circular_iterator_inc(lst.begin(), lst.end());
	__circular_iterator_inc(fwd_lst.begin(), fwd_lst.end());

	__circular_iterator_overflow(v.begin(), v.end());
	__circular_iterator_overflow(lst.begin(), lst.end());
	//__circular_iterator_overflow(fwd_lst.begin(), fwd_lst.end());
}



