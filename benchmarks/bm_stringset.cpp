#include <random>
#include <algorithm>
#include <vector>
#include <string>

#include <unordered_set>
#include <stlext/containers/stringset.hpp>

#include <benchmark/benchmark.h>

#define _KEY_SIZE 14


size_t make_array(std::vector<std::string>& elems)
{
    size_t k = 0;
    for (size_t i = 0; i < elems.size(); i++) {
        size_t n = (rand() % _KEY_SIZE + 2);
        for (size_t j = 0; j < n; j++, k++) {
            elems[i].push_back(char(32 + (rand()%94)));
        }
    }
    return k;
}

template<class _USet>
size_t __bench_uset_insert(benchmark::State& state)
{
    using namespace std;

    size_t n = state.range(0);
    vector<string> elems(n);
    size_t s = make_array(elems);

    _USet mapping;
    //mapping.reserve(n);
    for (auto _ : state) {
        for (size_t k = 0; k < n; k++)
            benchmark::DoNotOptimize(mapping.insert(elems[k]));
        state.PauseTiming();
        mapping.clear();
        state.ResumeTiming();
    }
    return s;
}



template<class _USet>
size_t __bench_uset_erase(benchmark::State& state)
{
    using namespace std;

    size_t n = state.range(0);
    vector<string> elems(n);
    size_t s = make_array(elems);

    _USet mapping;
    mapping.reserve(n);
    for (size_t k = 0; k < n; k++)
        mapping.insert(elems[k]);

    for (auto _ : state) {
        for (size_t k = 0; k < n; k++)
            benchmark::DoNotOptimize(mapping.erase(elems[k]));
    }
    return s;
}


template<class _USet>
size_t __bench_uset_insert_clear(benchmark::State& state)
{
    using namespace std;

    size_t n = state.range(0);
    vector<string> elems(n);
    size_t s = make_array(elems);

    _USet mapping;
    //mapping.reserve(n);
    for (auto _ : state)
    {
        for (size_t k = 0; k < n; k++) {
            benchmark::DoNotOptimize(mapping.insert(elems[k]));
        }
        mapping.clear();
        benchmark::ClobberMemory();
    }
    return s;
}


template<class _USet>
size_t __bench_uset_traverse(benchmark::State& state)
{
    using namespace std;

    size_t n = state.range(0);
    vector<string> elems(n);
    size_t s = make_array(elems);

    _USet mapping;
    mapping.reserve(n);
    for (auto it = elems.begin(); it != elems.end(); ++it) {
        mapping.insert(*it);
    }

    string str;
    str.reserve(n * 4);
    for (auto _ : state) {
        for (auto it = mapping.begin(); it != mapping.end(); ++it) {
            str.append(it->begin(), it->end());
            benchmark::DoNotOptimize(str);
        }
        //benchmark::ClobberMemory();
        state.PauseTiming();
        str.clear();
        state.ResumeTiming();
    }
    return s;
}




void BM_unordered_set_insert(benchmark::State& state)
{
    size_t n = __bench_uset_insert< std::unordered_set<std::string> >(state);
    state.SetItemsProcessed(state.range(0));
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_unordered_set_insert)->RangeMultiplier(2)->Range(8, 1 << 20);

void BM_string_set_insert(benchmark::State& state)
{
    size_t n = __bench_uset_insert< stdx::stringset >(state);
    state.SetItemsProcessed(state.range(0));
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_string_set_insert)->RangeMultiplier(2)->Range(8, 1 << 20);



void BM_unordered_set_insert_clear(benchmark::State& state)
{
    size_t n = __bench_uset_insert_clear< std::unordered_set<std::string> >(state);
    state.SetItemsProcessed(state.range(0));
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_unordered_set_insert_clear)->RangeMultiplier(2)->Range(8, 1 << 20);

void BM_string_set_insert_clear(benchmark::State& state)
{
    size_t n = __bench_uset_insert_clear< stdx::stringset >(state);
    state.SetItemsProcessed(state.range(0));
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_string_set_insert_clear)->RangeMultiplier(2)->Range(8, 1 << 20);



void BM_unordered_set_traverse(benchmark::State& state)
{
    size_t n = __bench_uset_traverse< std::unordered_set<std::string> >(state);
    state.SetItemsProcessed(state.range(0));
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_unordered_set_traverse)->RangeMultiplier(2)->Range(8, 1 << 20);

void BM_string_set_traverse(benchmark::State& state)
{
    size_t n = __bench_uset_traverse< stdx::stringset >(state);
    state.SetItemsProcessed(state.range(0));
    state.SetBytesProcessed(state.iterations() * n);
}
BENCHMARK(BM_string_set_traverse)->RangeMultiplier(2)->Range(8, 1 << 20);


