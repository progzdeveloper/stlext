#pragma once
#include <cstdint>
#include <climits>
#include <iostream>
#include <iomanip>
#include <bitset>
#include <stlext/iostreams/utility.hpp>


static void ioutility_example()
{
	using namespace std;

	/// This example show how to use
	/// count_digits() function to 
	/// count number of digits in a
	/// given number with different radix

	int x;
	
	cout << setfill(' ') << left;

	/// This block show  how to use compile-time supplied radix
	x = 0;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits<10>(x) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits<16>((uint32_t)x) << " hexadecimal digits" << endl;

	x = 10;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits<10>(x) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits<16>((uint32_t)x) << " hexadecimal digits" << endl;

	x = -100;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits<10>(x) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits<16>((uint32_t)x) << " hexadecimal digits" << endl;

	x = 1000;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits<10>(x) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits<16>((uint32_t)x) << " hexadecimal digits" << endl;

	x = 0xFF;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits<10>(x) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits<16>((uint32_t)x) << " hexadecimal digits" << endl;
	cout << setw(12) << std::bitset<sizeof(uint32_t) * CHAR_BIT>(x) << " has " << dec << stdx::count_digits<2>((uint32_t)x) << " binary digits (without leading zeroes)" << endl;

	cout << hex;


	/// This block show  how to use run-time supplied radix
	x = 0;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits(x, 10) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits((uint32_t)x, 16) << " hexadecimal digits" << endl;

	x = -1;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits(x, 10) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits((uint32_t)x, 16) << " hexadecimal digits" << endl;

	x = 0xFF;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits(x, 10) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits((uint32_t)x, 16) << " hexadecimal digits" << endl;
	cout << setw(12) << std::bitset<sizeof(uint32_t) * CHAR_BIT>(x) << " has " << dec << stdx::count_digits((uint32_t)x, 2) << " binary digits (without leading zeroes)" << endl;

	x = 0xFFA;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits(x, 10) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits((uint32_t)x, 16) << " hexadecimal digits" << endl;

	x = 0xFAFAA;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits(x, 10) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits((uint32_t)x, 16) << " hexadecimal digits" << endl;

	x = 0x0AFACCCC;
	cout << setw(12) << dec << x << " has " << dec << stdx::count_digits(x, 10) << " decimal digits" << endl;
	cout << setw(12) << hex << x << " has " << dec << stdx::count_digits((uint32_t)x, 16) << " hexadecimal digits" << endl;

	cout << dec << right;
}
