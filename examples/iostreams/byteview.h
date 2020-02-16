#pragma once
#include <iostream>
#include <stlext/iostreams/byte_view.hpp>

static void byte_view_example()
{
	using namespace std;
	uint8_t bytes[] = {
		0xDE, 0xAD, 0xBE, 0xEF,
		0xCC, 0xCC, 0xCC, 0xCC,
		0xA0, 0xA1, 0xA2, 0xA3,
		0xFF, 0xFF, 0x00, 0x01,
		0xFF, 0xFD, 0xDD, 0x20,
		0x30, 0x31, 0x32, 0x33,
		0x34, 0x35, 0x36, 0x37,
		0x38, 0x39
	};

	cout << "raw bytes: ";
	for (auto it = begin(bytes); it != end(bytes); ++it) {
		cout << setw(2) << hex << setfill('0') << (uint16_t)(*it) << ' ';
	}
	cout << dec << endl << endl;

	cout << "Hexadecimal form without offsets (column width = 4):" << endl;
    auto hex_viewer = stdx::hex_view(begin(bytes), end(bytes), (std::ios::fmtflags)0, 4);
	cout << hex_viewer << endl;

	cout << "Hexadecimal form with offsets (column width = 16):" << endl;
    cout << stdx::hex_view(begin(bytes), end(bytes), (std::ios::fmtflags)0, 16, true) << endl;

	cout << "Octal form: " << endl;
	cout << stdx::oct_view(bytes) << endl;

	cout << "Decimal form: " << endl;
	auto viewer = stdx::make_view<std::ios::dec>(begin(bytes), end(bytes));
	cout << viewer << endl;

}

