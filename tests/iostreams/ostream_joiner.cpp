#include <catch.hpp>

#include <sstream>
#include <string>

#include <stlext/iostreams/ostream_joiner.hpp>


struct indentator
{
	std::string str;
	char filler;
	mutable int indent;

	indentator(const std::string& s, char c) :
		str(s), filler(c), indent(0) {
	}

	void reset() { indent = 0; }

	friend inline std::ostream& operator<<(std::ostream& stream, const indentator& delim) {
		return (stream << delim.str << std::string(++delim.indent, delim.filler));
	}

};


TEST_CASE("iostreams/ostream_joiner", "[iostreams]")
{
	using namespace std;

	const char* strs[] = { "one", "two", "three", "four", "five" };
	int ivals[] = { 0, 1, 2, 3, 4, 5 };
    //float fvals[] = { 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f };


	string result;
	ostringstream buf;

	buf.str(result);
	copy(begin(strs), end(strs), stdx::make_ostream_joiner(buf, ','));
	result = buf.str();
	REQUIRE(result == "one,two,three,four,five");
	result.erase(); // clear

	buf.str(result);
	copy(begin(ivals), end(ivals), stdx::make_ostream_joiner(buf, ", "));
	result = buf.str();
	REQUIRE(result == "0, 1, 2, 3, 4, 5");
	result.erase(); // clear

	buf.str(result);
	copy(begin(ivals), end(ivals), stdx::make_ostream_joiner(buf, indentator(":\n", '.')));
	result = buf.str();
	REQUIRE(result == "0:\n.1:\n..2:\n...3:\n....4:\n.....5");
	result.erase(); // clear
}
