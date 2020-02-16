TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
TARGET = stlext
DESTDIR = bin
DEPENDPATH += ../

QMAKE_CXXFLAGS += -O3 -m64 -msse4.2 -mtune=corei7


HEADERS += \
    algorithm/algorithm.h \
    algorithm/experimental.h \
    algorithm/ext/gather.hpp \
    algorithm/ext/is_unique.hpp \
    algorithm/ext/join.hpp \
    algorithm/ext/kadane.hpp \
    algorithm/ext/majority_element.hpp \
    algorithm/ext/regex_split.hpp \
    algorithm/ext/slide.hpp \
    algorithm/ext/split.hpp \
    algorithm/searching/exponential_search.hpp \
    algorithm/searching.h \
    algorithm/searching/interpolation_search.hpp \
    algorithm/sorting/bucket_sort.hpp \
    algorithm/sorting/counting_sort.hpp \
    algorithm/sorting.h \
    algorithm/sorting/insertion_sort.hpp \
    algorithm/sorting/selection_sort.hpp \
    allocators/aligned_allocator.hpp \
    allocators/allocators.hpp \
    allocators/arena_traits.hpp \
    allocators/basic_allocator.hpp \
    allocators/basic_arena.hpp \
    allocators/binexp_arena.hpp \
    allocators/bitmap_arena.hpp \
    allocators/fallback_arena.hpp \
    allocators/freelist.hpp \
    allocators/heap_arena.hpp \
    allocators/intrusive_ptr.hpp \
    allocators/memory_storage.hpp \
    allocators/ordered_arena.hpp \
    allocators/pooled_object.hpp \
    bfc/basic_bloom_filter.h \
    bfc/bfcfwd.h \
    bfc/bloom_filter.h \
    bfc/compressed_bloom_filter.h \
    bfc/counting_bloom_filter.h \
    bfc/scalable_bloom_filter.h \
    bitvector/bit_iterator.hpp \
    bitvector/bit_storage.hpp \
    bitvector/bit_traits.hpp \
    bitvector/bitvector.hpp \
    compability/cxx11/all_of.hpp \
    compability/cxx11/any_of.hpp \
    compability/cxx11/copy_if.hpp \
    compability/cxx11/copy_n.hpp \
    compability/cxx11/find_if_not.hpp \
    compability/cxx11/iota.hpp \
    compability/cxx11/is_partitioned.hpp \
    compability/cxx11/is_permutation.hpp \
    compability/cxx11/is_sorted.hpp \
    compability/cxx11/none_of.hpp \
    compability/cxx11/one_of.hpp \
    compability/cxx11/partition_copy.hpp \
    compability/cxx11/partition_point.hpp \
    compability/cxx11/sample.hpp \
    compability/cxx14/equal.hpp \
    compability/cxx14/integer_sequence.hpp \
    compability/cxx14/mismatch.hpp \
    compability/cxx17/basic_string_view.hpp \
    compability/cxx17/for_each.hpp \
    compability/cxx17/invoke.hpp \
    compability/cxx17/utility.hpp \
    compability/cxx20/identity.hpp \
    compability/cxx20/span.hpp \
    compact/storages.hpp \
    compact/string.hpp \
    compact/vector.hpp \
    components/binary_scanner.hpp \
    components/class_factory.hpp \
    components/crtp.hpp \
    components/interpret_traits.hpp \
    components/invoker.hpp \
    components/mixin.h \
    components/reflect.hpp \
    components/stream_scanner.hpp \
    containers/circular_queue.hpp \
    containers/lru_cache.hpp \
    containers/packed_hashtbl.hpp \
    containers/packed_lru_cache.hpp \
    containers/priority_map.hpp \
    containers/span.hpp \
    containers/stringset.hpp \
    cui/noecho.hpp \
    cui/progress_viewer.hpp \
    cui/progress_watcher.hpp \
    cui/prompt.hpp \
    functional/benchmark.hpp \
    functional/bit_andnot.hpp \
    functional/function_traits.hpp \
    functional/identity.hpp \
    functional/invoker.hpp \
    functional/memorized.hpp \
    functional/overload.hpp \
    functional/predicates.hpp \
    iostreams/base16.hpp \
    iostreams/base64.hpp \
    iostreams/bin.hpp \
    iostreams/byte_view.hpp \
    iostreams/codecs.hpp \
    iostreams/format.hpp \
    iostreams/getline.hpp \
    iostreams/ostream_joiner.hpp \
    iostreams/ratio.hpp \
    iostreams/timestamp.hpp \
    iostreams/utility.hpp \
    iterator/any_iterator.hpp \
    iterator/circular_iterator.hpp \
    iterator/istring_iterator.hpp \
    iterator/numeric_iterator.hpp \
    iterator/uinserter.hpp \
    mathext/constants.hpp \
    mathext/fuzzy_compare.hpp \
    mathext/mathext.hpp \
    mathext/static_log2.hpp \
    mathext/static_popcnt.hpp \
    numeric/cartesian_product.hpp \
    numeric/collector.hpp \
    numeric/features.hpp \
    numeric/identity_element.hpp \
    numeric/metrics.hpp \
    numeric/power.hpp \
    options/command_line.hpp \
    options/config_file.hpp \
    options/enviroment_map.hpp \
    options/option_source.hpp \
    platform/arch.h \
    platform/bits.h \
    platform/common.h \
    platform/compiler.h \
    platform/error.h \
    platform/platform.h \
    platform/version.h \
    range/iter_range.hpp \
    range/range_algo.hpp \
    range/value_range.h \
    stltraits/stdxfwd.hpp \
    stltraits/stlfwd.hpp \
    stltraits/stltraits.hpp \
    threads/latch.hpp \
    threads/thread_set.hpp \
    tuple/tuple_algo.hpp \
    tuple/tuple_apply.hpp \
    tuple/tuple_foreach.hpp \
    tuple/tupleio.hpp \
    tuple/tuple_transform.hpp \
    tuple/tuple_utils.hpp



