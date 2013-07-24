#ifndef DOTPRINTER_H_
#define DOTPRINTER_H_

#include <iostream>
#include <vector>
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
					out << '\"' << start << "\" [shape=box]" << std::endl; // declare node
					out << "edge [color=";
					switch(i)
					{
					case 1:
						out << "red]" << endl;
						break;
					default:
						out << "black]" << endl;
					}
					out << '\"' << start << "\" -> \"" << start->next(i) << '\"' << std::endl;
					start = start->next(i);
				}
			}

		out << "0 [label=\"nullptr\"]" << std::endl;
		out << "}" << std::endl;
	}

private:
	std::vector<MultiListElement<T,A>*> lists;
};

#endif /* DOTPRINTER_H_ */
