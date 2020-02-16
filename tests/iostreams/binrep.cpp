#include <catch.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>

#include <stlext/bitvector/bitvector.hpp>
#include <stlext/iostreams/bin.hpp>


TEST_CASE("iostreams/binrep", "[manipulators]")
{
	using namespace std;

	string result;

	bitset<16> bits("0101110111010101");
	stdx::bitvector<uint64_t> bitvec(bits);
	uint8_t u8 = 255;
	uint16_t u16 = 65534;
	uint32_t u32 = 0xFFAA44CC;
	uint64_t u64 = 0xFFAACCBB11332255;
	int ival = -65535; // ???

	stringstream buffer;
	buffer << stdx::bin(bits) << endl;
	buffer << stdx::bin(bitvec) << endl;
	buffer << stdx::bin(u8) << endl;
	buffer << stdx::bin(u16) << endl;
	buffer << stdx::bin(u32) << endl;
	buffer << stdx::bin(u64) << endl;
	buffer << stdx::bin(ival) << endl;

	result = buffer.str();

	buffer.str(string());
	buffer << bits << endl;
	buffer << bitvec << endl;
	buffer << bitset<8>(u8) << endl;
	buffer << bitset<16>(u16) << endl;
	buffer << bitset<32>(u32) << endl;
	buffer << bitset<64>(u64) << endl;
	buffer << bitset<32>(ival) << endl;

	REQUIRE(buffer.str() == result);


	buffer.str(string());
	buffer << stdx::bin(bits, 4) << endl;
	buffer << stdx::bin(bitvec, 8, " | ") << endl;
	buffer << stdx::bin(u8) << endl;
	buffer << stdx::bin(u16, 4, "-") << endl;
	buffer << stdx::bin(u32, 8, "\t") << endl;
	buffer << stdx::bin(u64, 16, "|") << endl;
	buffer << stdx::bin(ival, 8, "  ") << endl;

	const char* expected =
		"0101 1101 1101 0101\n"
		"01011101 | 11010101\n"
		"11111111\n"
		"1111-1111-1111-1110\n"
		"11111111	10101010	01000100	11001100\n"
		"1111111110101010|1100110010111011|0001000100110011|0010001001010101\n"
		"11111111  11111111  00000000  00000001\n";

	REQUIRE(buffer.str() == expected);
}
