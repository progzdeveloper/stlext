#pragma once
#include <iostream>
#include <string>


struct record
{
	std::string name;
	double rating;
	int id;

	record() : rating(0), id(-1) {}
	record(int i, const std::string& s, double r) :
		name(s), rating(r), id(i) {}

	record(const record&) = default;
	record& operator= (const record&) = default;
};


inline bool operator== (const record& lhs, const record& rhs)
{
	return (lhs.id == rhs.id &&
			lhs.rating == rhs.rating &&
			lhs.name == rhs.name);
}

inline bool operator!= (const record& lhs, const record& rhs)
{
	return !(lhs == rhs);
}

inline std::ostream& operator<< (std::ostream& stream, const record& r) {
	return (stream << " ID: " << r.id
				   << " \n NAME: " << r.name
				   << " \n RATING: " << r.rating);
}

