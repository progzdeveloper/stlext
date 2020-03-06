#include <catch.hpp>

#include <vector>
#include <string>

#include <cstring>

#include <stlext/containers/span.hpp>

#define __STATIC_TEST_TEXT "Hello, World!"

TEST_CASE("containers/span", "[containers]")
{
    int arr[] = { 0, 1, 2, 3, 4, 5, 6 };
    const char* str = __STATIC_TEST_TEXT;

    auto array_span_fixed = stdx::make_span(arr);
    auto word_span_fixed = stdx::make_span(__STATIC_TEST_TEXT);

    REQUIRE(std::memcmp(array_span_fixed.begin(), arr, array_span_fixed.size()) == 0);
    REQUIRE(array_span_fixed.size() == std::size(arr));
    REQUIRE(array_span_fixed.space() == std::size(arr) * sizeof(*arr));

    REQUIRE(array_span_fixed.at(0) == 0);
    REQUIRE(array_span_fixed[3] == 3);
    REQUIRE(array_span_fixed.front() == 0);
    REQUIRE(array_span_fixed.back() == 6);
    REQUIRE(array_span_fixed.data() == &arr[0]);
    REQUIRE(array_span_fixed.empty() == false);

    auto arr_subspan_fixed = array_span_fixed.subspan<2, 2>(); //[2, 3]
    REQUIRE(std::memcmp(arr_subspan_fixed.data(), arr + 2, arr_subspan_fixed.size()) == 0);
    REQUIRE(arr_subspan_fixed[0] == 2);
    REQUIRE(arr_subspan_fixed.at(1) == 3);
    REQUIRE(arr_subspan_fixed.front() == 2);
    REQUIRE(arr_subspan_fixed.back() == 3);

    auto arr_subspan_dynamic = array_span_fixed.subspan(1, 5); //[1, 2, 3, 4, 5]
    REQUIRE(std::memcmp(arr_subspan_dynamic.data(), arr + 1, arr_subspan_dynamic.size()) == 0);
    REQUIRE(arr_subspan_dynamic.size() == 5);
    REQUIRE(arr_subspan_dynamic[0] == 1);
    REQUIRE(arr_subspan_dynamic.at(1) == 2);
    REQUIRE(arr_subspan_dynamic.front() == 1);
    REQUIRE(arr_subspan_dynamic.back() == 5);

    arr_subspan_dynamic.pop_back(); // [1, 2, 3, 4]
    REQUIRE(arr_subspan_dynamic.size() == 4);
    REQUIRE(arr_subspan_dynamic.front() == 1);
    REQUIRE(arr_subspan_dynamic.back() == 4);

    arr_subspan_dynamic.pop_front(); // [2, 3, 4]
    REQUIRE(arr_subspan_dynamic.size() == 3);
    REQUIRE(arr_subspan_dynamic.front() == 2);
    REQUIRE(arr_subspan_dynamic.back() == 4);


    arr_subspan_dynamic = array_span_fixed.subspan(1, 5); //[1, 2, 3, 4, 5]
    arr_subspan_dynamic.remove_prefix(2); // [3, 4, 5]
    REQUIRE(arr_subspan_dynamic.size() == 3);
    REQUIRE(arr_subspan_dynamic.front() == 3);
    REQUIRE(arr_subspan_dynamic.back() == 5);

    arr_subspan_dynamic.remove_suffix(2); // [3]
    REQUIRE(arr_subspan_dynamic.size() == 1);
    REQUIRE(arr_subspan_dynamic.front() == 3);
    REQUIRE(arr_subspan_dynamic.back() == 3);

    arr_subspan_dynamic.pop_back(); // []
    REQUIRE(arr_subspan_dynamic.size() == 0);
    REQUIRE(arr_subspan_dynamic.empty() == true);

    REQUIRE(word_span_fixed.size() == std::size(__STATIC_TEST_TEXT));
    REQUIRE(std::memcmp(word_span_fixed.begin(), str, word_span_fixed.size()) == 0);



}
