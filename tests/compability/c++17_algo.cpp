#include <catch.hpp>

#include <list>
#include <vector>
#include <string>
#include <cstring>

#include <stlext/compability/cxx17/for_each.hpp>
#include <stlext/compability/cxx17/invoke.hpp>


namespace detail
{
	void foo() { REQUIRE(true); }
	int bar() { REQUIRE(true); return 0; }

	class _Clazz {
		std::string m_name;
	public:
		_Clazz() : m_name("_Clazz") {}
		int exec(int x) { return x; }
		const std::string& name() const { return m_name; }
	};
}


TEST_CASE("functional/invoke", "[compability.cpp17]")
{
	using namespace ::detail;

	stdx::invoke(foo);
	REQUIRE(stdx::invoke(bar) == 0);
	REQUIRE(stdx::invoke([](int x) { REQUIRE(true); return x; }, -1) == -1);
	REQUIRE(stdx::invoke(std::strtoull, "0xFF", nullptr, 16) == 0xFFULL);

	std::function<int(int)> fx = [](int x) { return x; };
	REQUIRE(stdx::invoke(fx, 11) == 11);

	_Clazz c;
	REQUIRE(stdx::invoke(&_Clazz::exec, &c, 55) == 55);
	REQUIRE(stdx::invoke(&_Clazz::name, &c) == "_Clazz");

	char buf[64];
	size_t n = stdx::invoke(snprintf, buf, 64, "%s %i %f", "Nums:", -10, 0.5f);
	REQUIRE(strncmp(buf, "Nums: -10 0.500000", n) == 0);
}

TEST_CASE("algorithms/for_each_n", "[compability.cpp17]")
{
	using namespace ::detail;

	std::vector<int> vec(10), vres;
	for (int i = 0; i < 10; i++) vec[i] = i;
	stdx::for_each_n(vec.begin(), 5, [&](int x) { vres.push_back(x); });
	REQUIRE(std::equal(vres.begin(), vres.end(), vec.begin()));

	std::list<int> lst(vec.begin(), vec.end()), lres;
	stdx::for_each_n(lst.begin(), 5, [&](int x) { lres.push_back(x); });
	REQUIRE(std::equal(lres.begin(), lres.end(), lst.begin()));
}




