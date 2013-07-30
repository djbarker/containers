#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include "multi_list.hpp"
#include "DotPrinter.h"

using namespace std;

int main(void)
{
	std::uniform_int_distribution<int> distribution(0, 2);
	std::mt19937 engine;
	engine.seed(42);
	auto generator = std::bind(distribution, engine);

    MultiList<int,3> main;
    
    for(int i=0;i<20;++i)
    {
    	main.push_back<0>(i);
    	if(generator()<2 || i==0) main.push_back<1>(i);
    	if(generator()<1 || i==0) main.push_back<2>(i);
    }

    cout << "Printing..." << endl;

    ofstream fout("test.dot");
    if(!fout.is_open())
    {
    	cerr << "Unable to open test.dot" << endl;
    	return 1;
    }

    DotPrinter<int,3> printer;
    printer.addMultiList(&main);
    printer.print(fout);

    fout.close();

    return 0;
}
