#include <catch.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stlext/iostreams/getline.hpp>
#include <stlext/functional/predicates.hpp>
#include <stlext/compability/utility>

TEST_CASE("iostreams/getline", "[iostreams]")
{
	using namespace std;
	string seps = " ,<>{}():'\t\r\n\"$*@;-.|#~=+\\/&^%!?";
	string str = "=^%$#@=?!A very,<very>{long}(string):'containing' \t\"text\"\n$with*!a?*m@ny;multi-separators.|:-)#that~must~be***splitted=\r\n\ninto+++some\\//\\\\-&-////different%^%strings!?";
	const char* expected[] = {
		"A",
		"very",
		"very",
		"long",
		"string",
		"containing",
		"text",
		"with",
		"a",
		"m",
		"ny",
		"multi",
		"separators",
		"that",
		"must",
		"be",
		"splitted",
		"into",
		"some",
		"different",
		"strings"
	};

	vector<string> results;
	stringstream buffer(str, ios::in | ios::out);
	string line;
	while (stdx::getline(buffer, line, stdx::is_any_of(seps))) {
		results.push_back(line);
	}

	REQUIRE(std::size(expected) == results.size());
	REQUIRE(equal(begin(expected), end(expected), results.begin()));
}


