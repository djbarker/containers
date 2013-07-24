#ifndef DOTPRINTER_H_
#define DOTPRINTER_H_

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "multi_list.hpp"

/*
 * Class to print MultiList structures
 * as DOT graphs for visualization.
 */

template<typename T, size_t A>
class DotPrinter {
public:
	DotPrinter(){};
	virtual ~DotPrinter(){};

	void addMultiList(MultiListElement<T,A>* start)
	{
		lists.push_back(start);
	}

	void print(std::ostream& out)
	{
		using namespace std;
		out << "digraph G {" << std::endl;

		for(auto pList : lists)
			for(int i=0;i<A;++i)
			{
				MultiListElement<T,A>* start = pList;
				while(start!=nullptr)
				{
					out << '\"' << start << "\" [shape=box,label="
						<< (start-lists[0]) << "]" << std::endl; // declare node
					out << "edge [color=" << colours[i] << "]" << endl; // declare edge
					out << '\"' << start << "\" -> \"" << start->next(i) << '\"' << std::endl;
					start = start->next(i);
				}
			}

		out << "0 [label=\"nullptr\"]" << std::endl;
		out << "}" << std::endl;
	}

private:
	static const std::string colours[4];
	std::vector<MultiListElement<T,A>*> lists;
};

template<typename T, size_t A>
const std::string DotPrinter<T,A>::colours[4] = {"black","red","blue","green"};

#endif /* DOTPRINTER_H_ */
