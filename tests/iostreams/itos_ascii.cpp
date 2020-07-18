#ifndef _ITOS_BENCHMARK
#include <iostream>
//#include <sstream>
//#include <iomanip>

#include <cstdlib>
#include <cstring>
#include <cstdint>

#include <catch.hpp>

#include <stlext/iostreams/itos.hpp>

#include "itos_utils.hpp"


#define TOLERANCE 1000010



//// BINARY REPRESENTATION TESTS ////

#define _ITOS stdx::itos
#define _ITOSS stdx::itos<2>

TEMPLATE_TEST_CASE("itos (base2) unsigned|signed int(8|16) ascii", "[unsigned|signed int][8|16][base2][ascii]", int8_t, uint8_t, int16_t, uint16_t)
{
    using namespace ascii;

    std::string desired, result;

    desired = to_binary(std::numeric_limits<TestType>::min());
    _ITOSS(std::numeric_limits<TestType>::min(), result.assign({}));
    REQUIRE(desired == result);

    _ITOS(std::numeric_limits<TestType>::min(), result.assign({}), 2);
    REQUIRE(desired == result);

    desired = to_binary(std::numeric_limits<TestType>::max());
    _ITOSS(std::numeric_limits<TestType>::max(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_binary(std::numeric_limits<TestType>::max());
    _ITOS(std::numeric_limits<TestType>::max(), result.assign({}), 2);
    REQUIRE(desired == result);

    for (TestType x = std::numeric_limits<TestType>::min(); x < (std::numeric_limits<TestType>::max()); ++x)
    {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);
    }

}


TEMPLATE_TEST_CASE("itos (base2) signed int(32|64) ascii", "[signed int][32|64][base2][ascii]", int32_t, int64_t)
{
    using namespace ascii;

    std::string desired, result;

    desired = to_binary(std::numeric_limits<TestType>::min());
    _ITOSS(std::numeric_limits<TestType>::min(), result.assign({}));
    REQUIRE(desired == result);

    _ITOS(std::numeric_limits<TestType>::min(), result.assign({}), 2);
    REQUIRE(desired == result);

    desired = to_binary(std::numeric_limits<TestType>::max());
    _ITOSS(std::numeric_limits<TestType>::max(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_binary(std::numeric_limits<TestType>::max());
    _ITOS(std::numeric_limits<TestType>::max(), result.assign({}), 2);
    REQUIRE(desired == result);


    for (TestType x = std::numeric_limits<TestType>::min(); x < (std::numeric_limits<TestType>::min() + TOLERANCE); ++x)
    {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);

    }

    for (TestType x = std::numeric_limits<TestType>::max(); x < (std::numeric_limits<TestType>::max() - TOLERANCE); ++x)
    {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);

    }

    for (TestType x = -TOLERANCE; x <= TOLERANCE; ++x) {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);
    }
}


TEMPLATE_TEST_CASE("itos (base2) unsigned int(32|64) ascii", "[unsigned int][32|64][base2][ascii]", uint32_t, uint64_t)
{
    using namespace ascii;

    std::string desired, result;

    desired = to_binary(std::numeric_limits<TestType>::min());
    _ITOSS(std::numeric_limits<TestType>::min(), result.assign({}));
    REQUIRE(desired == result);

    _ITOS(std::numeric_limits<TestType>::min(), result.assign({}), 2);
    REQUIRE(desired == result);

    desired = to_binary(std::numeric_limits<TestType>::max());
    _ITOSS(std::numeric_limits<TestType>::max(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_binary(std::numeric_limits<TestType>::max());
    _ITOS(std::numeric_limits<TestType>::max(), result.assign({}), 2);
    REQUIRE(desired == result);


    for (TestType x = std::numeric_limits<TestType>::min(); x < (std::numeric_limits<TestType>::min() + TOLERANCE); ++x)
    {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);

    }

    for (TestType x = std::numeric_limits<TestType>::max(); x < (std::numeric_limits<TestType>::max() - TOLERANCE); ++x)
    {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);
    }

    for (TestType x = std::numeric_limits<TestType>::min() + TOLERANCE; x <= TestType(4*TOLERANCE); ++x)
    {
        desired = to_binary(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 2);
        REQUIRE(desired == result);
    }
}



//// DECIMAL REPRESENTATION TESTS ////

#undef _ITOSS
#define _ITOSS stdx::itos<10>


TEMPLATE_TEST_CASE("itos (base10) unsigned|signed int(8|16) ascii", "[unsigned|signed int][8|16][base10][ascii]", int8_t, uint8_t, int16_t, uint16_t)
{
    using namespace ascii;

    std::string desired, result;

    desired = to_decimal(std::numeric_limits<TestType>::min());
    _ITOSS(std::numeric_limits<TestType>::min(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::min());
    _ITOS(std::numeric_limits<TestType>::min(), result.assign({}), 10);
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::max());
    _ITOSS(std::numeric_limits<TestType>::max(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::max());
    _ITOS(std::numeric_limits<TestType>::max(), result.assign({}), 10);
    REQUIRE(desired == result);

    for (TestType x = std::numeric_limits<TestType>::min(); x < (std::numeric_limits<TestType>::max()); ++x)
    {
        result.clear();

        desired = to_decimal(x);
        _ITOSS(x, result);
        REQUIRE(desired == result);

    }

}


TEMPLATE_TEST_CASE("itos (base10) signed int(32|64) ascii", "[signed int][32|64][base10][ascii]", int32_t, int64_t)
{
    using namespace ascii;

    std::string desired, result;

    desired = to_decimal(std::numeric_limits<TestType>::min());
    _ITOSS(std::numeric_limits<TestType>::min(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::min());
    _ITOS(std::numeric_limits<TestType>::min(), result.assign({}), 10);
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::max());
    _ITOSS(std::numeric_limits<TestType>::max(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::max());
    _ITOS(std::numeric_limits<TestType>::max(), result.assign({}), 10);
    REQUIRE(desired == result);

    for (TestType x = std::numeric_limits<TestType>::min(); x < (std::numeric_limits<TestType>::min() + TOLERANCE); ++x)
    {
        desired = to_decimal(x);

        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);
    }

    for (TestType x = std::numeric_limits<TestType>::max(); x < (std::numeric_limits<TestType>::max() - TOLERANCE); ++x)
    {
        desired = to_decimal(x);
        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);
    }

    for (TestType x = -TOLERANCE; x <= TOLERANCE; ++x) {
        desired = to_decimal(x);

        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);
    }
}


TEMPLATE_TEST_CASE("itos (base10) unsigned int(32|64) ascii", "[unsigned int][32|64][base10][ascii]", uint32_t, uint64_t)
{
    using namespace ascii;

    std::string desired, result;

    desired = to_decimal(std::numeric_limits<TestType>::min());
    _ITOSS(std::numeric_limits<TestType>::min(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::min());
    _ITOS(std::numeric_limits<TestType>::min(), result.assign({}), 10);
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::max());
    _ITOSS(std::numeric_limits<TestType>::max(), result.assign({}));
    REQUIRE(desired == result);

    desired = to_decimal(std::numeric_limits<TestType>::max());
    _ITOS(std::numeric_limits<TestType>::max(), result.assign({}), 10);
    REQUIRE(desired == result);

    for (TestType x = std::numeric_limits<TestType>::min(); x < (std::numeric_limits<TestType>::min() + TOLERANCE); ++x)
    {
        desired = to_decimal(x);

        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);

    }

    for (TestType x = std::numeric_limits<TestType>::max(); x < (std::numeric_limits<TestType>::max() - TOLERANCE); ++x)
    {
        desired = to_decimal(x);

        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);
    }

    for (TestType x = std::numeric_limits<TestType>::min() + TOLERANCE; x <= TestType(4*TOLERANCE); ++x)
    {
        desired = to_decimal(x);

        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);
    }
}


TEMPLATE_TEST_CASE("itos spec p10 (base10) signed int(32|64) ascii", "[spec][signed int][32|64][base10][ascii]", int32_t, int64_t)
{
    using namespace ascii;

    std::string desired, result;
    for (auto x : __powers_of_10<TestType>::values) {
        desired = to_decimal(x);

        _ITOSS(x, result.assign({}));
        REQUIRE(desired == result);

        _ITOS(x, result.assign({}), 10);
        REQUIRE(desired == result);
    }

    for (TestType i = 1; i < 12; i++)
    {
        for (auto p : __powers_of_10<TestType>::values)
        {
            auto x = p + i;

            desired = to_decimal(x);

            _ITOSS(x, result.assign({}));
            REQUIRE(desired == result);

            _ITOS(x, result.assign({}), 10);
            REQUIRE(desired == result);
        }
    }

}
#endif
