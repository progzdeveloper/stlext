#include <catch.hpp>

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include <stlext/iostreams/ratio.hpp>


TEST_CASE("iostreams/ratio.si", "[manipulators]")
{
	using namespace std;

	stringstream buffer;
	string suffix;

	suffix = "meters\n";
	buffer << stdx::ratio_traits<atto,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<femto, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<pico,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<nano,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<micro, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<milli, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<centi, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<deci,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<deca,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<hecto, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<kilo,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<mega,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<giga,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<tera,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<peta,  char>::prefix() << suffix;
	buffer << stdx::ratio_traits<exa,   char>::prefix() << suffix;
	const char* expected_prefixes =
		"attometers\n"
		"femtometers\n"
		"picometers\n"
		"nanometers\n"
		"micrometers\n"
		"millimeters\n"
		"centimeters\n"
		"decimeters\n"
		"decameters\n"
		"hectometers\n"
		"kilometers\n"
		"megameters\n"
		"gigameters\n"
		"terameters\n"
		"petameters\n"
		"exameters\n";

	REQUIRE(expected_prefixes == buffer.str());

	suffix = "m/s\n";
	buffer.str(string());
	buffer << stdx::ratio_traits<atto,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<femto, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<pico,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<nano,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<micro, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<milli, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<centi, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<deci,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<deca,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<hecto, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<kilo,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<mega,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<giga,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<tera,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<peta,  char>::symbol() << suffix;
	buffer << stdx::ratio_traits<exa,   char>::symbol() << suffix;

	const char* expected_symbols =
		"am/s\n"
		"fm/s\n"
		"pm/s\n"
		"nm/s\n"
        "\xC2\xB5m/s\n"
		"mm/s\n"
		"cm/s\n"
		"dm/s\n"
		"dam/s\n"
		"hm/s\n"
		"km/s\n"
		"Mm/s\n"
		"Gm/s\n"
		"Tm/s\n"
		"Pm/s\n"
		"Em/s\n";

	REQUIRE(expected_symbols == buffer.str());
}



TEST_CASE("iostreams/ratio.iec", "[manipulators]")
{
	using namespace std;

	stringstream buffer;
	string suffix;

	suffix = "bytes\n";
	buffer << stdx::ratio_traits<stdx::kibi, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<stdx::mebi, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<stdx::gibi, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<stdx::tebi, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<stdx::pebi, char>::prefix() << suffix;
	buffer << stdx::ratio_traits<stdx::exbi, char>::prefix() << suffix;
	const char* expected_prefixes =
		"kibibytes\n"
		"mebibytes\n"
		"gibibytes\n"
		"tebibytes\n"
		"pebibytes\n"
		"exbibytes\n";

	REQUIRE(expected_prefixes == buffer.str());


	buffer.str(string());
	suffix = "B\n";
	buffer << stdx::ratio_traits<stdx::kibi, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<stdx::mebi, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<stdx::gibi, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<stdx::tebi, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<stdx::pebi, char>::symbol() << suffix;
	buffer << stdx::ratio_traits<stdx::exbi, char>::symbol() << suffix;
	const char* expected_symbols =
		"KiB\n"
		"MiB\n"
		"GiB\n"
		"TiB\n"
		"PiB\n"
		"EiB\n";

	REQUIRE(expected_symbols == buffer.str());
}


