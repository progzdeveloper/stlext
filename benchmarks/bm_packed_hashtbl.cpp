#include <random>
#include <algorithm>
#include <array>
#include <string>

#include <unordered_map>
#include <stlext/containers/packed_hashtbl.hpp>

#include <stlext/containers/lru_cache.hpp>
//#include <stlext/containers/packed_lru_cache.h>

#include <benchmark/benchmark.h>

#define _MAX_UMAP_ELEMS 1024
#define _KEY_MULT 1000000
#define _VAL_MULT 100

template<size_t N>
void make_array(std::array<std::pair<std::string, int>, N>& elems)
{
    for (int i = 0; i < (int)N; i++) {
        elems[i] = std::make_pair(std::to_string((i+1)*_KEY_MULT), i * _VAL_MULT);
    }
}

template<class _UMap, size_t _Size>
void __bench_umap_insert(benchmark::State& s)
{
    using namespace std;
    _UMap mapping;

    array<pair<string, int>, _Size> elems;
    make_array(elems);

    for (auto _ : s) {
        for (size_t k = 0; k < _Size; k++)
            benchmark::DoNotOptimize(mapping.insert(elems[k]));
        s.PauseTiming();
        mapping.clear();
        s.ResumeTiming();
    }
}


void BM_unordered_map_insert(benchmark::State& state)
{
    __bench_umap_insert< std::unordered_map<std::string, int>, _MAX_UMAP_ELEMS>(state);
}
BENCHMARK(BM_unordered_map_insert);


void BM_packed_hashmap_insert(benchmark::State& state)
{
    __bench_umap_insert< stdx::packed_hashmap<std::string, int, _MAX_UMAP_ELEMS+1>, _MAX_UMAP_ELEMS >(state);
}
BENCHMARK(BM_packed_hashmap_insert);




template<class _UMap, size_t _Size>
void __bench_umap_erase(benchmark::State& s)
{
    using namespace std;

    _UMap mapping;
    //mapping.reserve(_Size);

    array<pair<string, int>, _Size> elems;
    make_array(elems);

    for (size_t k = 0; k < _Size; k++)
        mapping.insert(elems[k]);

    for (auto _ : s) {
        for (size_t k = 0; k < _Size; k++)
            benchmark::DoNotOptimize(mapping.erase(elems[k].first));
    }
}

void BM_unordered_map_erase(benchmark::State& state)
{
    __bench_umap_erase< std::unordered_map<std::string, int>, _MAX_UMAP_ELEMS>(state);
}
BENCHMARK(BM_unordered_map_erase);


void BM_packed_hashmap_erase(benchmark::State& state)
{
    __bench_umap_erase< stdx::packed_hashmap<std::string, int, _MAX_UMAP_ELEMS+1>, _MAX_UMAP_ELEMS >(state);
}
BENCHMARK(BM_packed_hashmap_erase);




template<class _UMap, size_t _Size>
void __bench_umap_insert_erase(benchmark::State& s)
{
    using namespace std;

    _UMap mapping;
    //mapping.reserve(_Size);

    array<pair<string, int>, _Size> elems;
    make_array(elems);

    array<string, _Size> victims;


    transform(elems.begin(), elems.end(), victims.begin(),
                   [](const pair<string, int>& p) { return p.first; });

    random_device rd;
    mt19937 g(rd());
    shuffle(victims.begin(), victims.end(), g);


    for (auto _ : s)
    {
        for (size_t k = 0; k < _Size; k++) {
            benchmark::DoNotOptimize(mapping.insert(elems[k]));
        }
        for (size_t k = 0; k < _Size; k++) {
            benchmark::DoNotOptimize(mapping.erase(victims[k]));
        }

        s.PauseTiming();
        shuffle(victims.begin(), victims.end(), g);
        shuffle(elems.begin(), elems.end(), g);
        s.ResumeTiming();
    }
}

void BM_unordered_map_insert_erase(benchmark::State& state)
{
    __bench_umap_insert_erase< std::unordered_map<std::string, int>, _MAX_UMAP_ELEMS>(state);
}
BENCHMARK(BM_unordered_map_insert_erase);


void BM_packed_hashmap_insert_erase(benchmark::State& state)
{
    __bench_umap_insert_erase< stdx::packed_hashmap<std::string, int, _MAX_UMAP_ELEMS+1>, _MAX_UMAP_ELEMS >(state);
}
BENCHMARK(BM_packed_hashmap_insert_erase);


template<size_t _Count, class _LruCache>
void __bench_lrucache_insert(_LruCache& cache, benchmark::State& state)
{
    using namespace std;
    for (auto _ : state) {
        for (size_t i = 0; i < _Count; i++) {
            benchmark::DoNotOptimize(cache.insert(i));
        }
        state.PauseTiming();
        cache.clear();
        state.ResumeTiming();
    }
}

void BM_basic_lru_cache_insert(benchmark::State& state)
{
    stdx::lru_cacheset<size_t> cache(256);
    __bench_lrucache_insert<_MAX_UMAP_ELEMS>(cache, state);
}
BENCHMARK(BM_basic_lru_cache_insert);

/*void BM_packed_lru_cache_insert(benchmark::State& state)
{
    stdx::fixed_lru_cache<size_t, size_t> cache(256);
    __bench_lrucache_insert<1024>(cache, state);
}*/
