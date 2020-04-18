#include <catch.hpp>

// Continious storage
#include <string>
#include <array>
#include <vector>
#include <deque>

// Node-based
#include <forward_list>
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


#include <stlext/algorithm/experimental.h>

namespace detail {


template<class _Container>
static inline void __slide_apply_helper()
{
    using namespace std;
    static const size_t kWidth = 4;
    _Container vec;
    for (int i = 0; i < 10; i++)
        vec.push_back(i);
    typedef typename _Container::iterator vecit;
    typedef pair<vecit, vecit> iter_range;

    vector<iter_range> vranges_orig;
    vector<iter_range> vranges;
    auto lambda0 = [&vranges_orig](vecit f, vecit l) {
        vranges_orig.emplace_back(f, l);
    };
    auto lambda1 = [&vranges](vecit f, vecit l) {
        vranges.emplace_back(f, l);
    };
    stdx::inclusive_slide(vec.begin(), vec.end(), kWidth, lambda0);

    for (size_t i = 0; i < vranges_orig.size(); i++) {
        REQUIRE(equal(next(vec.begin(), i), next(vec.begin(), (i + kWidth)), vranges_orig[i].first));
    }

    vec.pop_back();

    stdx::inclusive_slide(vec.begin(), vec.end(), kWidth, lambda1);
    REQUIRE((vranges_orig.size() - 1) == vranges.size());
    for (size_t i = 0; i < vranges.size(); i++) {
        REQUIRE(equal(next(vec.begin(), i), next(vec.begin(), (i + kWidth)), vranges_orig[i].first));
    }

    vranges.clear();
    vec.resize(3);
    stdx::exclusive_slide(vec.begin(), vec.end(), kWidth, lambda1);
    REQUIRE(vranges.empty());

    stdx::inclusive_slide(vec.begin(), vec.end(), kWidth, lambda1);
    REQUIRE(vranges.size() == size_t(1));
    //REQUIRE(std::distance(vec.begin(), vec.end()) == distance(vranges[0].first, vranges[0].second), L"subrange must be equal size with vector");
    REQUIRE(equal(vec.begin(), vec.end(), vranges[0].first));
}


} // end namespace detail


TEST_CASE("algorithms/slide_apply", "[algorithm.experimental]")
{
	::detail::__slide_apply_helper< std::vector<int> >();
	::detail::__slide_apply_helper< std::list<int> >();
}


TEST_CASE("algorithms/majority_element", "[algorithm.experimental]")
{
	using namespace std;
	const char* strs[] = { "aB", "Ab", "ab", "aA", "Ac", "AB", "ca", "cc", "Ab" };
	int random[] = { 0, 1, 0, 0, 2, 0, 3 };
	int ident[] = { 9, 9, 9, 9, 9, 9, 9 };
	int increase[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int uniform[] = { 0, 0, 0, 0,  1, 1, 1, 1,  -1, -1, -1, -1, 9, 9, 9, 9 };

	int* it = nullptr;
	it = stdx::majority_element(begin(random), end(random));
	REQUIRE(*it == 0);

	it = stdx::majority_element(begin(ident), end(ident));
	REQUIRE(it == begin(ident));

	it = stdx::majority_element(begin(increase), end(increase));
	REQUIRE(it == end(increase));

	it = stdx::majority_element(begin(uniform), end(uniform));
	REQUIRE(it == end(uniform));

	auto char_comp = [](char a, char b) { return tolower(a) == tolower(b); };
	auto sIt = stdx::majority_element(begin(strs), end(strs), [&char_comp](const char* a, const char* b) {
		return std::equal(a, a + strlen(a), b, b + strlen(b), char_comp);
	});
	string s = *sIt;
	transform(s.begin(), s.end(), s.begin(), [](char c) { return tolower(c); });
	REQUIRE(s == "ab");
}


TEST_CASE("algorithms/is_majority_element", "[algorithm.experimental]")
{
	using namespace std;
	const char* strs[] = { "aB", "Ab", "ab", "aA", "Ac", "AB", "ca", "cc", "Ab" };
	int random[]   = { 0, 1, 0, 0, 2, 0, 3 };
	int ident[]    = { 9, 9, 9, 9, 9, 9, 9 };
	int increase[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    //int uniform[]  = { 0, 0, 0, 0,  1, 1, 1, 1,  -1, -1, -1, -1,  9, 9, 9, 9 };

	REQUIRE(stdx::is_majority_element(begin(random), end(random), 0));
	REQUIRE(stdx::is_majority_element(begin(ident), end(ident), 9));
	REQUIRE_FALSE(stdx::is_majority_element(begin(increase), end(increase), 2));
	REQUIRE_FALSE(stdx::is_majority_element(begin(increase), end(increase), -1));

	auto char_comp = [](char a, char b) { return tolower(a) == tolower(b); };
	REQUIRE(stdx::is_majority_element(begin(strs), end(strs), "ab", [&char_comp](const char* a, const char* b) {
		return std::equal(a, a + strlen(a), b, b + strlen(b), char_comp);
	}));
}



TEST_CASE("algorithms/is_unique", "[algorithm.experimental]")
{
	typedef std::ostream_iterator<std::string> output_iterator;
	typedef std::istream_iterator<std::string> input_iterator;
	typedef std::ostream_iterator<std::wstring, wchar_t> woutput_iterator;
	typedef std::istream_iterator<std::wstring, wchar_t> winput_iterator;
	std::string cstr = "0123456789";
	std::wstring wstr = L"0123456789";
	REQUIRE(stdx::is_unique(cstr));
	REQUIRE(stdx::is_unique(wstr));
	REQUIRE(stdx::is_unique("0123456789"));
	REQUIRE(stdx::is_unique(L"0123456789"));

	std::vector<int> v1 = { -1, 0, 1, 3, 5 };
	REQUIRE(stdx::is_unique(v1.begin(), v1.end()));

	std::list<std::string> l2 = { "one", "two", "three" };
	REQUIRE(stdx::is_unique(l2.begin(), l2.end()));
	
	std::forward_list<std::string> f2 = { "one", "two", "three" };
	REQUIRE(stdx::is_unique(f2.begin(), f2.end()));

	std::forward_list<std::wstring> wf2 = { L"one", L"two", L"three" };
	REQUIRE(stdx::is_unique(wf2.begin(), wf2.end()));


	std::stringstream buf;
	std::copy(l2.begin(), l2.end(), output_iterator(buf, "\n"));
	REQUIRE(stdx::is_unique(input_iterator(buf), input_iterator()));

	buf.clear();
	buf.seekg(0);
	REQUIRE_FALSE(stdx::is_unique(std::istreambuf_iterator<char>(buf),
								  std::istreambuf_iterator<char>()));


	std::wstringstream wbuf;
	std::copy(wf2.begin(), wf2.end(), woutput_iterator(wbuf, L"\n"));
	REQUIRE(stdx::is_unique(winput_iterator(wbuf), winput_iterator()));

	wbuf.clear();
	wbuf.seekg(0);
	REQUIRE_FALSE(stdx::is_unique(std::istreambuf_iterator<wchar_t>(wbuf),
								  std::istreambuf_iterator<wchar_t>()));
}



TEST_CASE("algorithms/kadane_sum", "[algorithm.experimental]")
{
	std::vector<int> v = { 0, -1, -2, 3, 1, -2, 5, 3, 4, -1, 0 };
	auto max_weight = stdx::kadane_max(v.begin(), v.end());
	auto min_weight = stdx::kadane_min(v.begin(), v.end());
	auto max_result = stdx::kadane_range_max(v.begin(), v.end());
	auto min_result = stdx::kadane_range_min(v.begin(), v.end());

	REQUIRE(std::get<0>(max_result) == max_weight);
	REQUIRE(std::get<0>(min_result) == min_weight);
}

TEST_CASE("algorithms/split", "[algorithm.experimental]")
{
    static const auto predicate = [](char c) { return (std::isspace(c) || std::ispunct(c)); };

    int count = 0;

    std::string delims;
    stdx::split(delims.begin(), delims.end(), predicate, [&count](auto /*first*/, auto /*last*/) {
        ++count;
    });
    REQUIRE(count == 0);

    delims = "?!,... ;:[]{\t}<>/\\^%$##@@)(  \r\n\r\n  /).\"''|*-%    ";
    stdx::split(delims.begin(), delims.end(), predicate, [&count](auto /*first*/, auto /*last*/) {
        ++count;
    });
    REQUIRE(count == 0);

    std::string strs[] = {
        "A  very \t\t long    string   with \t some,   delimiters",
        "?  A  very \t\t long    string   with \t some,   delimiters",
        "?  A  very \t\t long    string   with \t some,   delimiters!  \r\n"
    };

    for (auto it = std::begin(strs); it != std::end(strs); ++it)
    {
        count = 0;
        stdx::split(it->begin(), it->end(), predicate, [&count](auto first, auto last) {
            std::string  tmp(first, last);
            switch (count) {
            case 0:
                REQUIRE(tmp == "A"); break;
            case 1:
                REQUIRE(tmp == "very"); break;
            case 2:
                REQUIRE(tmp == "long"); break;
            case 3:
                REQUIRE(tmp == "string"); break;
            case 4:
                REQUIRE(tmp == "with"); break;
            case 5:
                REQUIRE(tmp == "some"); break;
            case 6:
                REQUIRE(tmp == "delimiters"); break;
            }
            ++count;
        });
        REQUIRE(count == 7);
    }
}

TEST_CASE("algorithms/split_copy", "[algorithm.experimental]")
{
    using namespace std;
    static const auto predicate = [](char c) { return (std::isspace(c) || std::ispunct(c)); };
    string str;
    vector<string> expected;
    vector<string> results;

    stdx::split_copy(str, back_inserter(results), predicate);
    REQUIRE(results == expected);

    str = "?!,... ;:[]{\t}<>/\\^%$##@@)(  \r\n\r\n  /).\"''|*-%    ";
    stdx::split_copy(str, back_inserter(results), predicate);
    REQUIRE(results == expected);

    str = "?  A  very \t\t long    string   with \t some,   delimiters!   \r\n";
    expected = { "A", "very", "long", "string", "with", "some", "delimiters" };

    stdx::split_copy(str, back_inserter(results), predicate);
    REQUIRE(results == expected);
}

TEST_CASE("algorithms/regex_split", "[algorithm.experimental]")
{
	const char* cexpr = "(\\w+)";
	const char* csrc = "Hello C++ World!";

	std::regex rx(cexpr);
	std::string expr(cexpr);
	std::string str(csrc);
	
	std::vector<std::string> results;
	results.reserve(3);

	stdx::regex_split(cexpr, csrc, std::back_inserter(results));
	REQUIRE(results[0] == "Hello");
	REQUIRE(results[1] == "C");
	REQUIRE(results[2] == "World");
	results.clear();

	stdx::regex_split(expr, str, std::back_inserter(results));
	REQUIRE(results[0] == "Hello");
	REQUIRE(results[1] == "C");
	REQUIRE(results[2] == "World");
	results.clear();


	stdx::regex_split(rx, csrc, std::back_inserter(results));
	REQUIRE(results[0] == "Hello");
	REQUIRE(results[1] == "C");
	REQUIRE(results[2] == "World");
	results.clear();

	stdx::regex_split(rx, str, std::back_inserter(results));
	REQUIRE(results[0] == "Hello");
	REQUIRE(results[1] == "C");
	REQUIRE(results[2] == "World");
	results.clear();

}

