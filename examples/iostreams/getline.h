#pragma once
#include <cstdint>
#include <iostream>
#include <stlext/iostreams/getline.hpp>
#include <stlext/functional/predicates.hpp>

#include <string>

static void getline_example()
{
	using namespace std;

	/// This example show how to use 
	/// stdx::getline() function with
	/// conjunction of stdx::is_any_of
	/// predicate to split the source
	/// text

	vector<uint8_t> separators = {
		' ', '\r', '\n', '\t', 
		'$', ';',  '!',  '&'
	};

	// Print out separators in hex form
	cout << "separators: ";
	for (auto it = begin(separators); it != end(separators); ++it) {
		cout << setw(2) << hex << setfill('0') << (uint16_t)(*it) << ' ';
	}
	cout << dec << endl;

	// This is our source text string
	string source = "This is\ta&text\r\nwith;some$delimiters!";
	// Print out source text
	cout << "source text:   '" << source << "'" << endl << endl;
	cout << "splitted text: " << endl;

	// Emulate stream by using stringstream
	stringstream iss(source);
	// Iterate over lines using stdx::is_any_of() predicate to split the source
	while (stdx::getline(iss, source, stdx::is_any_of(separators))) {
		// Print out splitted line
		cout << '\'' << source << '\'' << endl;
	}
}

