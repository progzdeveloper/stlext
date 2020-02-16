#include <iostream>
#include <forward_list>
#include <algorithm>
#include <iterator>

// include all experimental algorithms
#include <stlext/algorithm/experimental.h>


#include "../point2d.h"


// example usage of stdx::majority_element() algorithm
static void majority_element_example()
{
	using namespace std;

	/// Part I. Basic usage
	///
	/// In this part of example we will use stdx::majority_element() algorithm
	/// to find out the majority element in forward list of integers.

	// put some numbers into the list
	std::forward_list<int> lst = { 1, -2, 1, 5, 8, 1, -3, 1, 1, 1, -4, 1, 7, 1, 1 };

	// print out the list
	copy(lst.begin(), lst.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	// now determine the majority element
	auto it = stdx::majority_element(lst.begin(), lst.end());
	// check if iterator is valid, i.e. we found a result
	if (it == lst.end()) {
		cout << "no majority element was found" << endl;
	}
	else {
		cout << "majority element: " << *it 
			 << " was found at position: " << std::distance(lst.begin(), it) << endl;
	}
	cout << endl;

	/// Part II. Advanced usage
	///
	/// In this part of example we will examinate advanced usage of
	/// stdx::majority_element() algorithm.
	/// Our task now is to find out the majority element in forward 
	/// list of user-defined type.
	/// This part of example illustrate possibilities of customizing 
	/// stdx::majority_element() algorithm by providing custom 
	/// comparision predicates.

	// put some points into the list
	std::forward_list<point2d> points = { 
		{ 1,   1  },
		{ -2,  2  },
		{ 1,   3  },
		{ 5,   4  },
		{ 8,   5  },
		{ 1,   6  },
		{ -3,  7  },
		{ 1,   8  },
		{ 1,   9  },
		{ 1,   10 },
		{ -4,  11 },
		{ 1,   12 },
		{ 7,   13 },
		{ 1,   14 },
		{ 1,   15 }
	};

	// print out the list
	copy(points.begin(), points.end(), ostream_iterator<point2d>(cout, "\n"));
	cout << endl;

	// now determine the majority element using default predicate
	auto pos = stdx::majority_element(points.begin(), points.end());
	// check if iterator is valid, i.e. we found a result
	if (pos == points.end()) {
		cout << "no majority element was found using default predicate" << endl;
	}
	else {
		cout << "majority element: " << *pos
			<< " was found at position: " << std::distance(points.begin(), pos)
			<< " using default predicate" << endl;
	}

	// define a custom comparision predicate: compare only x-coordinate
	auto pred = [](const point2d& p1, const point2d& p2) { return p1.x == p2.x; };

	// now determine the majority element using custom predicate
	pos = stdx::majority_element(points.begin(), points.end(), pred);
	// check if iterator is valid, i.e. we found a result
	if (pos == points.end()) {
		cout << "no majority element was found using custom predicate" << endl;
	}
	else {
		cout << "majority element: " << *pos
			<< " was found at position: " << std::distance(points.begin(), pos)
			<< " using custom predicate" << endl;
	}
}




// example usage of stdx::is_majority_element() algorithm
static void is_majority_element_example()
{
	/// Part I. Basic usage
	/// 
	/// In this part of example we will use stdx::is_majority_element() algorithm
	/// check that provided value is really the majority element in a given  
	/// forward list of integers.

	using namespace std;

	// put some numbers into the list
	std::forward_list<int> lst = { 0, -2, 0, 5, 8, 0, -3, 0, 0, 0, -4, 0, 7, 0, 0 };

	// print out the list
	copy(lst.begin(), lst.end(), ostream_iterator<int>(cout, " "));
	cout << endl;

	// assume that majority element value is 0
	int maj = 0;
	// now confirm that maj is really the majority element in lst
	bool result = stdx::is_majority_element(lst.begin(), lst.end(), maj);
	// print the results
	cout << maj << (result ?  " is " : " not ") << "a majority element" << endl;




	/// Part II. Advanced usage
	/// 
	/// In this part of example we will use stdx::is_majority_element() algorithm
	/// check that provided value really is the majority element in a given  
	/// forward list of integers. But in this time we will use custom defined
	/// types along with custom comparision predicate.
	/// This example show how stdx::is_majority_element() algorithm can be customized.

	// put some points into the list
	std::forward_list<point2d> points = {
		{ 1,   1 },
		{ -2,  2 },
		{ 1,   3 },
		{ 5,   4 },
		{ 8,   5 },
		{ 1,   6 },
		{ -3,  7 },
		{ 1,   8 },
		{ 1,   9 },
		{ 1,   10 },
		{ -4,  11 },
		{ 1,   12 },
		{ 7,   13 },
		{ 1,   14 },
		{ 1,   15 }
	};

	// print out the list
	copy(points.begin(), points.end(), ostream_iterator<point2d>(cout, "\n"));
	cout << endl;

	//define custom predicate
	auto pred = [](const point2d& p1, const point2d& p2) {
		return (p1.x < p2.x);
	};

	// assume that majority element value is (1, 9)
    point2d pmaj(1.0f, 9.0f);
	// now confirm using custom predicate that major is really the majority element in lst 
    bool res = stdx::is_majority_element(points.begin(), points.end(), pmaj, pred);
	// print the results
    cout << pmaj << (res ? " is " : " not ") << "a majority element (custom predicate used)" << endl;
}


