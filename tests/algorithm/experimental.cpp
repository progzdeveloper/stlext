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



TEST_CASE("algorithms/simplify", "[algorithm.experimental]")
{
    std::string s = "\r   \nHello, \tWorld ! \n  ?";
    auto escape = [](char c) {
        return (c == '\r' || c == '\n' || c == '\t' ||
                c == ' '  || c == '!' ||
                c == '?'  || c == '&' ||
                c == ','  || c == '.');
    };
    s.erase(stdx::simplify(s.begin(), s.end(), s.begin(), escape, ' '), s.end());
    REQUIRE(s == "Hello World");

    std::string s2 = "Hello, \tWorld ! \n  ?";
    s2.erase(stdx::simplify(s2.begin(), s2.end(), s2.begin(), escape, ' '), s2.end());
    REQUIRE(s2 == "Hello World");

    std::string s3 = "\r   \nHello, \tWorld";
    s3.erase(stdx::simplify(s3.begin(), s3.end(), s3.begin(), escape, ' '), s3.end());
    REQUIRE(s3 == "Hello World");

    std::ostringstream output;
    std::stringstream buffer;
    buffer << "Hello, \tWorld ! \n  ? \n"
           << "Hello, \tWorld ! \n  ?"
           << "\r   \nHello, \tWorld              ";

    stdx::simplify(std::istreambuf_iterator<char>(buffer),
                   std::istreambuf_iterator<char>(),
                   std::ostreambuf_iterator<char>(output), escape, ' ');

    REQUIRE(output.str() == "Hello World Hello World Hello World");

    std::string s4 = " Hello World ";
    s4.erase(stdx::simplify(s4.begin(), s4.end(), s4.begin(), escape, ' '), s4.end());
    REQUIRE(s4 == "Hello World");
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



TEST_CASE("algorithms/share_element", "[algorithm.experimental]")
{
    bool has_shared = false;
    std::vector<int> s1 = { -8,  -5,  -1,  0,  3,  5,  7, 8, 9 };
    std::vector<int> s2 = { -19, -17, -9, -6, -4, -3, -2, 0, 2 };
    std::vector<int> s3 = { -19, -17, -9, -6, -4, -3, -2, 1, 2 };
    has_shared = stdx::share_element(s1.begin(), s1.end(), s2.begin(), s2.end());
    REQUIRE(has_shared);

    has_shared = stdx::share_element(s1.begin(), s1.end(), s3.begin(), s3.end());
    REQUIRE(!has_shared);
}



TEST_CASE("algorithms/kway_merge", "[algorithm.experimental]")
{
    using namespace std;
    vector< vector<int> > arr{ { 2, 2, 2, 6, 6, 12 },
                               { 1, 2, 2, 6, 6, 6, 9 },
                               { 23, 34, 90, 2000 } };

    vector<int> desired_less = {
        1, 2, 2, 2, 2, 2, 6, 6, 6, 6, 6, 9, 12, 23, 34, 90, 2000
    };

    vector<int> desired_greater = {
        2000, 90, 34, 23, 12, 9, 6, 6, 6, 6, 6, 2, 2, 2, 2, 2, 1
    };

    typedef vector<int>::iterator iter_t;
    typedef std::pair<iter_t, iter_t> iter_pair;
    vector< iter_pair > inputs {
            { arr[0].begin(), arr[0].end() },
            { arr[1].begin(), arr[1].end() },
            { arr[2].begin(), arr[2].end() }
    };

    vector<int> merged;
    merged.reserve(arr[0].size() + arr[1].size() + arr[2].size());

    stdx::kway_merge(inputs, back_inserter(merged));
    REQUIRE(merged == desired_less);
    merged.clear();



    for (auto& x : arr) {
        std::sort(x.begin(), x.end(), std::greater<>{});
    }

    stdx::kway_merge(arr, back_inserter(merged), std::greater<>{});
    REQUIRE(merged == desired_greater);
}


TEST_CASE("algorithms/kway_union", "[algorithm.experimental]")
{
    using namespace std;
    vector< vector<int> > arr{ { 2, 2, 2, 6, 6, 12 },
                               { 1, 2, 2, 6, 6, 6, 9 },
                               { 23, 34, 90, 2000 } };

    vector<int> desired_less = {
        1, 2, 6, 9, 12, 23, 34, 90, 2000
    };

    vector<int> desired_greater = {
        2000, 90, 34, 23, 12, 9, 6, 2, 1
    };

    typedef vector<int>::iterator iter_t;
    typedef std::pair<iter_t, iter_t> iter_pair;
    vector< iter_pair > inputs {
            { arr[0].begin(), arr[0].end() },
            { arr[1].begin(), arr[1].end() },
            { arr[2].begin(), arr[2].end() }
    };

    vector<int> merged;
    merged.reserve(arr[0].size() + arr[1].size() + arr[2].size());

    stdx::kway_union(inputs, back_inserter(merged));
    REQUIRE(merged == desired_less);
    merged.clear();


    for (auto& x : arr) {
        std::sort(x.begin(), x.end(), std::greater<>{});
    }

    stdx::kway_union(arr, back_inserter(merged), std::greater<>{});
    REQUIRE(merged == desired_greater);
}

TEST_CASE("algorithms/kway_intersect", "[algorithm.experimental]")
{
    using namespace std;

    vector< vector<int> > arr00{ {}, {} };
    vector< vector<int> > arr01{ {}, { -3, -1, 2, 6, 9} };
    vector< vector<int> > arr02{ {}, { -3, -1, 2, 6, 9}, { -3, -1, 2, 6, 9 } };

    vector< vector<int> > arr1{ { -5, -3, -2, -2, -1, -1, 2, 2, 2, 6, 6, 9, 12 },
                                { -4, -3, -1, 1, 2, 2, 6, 6, 6, 9 },
                                { -7, -3, -1, 2, 6, 7, 8, 9, 23, 34, 90, 2000 } };

    vector< vector<int> > arr2{ { -5, -3, -2, -2, -1, -1, 2, 2, 2, 6, 6, 9, 12 },
                                { -4, -3, -1, 1, 2, 2, 6, 6, 6, 9 } };

    vector< vector<int> > arr3{ { -5, -3, -2, -2, -1, -1, 2, 2, 2, 6, 6, 9, 12 },
                                { -4, -3, -1, 1, 2, 2, 6, 6, 6, 9 },
                                { -7, -3, -1, 2, 6, 7, 8, 9, 23, 34, 90, 2000 },
                                { -5, -3, -2, -2, -1, -1, 2, 2, 2, 6, 6, 9, 12 },
                                { -4, -3, -1, 1, 2, 2, 6, 6, 6, 9 },
                                { -7, -3, -1, 2, 6, 7, 8, 9, 23, 34, 90, 2000 },
                                { -5, -3, -2, -2, -1, -1, 2, 2, 2, 6, 6, 9, 12 },
                                { -4, -3, -1, 1, 2, 2, 6, 6, 6, 9 },
                                { -7, -3, -1, 2, 6, 7, 8, 9, 23, 34, 90, 2000 },
                                { -4, -3, -1, 1, 2, 2, 6, 6, 6, 9 },
                                { -7, -3, -1, 2, 6, 7, 8, 9, 23, 34, 90, 2000 },
                                { -7, -3, -1, 2, 6, 7, 8, 9, 23, 34, 90, 2000 } };


    vector<int> desired = { -3, -1, 2, 6, 9 };

    typedef vector<int>::iterator iter_t;
    typedef std::pair<iter_t, iter_t> iter_pair;

    vector< iter_pair > inputs00 {
            { arr00[0].begin(), arr00[0].end() },
            { arr00[1].begin(), arr00[1].end() }
    };

    vector< iter_pair > inputs01 {
            { arr01[0].begin(), arr01[0].end() },
            { arr01[1].begin(), arr01[1].end() }
    };
    vector< iter_pair > inputs02 {
            { arr02[0].begin(), arr02[0].end() },
            { arr02[1].begin(), arr02[1].end() },
            { arr02[2].begin(), arr02[2].end() }
    };

    vector< iter_pair > inputs1 {
            { arr1[0].begin(), arr1[0].end() },
            { arr1[1].begin(), arr1[1].end() },
            { arr1[2].begin(), arr1[2].end() }
    };

    vector< iter_pair > inputs2 {
            { arr2[0].begin(), arr2[0].end() },
            { arr2[1].begin(), arr2[1].end() }
    };

    vector< iter_pair > inputs3 {
        { arr3[0].begin(),  arr3[0].end() },
        { arr3[1].begin(),  arr3[1].end() },
        { arr3[2].begin(),  arr3[2].end() },
        { arr3[3].begin(),  arr3[3].end() },
        { arr3[4].begin(),  arr3[4].end() },
        { arr3[5].begin(),  arr3[5].end() },
        { arr3[6].begin(),  arr3[6].end() },
        { arr3[7].begin(),  arr3[7].end() },
        { arr3[8].begin(),  arr3[8].end() },
        { arr3[9].begin(),  arr3[9].end() },
        { arr3[10].begin(), arr3[10].end() },
        { arr3[11].begin(), arr3[11].end() }
    };

    vector<int> intersection;

    stdx::kway_intersect(inputs00, back_inserter(intersection));
    REQUIRE(intersection.empty());
    intersection.clear();

    stdx::kway_intersect(inputs01, back_inserter(intersection));
    REQUIRE(intersection.empty());
    intersection.clear();

    stdx::kway_intersect(inputs02, back_inserter(intersection));
    REQUIRE(intersection == desired);
    intersection.clear();

    stdx::kway_intersect(inputs1, back_inserter(intersection));
    REQUIRE(intersection == desired);
    intersection.clear();

    stdx::kway_intersect(inputs2, back_inserter(intersection));
    REQUIRE(intersection == desired);
    intersection.clear();

    stdx::kway_intersect(inputs3, back_inserter(intersection));
    REQUIRE(intersection == desired);
    intersection.clear();
}

