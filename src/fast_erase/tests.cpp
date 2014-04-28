#define BOOST_TEST_MODULE DequeTests
#include <boost/test/included/unit_test.hpp>

#include <utility>
#include <iostream>
#include <set>
#include "deque.hpp"

using namespace std;

using deque_t = fast_erase::deque<int, 5>;

/*
Checks if the given deque contains only the consecutive integers in the range [a,b)
*/
bool contains_range(deque_t& mydeq, int a, int b)
{
	std::set<int> ints;
	for (int i : mydeq) ints.insert(i);
	return (ints.size() == (b - a)) && ((*ints.begin()) == a) && ((*ints.rbegin()) == b-1);
}

std::ostream& operator<<(std::ostream& out, deque_t& mydeq)
{
	for (int i = 0; i < mydeq.size();++i)
			out << mydeq[i] << (i == (mydeq.size()-1) ? ' ' : ',');

	return out;
}

BOOST_AUTO_TEST_CASE(simple_tests)
{
	deque_t mydeq;

	BOOST_REQUIRE(mydeq.size() == 0);

	for (int i = 0; i < 10; ++i)
		mydeq.push_back(i);

	BOOST_REQUIRE(mydeq.size() == 10);

	for (int i = 0; i < 10; ++i)
		BOOST_REQUIRE(mydeq[i] == i);
	
}

BOOST_AUTO_TEST_CASE(move_tests)
{
	deque_t mydeq1, mydeq2;

	for (int i = 0; i < 10; ++i) mydeq1.push_back(i);

	cout << "Move assignment" << endl;

	// move assign
	mydeq2 = std::move(mydeq1);
	BOOST_REQUIRE(mydeq2.size() == 10 && mydeq1.size() == 0);
	BOOST_REQUIRE(contains_range(mydeq2, 0, 10));

	cout << "mydeq1 = " << mydeq1 << endl;
	cout << "mydeq2 = " << mydeq2 << endl;

	cout << "Move construct" << endl;

	// move construct
	deque_t mydeq3(std::move(mydeq2));
	BOOST_REQUIRE(mydeq3.size() == 10 && mydeq2.size() == 0);
	BOOST_REQUIRE(contains_range(mydeq3, 0, 10));

	cout << "mydeq2 = " << mydeq2 << endl;
	cout << "mydeq3 = " << mydeq3 << endl;
}

BOOST_AUTO_TEST_CASE(erase_tests)
{
	deque_t mydeq1;
	set<int> ints1;

	for (int i = 0; i < 30; ++i)
	{
		mydeq1.push_back(i);
		if (i<9 || i>17) ints1.insert(i);
	}
	
	for (int i = 9; i <= 17;++i) mydeq1.erase(i);
	
	set<int> ints2;
	for (int i : mydeq1) ints2.insert(i);

	BOOST_REQUIRE(ints1.size() == ints2.size());

	auto itr1 = ints1.begin();
	auto itr2 = ints2.begin();
	while (itr1 != ints1.end())
	{
		BOOST_REQUIRE((*itr1) == (*itr2));
		++itr1;
		++itr2;
	}
}

BOOST_AUTO_TEST_CASE(combine_tests)
{
	for (int n = 1; n <= 10; ++n)
	{
		deque_t mydeq1, mydeq2;

		for (int i = 0; i < 4*n; ++i)
		{
			mydeq1.push_back(i);
			mydeq2.push_back(i + 4*n);
		}

		mydeq1.append(mydeq2);
		//cout << "combined (n="<<n<<") = " << mydeq1 << endl;

		BOOST_REQUIRE_MESSAGE(mydeq1.size() == 8*n,"n = " << n << "\tmydeq1.size() = " << mydeq1.size());
		BOOST_REQUIRE_MESSAGE(mydeq2.size() == 0,  "n = " << n << "\tmydeq2.size() = " << mydeq2.size());

		// check all elements are present
		std::set<int> ints;
		for (auto& i : mydeq1) ints.insert(i);
		BOOST_REQUIRE_MESSAGE(ints.size() == 8*n, "n = " << n);
		BOOST_REQUIRE_MESSAGE((*ints.begin()) == 0 && (*ints.rbegin())==8*n-1, "n = " << n);

	}
}

BOOST_AUTO_TEST_CASE(split_tests)
{
	for (int n = 1; n <= 10; ++n)
	{
		deque_t mydeq1;
		for (int i = 0; i < 4*n; ++i) mydeq1.push_back(i);

		auto mydeq2 = mydeq1.split(2*n);

		BOOST_REQUIRE_MESSAGE(mydeq1.size() == 2*n && mydeq2.size() == 2*n, "n = " << n);
		BOOST_REQUIRE_MESSAGE(contains_range(mydeq1, 0, 2 * n), "n = " << n);
		BOOST_REQUIRE_MESSAGE(contains_range(mydeq2, 2 * n, 4 * n), "n = " << n);
	}
}