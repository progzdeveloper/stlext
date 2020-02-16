#pragma once
#include <iostream>

struct point2d
{
	float x, y;

	point2d(float ax = 0, float ay = 0) :
		x(ax), y(ay) {}

	point2d(const point2d& other) = default;
	point2d& operator=(const point2d& other) = default;
};

inline bool operator== (const point2d& p1, const point2d& p2) {
	return ((p1.x == p2.x) && (p1.y == p2.y));
}

inline bool operator!= (const point2d& p1, const point2d& p2) {
	return !(p1 == p2);
}

inline std::ostream& operator<< (std::ostream& stream, const point2d& p) {
	return (stream << '(' << p.x << ';' << p.y << ')');
}

