#pragma once
#include <iostream>
#include <iomanip>
#include <set>
#include <regex>

#define _EXAMPLE(_Samples, _Function, _Descr) _Samples.emplace( &_Function##_example, #_Function, _Descr )

typedef void(*callback_t)();
struct entry
{
	callback_t  example;
	const char* title;
	const char* description;
	entry(callback_t e, const char* atitle, const char* adescr) :
		example(e), title(atitle), description(adescr) {
	}
};
typedef std::set<entry> sample_map;




inline std::ostream& operator<< (std::ostream& stream, const entry& e)
{
	using namespace std;
	static constexpr size_t title_width = 20, help_width = 32;
	static const char spacing[title_width + 1] = { ' ' };

	stream << ' ' << left << setw(title_width) << e.title << ' ';
	size_t length = 0;

	const char* first = e.description;
	const char* last = e.description + strlen(first);

	for (; first != last; ++first, ++length)
	{
        if (::isspace(*first) && length > help_width) {
			length = 0;
			stream << '\n';
			stream.write(spacing, title_width + 1);
		}
		stream << *first;
	}
	return (stream << endl);
}

inline bool operator< (const entry& lhs, const entry& rhs) {
	return (strcmp(lhs.title, rhs.title) < 0);
}



class filter
{
public:
	explicit filter(const std::string& s) :
		pattern(s)
	{
		if (!pattern.empty())
			rx.assign(pattern);
	}

	void operator()(const entry& e) const { run(e); }

protected:
	virtual void run(const entry&) const = 0;

	inline bool accepted(const entry& e) const {
		if (!pattern.empty())
			if (!regex_match(e.title, rx))
				return false;
		return true;
	}
private:
	std::string pattern;
	std::regex rx;
};


class executer : public filter
{
public:
	explicit executer(const std::string& pattern, bool interactive) :
		filter(pattern), m_interactive(interactive) {
	}

	inline void run(const entry& e) const
	{
		using namespace std;
		if (!accepted(e))
			return;

		cout << e.title << " example: " << endl;
		try {
			e.example();
		}
		catch (exception& err) {
			cerr << "failed to run " << e.title
				<< " example: " << err.what() << endl;
		}
		catch (...) {
			cerr << "failed to run " << e.title
				<< " example: unexpected exception" << endl;
		}
		cout << endl << endl;
		if (m_interactive) {
			cout << "Press enter to continue..." << endl;
			cin.get();
			cin.clear();
		}
	}
private:
	bool m_interactive;
};



class enumerator : public filter
{
public:
	explicit enumerator(const std::string& pattern) :
		filter(pattern) {
	}

	inline void run(const entry& e) const
	{
		using namespace std;
		if (accepted(e))
			cout << e << endl;
	}
};

