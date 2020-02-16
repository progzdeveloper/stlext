#include <string>
#include <vector>
#include <random>

#include <benchmark/benchmark.h>

#include <stlext/algorithm/sorting/counting_sort.hpp>



void BM_counting_sort_identity(benchmark::State& state)
{
    std::vector<int> v(state.range(0));
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 rgen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distr(-100, 100);

    std::generate(v.begin(), v.end(), [&rgen, &distr](){ return distr(rgen); });

    for (auto _ : state) {
        stdx::counting_sort(v.begin(), v.end());
    }

    bool success = std::is_sorted(v.begin(), v.end());
    state.SetLabel(success ? "[PASSED]" : "[FAILED]");
}
BENCHMARK(BM_counting_sort_identity)->RangeMultiplier(2)->Range(1024, 8<<10);


struct record
{
    int id;
    double value;

    record(): id(0), value(0) {}
    record(const std::ptrdiff_t _id) : id(_id), value(_id / 100.0) {}
    record(const record&) = default;
    record& operator=(const record&) = default;

    inline operator int() const {
        return id;
    }
};

namespace std {
    template<>
    struct is_integral<record> : true_type {};
}

void BM_counting_sort_proj(benchmark::State& state)
{
    std::vector<record> v(state.range(0));
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 rgen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distr(-100, 100);

    std::generate(v.begin(), v.end(), [&rgen, &distr](){
        record r;
        r.id = distr(rgen);
        r.value = r.id / 100.0;
        return r;
    });

    for (auto _ : state) {
        stdx::counting_sort(v.begin(), v.end()/*, [](const record& r) { return r.id; }*/);
    }

    bool success = std::is_sorted(v.begin(), v.end(), [](const record& r1, record& r2){
        return r1.id < r2.id;
    });
    state.SetLabel(success ? "[PASSED]" : "[FAILED]");
}
BENCHMARK(BM_counting_sort_proj)->RangeMultiplier(2)->Range(1024, 8<<10);
