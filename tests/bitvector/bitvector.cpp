#include <iostream>
#include <sstream>
#include <catch.hpp>

#include <stlext/bitvector/bitvector.hpp>

namespace detail 
{
	template<class _Word, class _Alloc>
	std::string to_string(const stdx::bitvector<_Word, _Alloc>& bits) {
		std::stringstream oss;
		oss << bits;
		return oss.str();
	}

	template<class _Word, bool C>
	std::string to_string(stdx::bit_iterator<_Word, C> first, stdx::bit_iterator<_Word, C> last) {
		std::stringstream oss;
		std::copy(std::make_reverse_iterator(last),
				  std::make_reverse_iterator(first),
				  std::ostream_iterator<bool>(oss));
		return oss.str();
	}
}

TEST_CASE("bitvector/constructor", "[bitvector]")
{
	using namespace stdx;
	using ::detail::to_string;

	typedef bitvector<> bitvec;
	std::bitset<65> _bits("11010101010101010101111111111000000011101000111110011111010101111");
	bitvec bits(_bits);

	bitvec bits0;
	bitvec bits1(64);
	bitvec bits2(128);
	bitvec bits3 = bits;
	bitvec bits4 = _bits;

	REQUIRE(_bits.to_string() == to_string(bits));
	REQUIRE(_bits.to_string() == to_string(bits3));
	REQUIRE(_bits.to_string() == to_string(bits4));
	REQUIRE(bits0.empty());
	REQUIRE(bits1.size() == 64);
	REQUIRE(bits2.blocks() == 2);
}


TEST_CASE("bitvector/memory", "[bitvector]")
{
	using ::detail::to_string;

	typedef stdx::bitvector<uint64_t> bitvector_t;
	static const size_t min_capacity = (sizeof(bitvector_t) * CHAR_BIT - CHAR_BIT);

	std::cout << "bitvector_t size: " << sizeof(bitvector_t) << " bytes" << std::endl;
	

	std::bitset<96> _bits("011010010000001101100010100000000010011111100101110110110111010101100100100101011111110000010011");
	bitvector_t bits(_bits), result(18, 0);

	stdx::flip(bits.begin() + 7, bits.begin() + 25, result.begin());
	REQUIRE("101101010000000111" == to_string(result));

	result.resize(bits.size());
	REQUIRE("000000000000000000000000000000000000000000000000000000000000000000000000000000101101010000000111" == to_string(result));

	bitvector_t s1((1 << 9) - 3, true);
	REQUIRE("11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
				   == to_string(s1));

	s1.resize(181, 1);
	REQUIRE("1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
				   == to_string(s1));

	s1.resize(72, 1);
	REQUIRE("111111111111111111111111111111111111111111111111111111111111111111111111" == to_string(s1));
	REQUIRE(s1.capacity() > min_capacity);

	s1.shrink();
#ifdef STDX_PROCESSOR_X86_64
	REQUIRE(s1.capacity() == min_capacity);
#else
	REQUIRE(s1.capacity() == 2 * bitvector_t::bpw);
#endif

	s1.resize(24, 1);
	REQUIRE("111111111111111111111111" == to_string(s1));
	s1.shrink();
#ifdef STDX_PROCESSOR_X86_64
	REQUIRE(s1.capacity() == min_capacity);
#else
	REQUIRE(s1.capacity() == bitvector_t::bpw);
#endif

	s1.resize(68, 1);
	REQUIRE("11111111111111111111111111111111111111111111111111111111111111111111" == to_string(s1));
#ifdef STDX_PROCESSOR_X86_64
	REQUIRE(s1.capacity() == min_capacity);
#else
	REQUIRE(s1.capacity() == 2 * bitvector_t::bpw);
#endif

	s1.resize(254, 0);
	REQUIRE("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111111111111111111111111111111"
				   == to_string(s1));
	REQUIRE(s1.capacity() > min_capacity);

	s1.shrink();
	REQUIRE(s1.capacity() == 256);

	s1.resize(190, 0);
	s1.shrink();
	REQUIRE("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111111111111111111111111111111111111111111111"
				   == to_string(s1));
	REQUIRE(s1.capacity() == 192);

	bitvector_t s2(72, true);
	REQUIRE("111111111111111111111111111111111111111111111111111111111111111111111111" == to_string(s2));
#ifdef STDX_PROCESSOR_X86_64
	REQUIRE(s2.capacity() == min_capacity);
#else
	REQUIRE(s1.capacity() == 2 * bitvector_t::bpw);
#endif

	s2.reserve(256);
	std::string t = to_string(s2);
	REQUIRE("111111111111111111111111111111111111111111111111111111111111111111111111" == t);
	REQUIRE(s2.capacity() > min_capacity);

	s2.resize(230, 0);
	REQUIRE("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111111111111111111111111111111111111111111111111111111111111111111"
				   == to_string(s2));

	s1.resize(24, 1);
	s1.shrink();
#ifdef STDX_PROCESSOR_X86_64
	REQUIRE(s1.capacity() == min_capacity);
#else
	REQUIRE(s1.capacity() == 2 * bitvector_t::bpw);
#endif

	bitvector_t s3 = s1;
	REQUIRE(s3.size() == s1.size());

	s3 = s2;
	REQUIRE(s3.size() == s2.size());

	s2 = s1;
	REQUIRE(s2.size() == s1.size());

	s2.resize(0);
	s2.shrink();
	REQUIRE(s2.size() == 0);
	REQUIRE(s2.empty());

}





TEST_CASE("bitvector/iterator", "[bitvector]")
{
	using namespace stdx;
	typedef bitvector<> bitvec;
	std::bitset<65> _bits("11010101010101010101111111111000000011101000111110011111010101111");
	bitvec bits(_bits);
	size_t n = 0;
	for (auto it = bits.begin(); it != bits.end(); ++it) {
		++n;
	}
	REQUIRE(std::distance(bits.begin(), bits.end()) == 65);
	REQUIRE(n == 65);
}




TEST_CASE("bitvector/flip", "[bitvector]")
{
	using ::detail::to_string;

	typedef stdx::bitvector<> bitvec;
	std::string s;
	std::bitset<256> _bits("1101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111");
	bitvec bits(_bits), result(256, 0);

	stdx::flip(bits.begin(), bits.begin(), bits.begin());
	stdx::flip(bits.begin(), bits.begin(), result.begin());

	stdx::flip(bits.begin() + 7, bits.begin() + 25, result.begin());
	s = to_string(result.begin(), result.begin() + 18);
	REQUIRE("101110000011000001" == s);

	stdx::flip(bits.begin(), bits.begin() + 10, result.begin());
	s = to_string(result.begin(), result.begin() + 10);
	REQUIRE("0010101000" == s);

	stdx::flip(bits.begin(), bits.end(), result.begin());
	s = to_string(result);
	REQUIRE("0010101010101010101000000000011111110001011100000110000010101000001010101010101010100000000001111111000101110000011000001010100000101010101010101010000000000111111100010111000001100000101010000010101010101010101000000000011111110001011100000110000010101000" == s);

	stdx::flip(result.begin(), result.end(), result.begin());
	s = to_string(result);
	REQUIRE("1101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111" == s);

	stdx::flip(result.begin(), result.begin() + 10, result.begin());
	s = to_string(result.begin(), result.begin() + 10);
	REQUIRE("0010101000" == s);

	stdx::flip(bits.begin() + 5, bits.end() - 5, result.begin());
	s = to_string(result.begin(), result.end() - 10);
	REQUIRE("010101010101010000000000111111100010111000001100000101010000010101010101010101000000000011111110001011100000110000010101000001010101010101010100000000001111111000101110000011000001010100000101010101010101010000000000111111100010111000001100000101" == s);
}

TEST_CASE("bitvector/equal_range", "[bitvector]")
{
    typedef stdx::bitvector<> bitvec;
    std::bitset<65> _bits("00000001110100011111001111101010111100000000000000000001111111111");
    bitvec bits(_bits);
    auto p1 = stdx::equal_range(bits.begin(), bits.end(), 1);
    REQUIRE(p1.second - p1.first == 10);
    auto p2 = stdx::equal_range(bits.begin(), bits.end(), 0);
    REQUIRE(p2.second - p2.first == 19);
}

TEST_CASE("bitvector/search_n", "[bitvector]")
{
    typedef stdx::bitvector<> bitvec;
    std::bitset<65> _bits1("00000001110100011111001111101010111100000000000000000001111111111");
    bitvec bits1(_bits1);
    auto p1 = stdx::search_n(bits1.begin(), bits1.end(), 4, 1);
    REQUIRE(p1 == bits1.begin());
    auto p2 = stdx::search_n(bits1.begin(), bits1.end(), 15, 0);
    REQUIRE(std::distance(bits1.begin(), p2) == 10);

    std::bitset<76> _bits2("1111111111000000000000000000000000000000000000000000000000000000000000000001");
    bitvec bits2(_bits2);
    auto p3 = stdx::search_n(bits2.begin(), bits2.end(), 10, 1);
    REQUIRE(std::distance(bits2.begin(), p3) == 66);

    std::bitset<76> _bits3("0000000000111111111111111111111111111111111111111111111111111111111111111110");
    bitvec bits3(_bits3);
    auto p4 = stdx::search_n(bits3.begin(), bits3.end(), 10, 0);
    REQUIRE(std::distance(bits3.begin(), p4) == 66);

    std::bitset<76> _bits4("0000000000000000000000000000000000000000000000000000000000000000000000000001");
    bitvec bits4(_bits4);
    auto p5 = stdx::search_n(bits4.begin(), bits4.end(), 3, 1);
    REQUIRE(std::distance(bits4.end(), p5) == 0);

    std::bitset<76> _bits5("1111111111111111111111111111111111111111111111111111111111111111111111111111");
    bitvec bits5(_bits5);
    auto p6 = stdx::search_n(bits5.begin(), bits5.end(), 3, 0);
    REQUIRE(std::distance(bits5.end(), p6) == 0);
}

TEST_CASE("bitvector/compare", "[bitvector]")
{
	typedef stdx::bitvector<> bitvec;

	std::bitset<64>  _a64("1101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<64>  _b64("1101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<118> _a118("1101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111");
	std::bitset<118> _b118("1101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111");
	std::bitset<128> _a128("11010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<128> _b128("11010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<512> _a512("11010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<512> _b512("11010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<512> _c512("01110101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111");
	std::bitset<512> _d512("00000010101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010101111101010101010101010111111111100000001110100011111001111101010111110101010101010101011111111110000000111010001111100111110101011111010101010101010101111111111000000011101000111110011111010");

	bitvec a64(_a64), b64(_b64);
	bitvec a118(_a118), b118(_b118);
	bitvec a128(_a128), b128(_b128);
	bitvec a512(_a512), b512(_b512), c512(_c512), d512(_d512);

	// aligned
	REQUIRE(stdx::equal(a64.begin(), a64.begin(), a64.begin(), a64.begin())); // same pointers zero size
	REQUIRE(stdx::equal(a64.begin(), a64.end(), a64.begin(), a64.end())); // same pointers non zero size
	REQUIRE(stdx::equal(a64.begin() + 32, a64.begin() + 33, b64.begin() + 32, b64.begin() + 33)); // single bit
	REQUIRE(stdx::equal(a64.begin() + 35, a64.begin() + 36, b64.begin() + 35, b64.begin() + 36)); // single bit
	REQUIRE(stdx::equal(a64.begin() + 3, a64.end() - 1, a64.begin() + 3, a64.end() - 1)); // same pointers
	REQUIRE(stdx::equal(a64.begin(), a64.end(), b64.begin(), b64.end())); // same content - aligned single word
	REQUIRE(stdx::equal(a64.begin() + 5, a64.end() - 10, b64.begin() + 5, b64.end() - 10)); // same content - aligned bit range
	REQUIRE(stdx::equal(a64.begin() + 5, a64.end() - 10, b512.begin() + 5, b512.begin() + 54)); // same content - different iters

	REQUIRE(stdx::equal(a128.begin(), a128.begin(), a128.begin(), a128.begin())); // same pointers
	REQUIRE(stdx::equal(a128.begin(), a128.end(), a128.begin(), a128.end())); // zero size
	REQUIRE(stdx::equal(a128.begin() + 64, a128.begin() + 65, b128.begin() + 64, b128.begin() + 65)); // single bit - true
	REQUIRE_FALSE(stdx::equal(a128.begin() + 68, a128.begin() + 69, b128.begin() + 3, b128.begin() + 4)); // single bit - false
	REQUIRE(stdx::equal(a128.begin() + 3, a128.end() - 1, a128.begin() + 3, a128.end() - 1)); // same pointers
	REQUIRE(stdx::equal(a128.begin(), a128.end(), b128.begin(), b128.end()));  // same content
	REQUIRE(stdx::equal(a128.begin() + 65, a128.end() - 16, b128.begin() + 65, b128.end() - 16)); // same content - aligned bit range

	REQUIRE(stdx::equal(a118.begin(), a118.begin(), a118.begin(), a118.begin())); // same pointers
	REQUIRE(stdx::equal(a118.begin(), a118.end(), a118.begin(), a118.end())); // zero size
	REQUIRE(stdx::equal(a118.begin() + 64, a118.begin() + 65, b118.begin() + 64, b118.begin() + 65)); // single bit
	REQUIRE(stdx::equal(a118.begin() + 68, a118.begin() + 69, b118.begin() + 68, b118.begin() + 69)); // single bit
	REQUIRE(stdx::equal(a118.begin() + 3, a118.end() - 1, a118.begin() + 3, a118.end() - 1)); // same pointers
	REQUIRE(stdx::equal(a118.begin(), a118.end(), b118.begin(), b118.end()));  // same content
	REQUIRE(stdx::equal(a118.begin() + 65, a118.end() - 16, b118.begin() + 65, b118.end() - 16)); // same content - aligned bit range

	REQUIRE(stdx::equal(a512.begin(), a512.begin(), a512.begin(), a512.begin())); // same pointers zero size
	REQUIRE(stdx::equal(a512.begin(), a512.end(), a512.begin(), a512.end())); // same pointers non zero size
	REQUIRE(stdx::equal(a512.begin(), a512.end(), b512.begin(), b512.end()));  // same content
	REQUIRE(stdx::equal(a512.begin() + 3, a512.end() - 2, b512.begin() + 3, b512.end() - 2)); // same content - unaligned first/last
	REQUIRE(stdx::equal(a512.begin(), a512.end() - 11, b512.begin(), b512.end() - 11));  // same content - unaligned last
	REQUIRE(stdx::equal(a512.begin() + 23, a512.end(), b512.begin() + 23, b512.end()));  // same content - unaligned first

	REQUIRE_FALSE(stdx::equal(a64.begin(), a64.end() - 2, a64.begin() + 1, a64.end())); // size differ
	REQUIRE_FALSE(stdx::equal(a64.begin() + 7, a64.end(), a64.begin() + 5, a64.end() - 2)); // heuristic test
	REQUIRE_FALSE(stdx::equal(a512.begin(), a512.end(), c512.begin(), c512.end()));
	REQUIRE(stdx::equal(a512.begin(), a512.end() - bitvec::bpw, c512.begin(), c512.end() - bitvec::bpw));
	REQUIRE_FALSE(stdx::equal(c512.begin() + 2, c512.end(), d512.begin() + 2, d512.end()));
	REQUIRE_FALSE(stdx::equal(c512.begin() + 2, c512.end(), d512.begin() + 2, d512.end()));
	REQUIRE_FALSE(stdx::equal(b512.begin() + 5, b512.end(), c512.begin() + 5, c512.end()));
	REQUIRE_FALSE(stdx::equal(b512.begin() + 5, b512.end() - 1, c512.begin() + 5, c512.end() - 1));
	// unaligned 
	REQUIRE_FALSE(stdx::equal(a64.begin(), a64.end() - 1, a64.begin() + 1, a64.end()));  // same pointers different content
	REQUIRE_FALSE(stdx::equal(a128.begin(), a128.begin() + 64, a128.begin() + 1, a64.begin() + 65));  // same pointers different content
	REQUIRE_FALSE(stdx::equal(a128.begin(), a128.end() - 1, a128.begin() + 1, a128.end())); // same pointers different content
	REQUIRE_FALSE(stdx::equal(a118.begin(), a118.end() - 1, a118.begin() + 1, a118.end())); // same pointers different content
	REQUIRE_FALSE(stdx::equal(a512.begin(), a512.end() - 1, a512.begin() + 1, a512.end())); // same pointers different content
	REQUIRE_FALSE(stdx::equal(a512.begin() + 5, a512.end(), d512.begin(), d512.end() - 5));
	REQUIRE(stdx::equal(a512.begin() + 5, a512.end() - 251, d512.begin(), d512.end() - 256));

	REQUIRE(b64 == a64);
	REQUIRE(b64 == b64);
	REQUIRE(b128 == a128);
	REQUIRE(a512 == b512);

	REQUIRE_FALSE(b64 == a128);
	REQUIRE_FALSE(b64 == a512);
	REQUIRE_FALSE(a512 == c512);
}





TEST_CASE("bitvector/algorithms", "[bitvector]")
{
	using namespace std;
	using ::detail::to_string;

	typedef stdx::bitvector<> bitvec;

	string rep;

	bitset<65> _bits("11010101010101010101111111111000000011101000111110011111010100000");
	bitvec bits(_bits);
	rep = to_string(bits);

	auto it0 = find(bits.begin(), bits.begin() + 15, 1);
	size_t n0 = distance(bits.begin(), it0);
	REQUIRE(*it0);
	REQUIRE(bits[n0]);
	REQUIRE(n0 == 5);


	auto it1 = find(bits.begin() + 5, bits.begin() + 15, 0);
	size_t n1 = distance(bits.begin(), it1);
	REQUIRE_FALSE(*it1);
	REQUIRE_FALSE(bits[n1]);
	REQUIRE(6 == n1);

	fill(bits.begin(), bits.begin() + 5, true);
	rep = to_string(bits);
	REQUIRE(rep.substr(rep.size() - 5, 5) == "11111");

	fill(bits.begin() + 15, bits.begin() + 20, false);
	rep = to_string(bits);
	REQUIRE(rep.substr(rep.size() - 20, 5) == "00000");

	size_t n = count(bits.begin() + 5, bits.begin() + 15, 1);
	REQUIRE(7 == n);

	string buf = to_string(bits);
	rotate(buf.begin(), buf.end() - 5, buf.end());

	rotate(bits.begin(), bits.begin() + 5, bits.end());
	rep = to_string(bits);

	REQUIRE(buf == rep);

    bitset<25> vv("000000000000000001111111");
    bits = vv;
    rep = to_string(bits);
    while(std::next_permutation(rep.begin(), rep.end())) {
        stdx::next_permutation(bits.begin(), bits.end());
        buf = to_string(bits);
        REQUIRE(buf == rep);
    }

    while(std::prev_permutation(rep.begin(), rep.end())) {
        stdx::prev_permutation(bits.begin(), bits.end());
        buf = to_string(bits);
        REQUIRE(buf == rep);
    }


	/// bit transformation

	bitset<192> bx("111111100001010000000000000000000000000000000000000000000000000011111110000101000000000000000000000000000000000000000000000000001111111000010100000000000000000000000000000000000000000000000000");
	bitset<192> by("101010111000000000000000000000000000000000000000000000000010101010101011100000000000000000000000000000000000000000000000001010101010101110000000000000000000000000000000000000000000000000101010");
	bitset<192> bz("000000011111110000000000000000000000000000000000000000010111111100000001111111000000000000000000000000000000000000000001011111110000000111111100000000000000000000000000000000000000000101111111");
	bitvec x, y, z;
	string sx, sy, sz;

	auto __or  = [](const char a, const char b) -> char { return ('0' + ((a - '0') | (b - '0'))); };
	auto __xor = [](const char a, const char b) -> char { return ('0' + ((a - '0') ^ (b - '0'))); };
	auto __and = [](const char a, const char b) -> char { return ('0' + ((a - '0') & (b - '0'))); };

	/// whole bitset
	/// OR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.end(), y.begin(), z.begin(), bit_or<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rend(), sy.rbegin(), sz.rbegin(), __or);
	REQUIRE(sz == rep); // "Bitwise OR"

	/// XOR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.end(), y.begin(), z.begin(), bit_xor<>());
	rep = to_string(z); // "Bitwise XOR"

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rend(), sy.rbegin(), sz.rbegin(), __xor);
	REQUIRE(sz == rep);

	/// AND
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.end(), y.begin(), z.begin(), bit_and<>());
	rep = to_string(z); // "Bitwise AND"

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rend(), sy.rbegin(), sz.rbegin(), __and);
	REQUIRE(sz == rep);

	/// part of single word
	/// OR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.begin() + 7, y.begin(), z.begin(), bit_or<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	std::transform(sx.rbegin(), sx.rbegin() + 7, sy.rbegin(), sz.rbegin(), __or);
	REQUIRE(sz == rep);

	/// XOR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.begin() + 7, y.begin(), z.begin(), bit_xor<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rbegin() + 7, sy.rbegin(), sz.rbegin(), __xor);
	REQUIRE(sz == rep);

	/// AND
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.begin() + 7, y.begin(), z.begin(), bit_and<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rbegin() + 7, sy.rbegin(), sz.rbegin(), __and);
	REQUIRE(sz == rep);

	/// part of single word
	/// OR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 7, x.begin() + 36, y.begin() + 7, z.begin() + 7, bit_or<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 7, sx.rbegin() + 36, sy.rbegin() + 7, sz.rbegin() + 7, __or);
	REQUIRE(sz == rep);

	/// XOR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 7, x.begin() + 36, y.begin() + 7, z.begin() + 7, bit_xor<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 7, sx.rbegin() + 36, sy.rbegin() + 7, sz.rbegin() + 7, __xor);
	REQUIRE(sz == rep);

	/// AND
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 7, x.begin() + 36, y.begin() + 7, z.begin() + 7, bit_and<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 7, sx.rbegin() + 36, sy.rbegin() + 7, sz.rbegin() + 7, __and);
	REQUIRE(sz == rep);

	/// one single and one interleaved word
	/// OR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.begin() + 136, y.begin(), z.begin(), bit_or<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rbegin() + 136, sy.rbegin(), sz.rbegin(), __or);
	REQUIRE(sz == rep);

	/// XOR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.begin() + 136, y.begin(), z.begin(), bit_xor<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rbegin() + 136, sy.rbegin(), sz.rbegin(), __xor);
	REQUIRE(sz == rep);

	/// AND
	x = bx; y = by; z = bz;
	stdx::transform(x.begin(), x.begin() + 136, y.begin(), z.begin(), bit_and<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin(), sx.rbegin() + 136, sy.rbegin(), sz.rbegin(), __and);
	REQUIRE(sz == rep);

	/// two interleaved words
	/// OR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 108, x.begin() + 136, y.begin() + 108, z.begin() + 108, bit_or<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 108, sx.rbegin() + 136, sy.rbegin() + 108, sz.rbegin() + 108, __or);
	REQUIRE(sz == rep);

	/// XOR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 108, x.begin() + 136, y.begin() + 108, z.begin() + 108, bit_xor<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 108, sx.rbegin() + 136, sy.rbegin() + 108, sz.rbegin() + 108, __xor);
	REQUIRE(sz == rep);

	/// AND
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 108, x.begin() + 136, y.begin() + 108, z.begin() + 108, bit_and<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 108, sx.rbegin() + 136, sy.rbegin() + 108, sz.rbegin() + 108, __and);
	REQUIRE(sz == rep);

	/// whole single word
	/// OR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 128, x.end(), y.begin() + 128, z.begin(), bit_or<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 128, sx.rend(), sy.rbegin() + 128, sz.rbegin(), __or);
	REQUIRE(sz == rep);

	/// XOR
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 128, x.end(), y.begin() + 128, z.begin(), bit_xor<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 128, sx.rend(), sy.rbegin() + 128, sz.rbegin(), __xor);
	REQUIRE(sz == rep);

	/// AND
	x = bx; y = by; z = bz;
	stdx::transform(x.begin() + 128, x.end(), y.begin() + 128, z.begin(), bit_and<>());
	rep = to_string(z);

	sx = bx.to_string(); sy = by.to_string(); sz = bz.to_string();
	transform(sx.rbegin() + 128, sx.rend(), sy.rbegin() + 128, sz.rbegin(), __and);
	REQUIRE(sz == rep);
}



TEST_CASE("bitvector/operators", "[bitvector]")
{
	using namespace std;
	using ::detail::to_string;

	typedef stdx::bitvector<> bitvec;

	string rep;

	bitset<192> bx("111111100001010000000000000000000000000000000000000000000000000011111110000101000000000000000000000000000000000000000000000000001111111000010100000000000000000000000000000000000000000000000000");
	bitset<192> by("101010111000000000000000000000000000000000000000000000000010101010101011100000000000000000000000000000000000000000000000001010101010101110000000000000000000000000000000000000000000000000101010");
	bitset<192> bz;

	bitvec x = bx, y = by, z;

	bz = bx ^ by;
	z = x ^ y;

	rep = to_string(z);

	REQUIRE(bz.to_string() == rep);


	bz = bx & by;
	z = x & y;

	rep = to_string(z);
	REQUIRE(bz.to_string() == rep);

	bz = bx | by;
	z = x | y;

	rep = to_string(z);
	REQUIRE(bz.to_string() == rep);

}
