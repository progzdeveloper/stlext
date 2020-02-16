#pragma once
#include <cstdint>
#include <iostream>
#include <stlext/iostreams/timestamp.hpp>
#include <stlext/iostreams/ratio.hpp>


static void iomanips_example()
{
	/// This example show how to use 
	/// stdx::ratio_traits to pretty
	/// print different SI and IEC ratios

	using namespace std;
	using stdx::ratio_traits;

    locale::global(stdx::system_locale());

	cout << "\nCurrent date/time is: " << stdx::timestamp << endl;

#ifdef STDX_OS_WIN
	cout << "\nWARNING: Not all symbols may display correctly due to bugs in Microsoft Windows Console output!" << endl;
#endif

	cout << "\nSI units:" << endl;
	cout << ratio_traits<atto, char>::prefix() << '\t' 
		 << ratio_traits<atto, char>::symbol() << endl;

	cout << ratio_traits<femto, char>::prefix() << '\t'
		 << ratio_traits<femto, char>::symbol() << endl;

	cout << ratio_traits<pico, char>::prefix() << '\t'
		 << ratio_traits<pico, char>::symbol() << endl;

	cout << ratio_traits<nano, char>::prefix() << '\t'
		 << ratio_traits<nano, char>::symbol() << endl;

	cout << ratio_traits<micro, char>::prefix() << '\t'
		 << ratio_traits<micro, char>::symbol() << endl;

	cout << ratio_traits<milli, char>::prefix() << '\t'
		 << ratio_traits<milli, char>::symbol() << endl;

	cout << ratio_traits<centi, char>::prefix() << '\t'
		 << ratio_traits<centi, char>::symbol() << endl;

	cout << ratio_traits<deci, char>::prefix() << '\t'
		 << ratio_traits<deci, char>::symbol() << endl;

	cout << ratio_traits<deca, char>::prefix() << '\t'
		 << ratio_traits<deca, char>::symbol() << endl;

	cout << ratio_traits<hecto, char>::prefix() << '\t'
		 << ratio_traits<hecto, char>::symbol() << endl;

	cout << ratio_traits<kilo, char>::prefix() << '\t'
		 << ratio_traits<kilo, char>::symbol() << endl;

	cout << ratio_traits<mega, char>::prefix() << '\t'
		 << ratio_traits<mega, char>::symbol() << endl;

	cout << ratio_traits<giga, char>::prefix() << '\t'
		 << ratio_traits<giga, char>::symbol() << endl;

	cout << ratio_traits<tera, char>::prefix() << '\t'
		 << ratio_traits<tera, char>::symbol() << endl;

	cout << ratio_traits<peta, char>::prefix() << '\t'
		 << ratio_traits<peta, char>::symbol() << endl;

	cout << ratio_traits<exa, char>::prefix() << '\t'
		 << ratio_traits<exa, char>::symbol() << endl;


	cout << "\nIEC units:" << endl;
	cout << ratio_traits<stdx::kibi, char>::prefix() << '\t'
		 << ratio_traits<stdx::kibi, char>::symbol() << endl;

	cout << ratio_traits<stdx::mebi, char>::prefix() << '\t'
		 << ratio_traits<stdx::mebi, char>::symbol() << endl;

	cout << ratio_traits<stdx::gibi, char>::prefix() << '\t'
		 << ratio_traits<stdx::gibi, char>::symbol() << endl;

	cout << ratio_traits<stdx::tebi, char>::prefix() << '\t'
		 << ratio_traits<stdx::tebi, char>::symbol() << endl;

	cout << ratio_traits<stdx::pebi, char>::prefix() << '\t'
		 << ratio_traits<stdx::pebi, char>::symbol() << endl;

	cout << ratio_traits<stdx::exbi, char>::prefix() << '\t'
		 << ratio_traits<stdx::exbi, char>::symbol() << endl;

    locale::global(locale("C"));
}

