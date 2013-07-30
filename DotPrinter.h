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

static const std::string colours[4] = {"black","red","blue","green"};

template<typename T, size_t A, size_t A_>
void _wrapper_print_impl(std::ostream& out, MultiList<T,A>* the_list)
{
	for(auto itr = the_list->template begin<A_>(); itr!=the_list->template end<A_>(); ++itr)
	{
		auto ml_element = itr.get();
		out << '\"' << ml_element << "\" [shape=box,label="
			<< (ml_element-the_list->template begin<0>().get()) << "]" << std::endl; // declare node
		out << "edge [color=" << colours[A_] << "]" << std::endl; // declare edge
		out << '\"' << ml_element << "\" -> \"" << ml_element->template next<A_>() << '\"' << std::endl;
	}
}

template<typename T, size_t A, size_t A_>
struct _wrapper
{
	static void print_inner(std::ostream& out, MultiList<T,A>* the_list)
	{
		_wrapper_print_impl<T,A,A_>(out,the_list);

		_wrapper<T,A,A_-1>::print_inner(out,the_list);
	}
};


template<typename T, size_t A>
struct _wrapper<T,A,0>
{
	static void print_inner(std::ostream& out, MultiList<T,A>* the_list){
		_wrapper_print_impl<T,A,0>(out,the_list);
	}
};

template<typename T, size_t A>
class DotPrinter {
public:
	DotPrinter(){};
	virtual ~DotPrinter(){};

	void addMultiList(MultiList<T,A>* list)
	{
		the_list = list;
	}

	void print(std::ostream& out)
	{
		using namespace std;
		out << "digraph G {" << std::endl;

		_wrapper<T,A,A-1>::print_inner(out,the_list);

		out << "0 [label=\"nullptr\"]" << std::endl;
		out << "}" << std::endl;
	}

private:

	MultiList<T,A>* the_list;
};

#endif /* DOTPRINTER_H_ */
