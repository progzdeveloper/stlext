#include <vector>
#include <random>
#include <memory>
#include <iostream>
#include <benchmark/benchmark.h>

#include <stlext/bitvector/bitvector.hpp>

#define _MAX_BITS (1 << 10)


void BM_bitvector_copy(benchmark::State& state)
{
    std::cerr << "sizeof(bitvec): " << sizeof(stdx::bitvector<>) << std::endl;
    std::cerr << "MAX BITS:  " << _MAX_BITS << std::endl;


    std::mt19937_64 rnd;
    std::uniform_int_distribution<stdx::bitvector<>::word_type> distr;

    stdx::bitvector<> x(_MAX_BITS, 0);
    stdx::bitvector<> y(_MAX_BITS, 0);

    stdx::generate(x.begin(), x.end(), [&]() { return distr(rnd); } );
    stdx::generate(y.begin(), y.end(), [&]() { return distr(rnd); });

    for (auto _ : state) {
        state.PauseTiming();
        size_t first1 = rand() % (_MAX_BITS / 2);
        size_t first2 = first1 + 3;
        size_t last1 = (rand() % (_MAX_BITS / 2)) + first1;
        state.ResumeTiming();
        auto it = stdx::copy(x.begin() + first1, x.begin() + last1, y.begin() + first2);
        benchmark::DoNotOptimize(it);
    }
}
BENCHMARK(BM_bitvector_copy);



void BM_bitvector_transform(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS, 0);
    stdx::bitvector<> y(_MAX_BITS, 0);
    stdx::bitvector<> z(_MAX_BITS, 0);
    stdx::fill_n(x.begin(), (_MAX_BITS / 2) + 171, 1);
    stdx::fill(y.begin() + ((_MAX_BITS / 2) - 27), y.end(), 1);

    for (auto _ : state)
    {
        state.PauseTiming();
        size_t first = (rand() % ((_MAX_BITS / 2) - 1));
        size_t second = first + (_MAX_BITS / 2);
        state.ResumeTiming();
        auto it = stdx::transform(x.begin() + first, x.end(),
                                  y.begin() + second, z.begin() + first,
                                  std::bit_xor<>());
        benchmark::DoNotOptimize(it);
    }
}
BENCHMARK(BM_bitvector_transform);



void BM_bitvector_flip(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS, 0);
    stdx::bitvector<> y(_MAX_BITS, 0);

    stdx::fill_n(x.begin(), (_MAX_BITS / 2) + 171, 1);

    for (auto _ : state)
    {
        state.PauseTiming();
        size_t first = (rand() % (_MAX_BITS / 2));
        size_t second = (rand() % (_MAX_BITS / 2));
        state.ResumeTiming();
        stdx::flip_range(x.begin() + first, x.begin() + (first + (_MAX_BITS / 2 - 1)),
                         y.begin() + second);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
}
BENCHMARK(BM_bitvector_flip);



void BM_bitvector_shifts(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS, 0);
    std::mt19937_64 rnd;
    std::uniform_int_distribution<stdx::bitvector<>::word_type> distr;
    stdx::generate(x.begin(), x.end(), [&]() { return distr(rnd); });

    for (auto _ : state) {
         x >>= rand() % (_MAX_BITS / 2);
         x <<= rand() % (_MAX_BITS / 2);
         benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bitvector_shifts);



void BM_bitvector_rotate(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS/2, 0);

    for (auto _ : state) {
         auto it  = std::rotate(x.begin(), x.begin() + (rand() % (_MAX_BITS / 4) + (_MAX_BITS / 16)), x.end());
         benchmark::DoNotOptimize(it);
    }
}
BENCHMARK(BM_bitvector_rotate);


void BM_bitvector_next_perm_stlext(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS/16, 0);
    stdx::fill_n(x.begin(), _MAX_BITS/16-42, 1);

    for (auto _ : state) {
        size_t n = 0;
        for (; n < 100000;++n) {
            benchmark::DoNotOptimize(stdx::next_permutation(x.begin(), x.end()));
        }
    }
}
BENCHMARK(BM_bitvector_next_perm_stlext);

void BM_bitvector_prev_perm_stlext(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS/16, 0);
    stdx::fill_n(x.begin(), _MAX_BITS/16-42, 1);

    for (auto _ : state) {
        size_t n = 0;
        for (; n < 100000;++n) {
            benchmark::DoNotOptimize(stdx::prev_permutation(x.begin(), x.end()));
        }
    }
}
BENCHMARK(BM_bitvector_prev_perm_stlext);






/*

void BM_bitvector_next_perm_std(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS/16);
    stdx::fill_n(x.begin(), _MAX_BITS/16-42, 1);

    size_t n = 0;
    for (auto _ : state) {
        size_t n = 0;
        for (;n < 100000;++n) {
            benchmark::DoNotOptimize(std::next_permutation(x.begin(), x.end()));
        }
    }
}
BENCHMARK(BM_bitvector_next_perm_std);

void BM_bitvector_prev_perm_std(benchmark::State& state)
{
    stdx::bitvector<> x(_MAX_BITS/16);
    stdx::fill_n(x.begin(), _MAX_BITS/16-42, 1);

    size_t n = 0;
    for (auto _ : state) {
        size_t n = 0;
        for (;n < 100000;++n) {
            benchmark::DoNotOptimize(std::prev_permutation(x.begin(), x.end()));
        }
    }
}
BENCHMARK(BM_bitvector_prev_perm_std);

*/


void BM_bitvector_reverse_aligned(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<uint64_t> distr;

    stdx::bitvector<uint64_t> x(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        stdx::reverse(x.begin(), x.end());
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bitvector_reverse_aligned);

void BM_bitvector_reverse_unaligned(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<uint64_t> distr;

    stdx::bitvector<uint64_t> x(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        stdx::reverse(x.begin() + 3, x.end() - 3);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bitvector_reverse_unaligned);



void BM_bitvector_reverse_aligned_std(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<uint64_t> distr;

    stdx::bitvector<uint64_t> x(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        std::reverse(x.begin(), x.end());
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bitvector_reverse_aligned_std);

void BM_bitvector_reverse_unaligned_std(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<uint64_t> distr;

    stdx::bitvector<uint64_t> x(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        std::reverse(x.begin() + 3, x.end() - 3);
        benchmark::DoNotOptimize(x);
    }
}
BENCHMARK(BM_bitvector_reverse_unaligned_std);




void BM_bitvector_equal_unaligned(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<stdx::bitvector<>::word_type> distr;

    stdx::bitvector<> x(_MAX_BITS, 0);
    stdx::bitvector<> y(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });
    std::generate(y.begin(), y.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        state.PauseTiming();
        size_t length = (rand() % (_MAX_BITS / 2)) - 1;
        size_t first1 = rand() % (_MAX_BITS / 2);
        size_t first2 = (rand() % (_MAX_BITS / 2)) + first1;
        size_t last1 = first1 + length;
        size_t last2 = first2 + length;
        state.ResumeTiming();
        bool result = stdx::equal(x.begin() + first1, x.begin() + last1,
                                  y.begin() + first2, y.begin() + last2);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_bitvector_equal_unaligned);



void BM_bitvector_equal_aligned(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<stdx::bitvector<>::word_type> distr;

    stdx::bitvector<> x(_MAX_BITS, 0);
    stdx::bitvector<> y(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });
    std::generate(y.begin(), y.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        state.PauseTiming();
        size_t length = 1ULL << (1 + rand() % 9);
        size_t first1 = rand() % (_MAX_BITS / 2);
        size_t first2 = first1;
        size_t last1 = first1 + length;
        size_t last2 = first2 + length;
        state.ResumeTiming();
        bool result = stdx::equal(x.begin() + first1, x.begin() + last1,
                                  y.begin() + first2, y.begin() + last2);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_bitvector_equal_aligned);


void BM_bitvector_equal_single(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<stdx::bitvector<>::word_type> distr;

    stdx::bitvector<> x(_MAX_BITS, 0);
    stdx::bitvector<> y(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });
    std::generate(y.begin(), y.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        state.PauseTiming();
        size_t first1 = rand() % (_MAX_BITS / 2);
        size_t first2 = (rand() % (_MAX_BITS / 2)) + first1;
        size_t last1 = first1 + 1;
        size_t last2 = first2 + 1;
        state.ResumeTiming();
        bool result = stdx::equal(x.begin() + first1, x.begin() + last1,
                                  y.begin() + first2, y.begin() + last2);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_bitvector_equal_single);



void BM_bitvector_equal_inplace(benchmark::State& state)
{
    std::mt19937_64 rnd;
    std::uniform_int_distribution<stdx::bitvector<>::word_type> distr;

    stdx::bitvector<> x(_MAX_BITS, 0);

    std::generate(x.begin(), x.end(), [&]() { return distr(rnd); });

    for (auto _ : state)
    {
        state.PauseTiming();
        size_t first1 = rand() % (_MAX_BITS / 2);
        size_t last1 = first1 + rand() % (_MAX_BITS / 2);
        state.ResumeTiming();
        bool result = stdx::equal(x.begin() + first1, x.begin() + last1,
                                  x.begin() + first1, x.begin() + last1);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_bitvector_equal_inplace);

