#include <iostream>
#include <fstream>
#include <random>
#include <functional>
#include "multi_list.hpp"
#include "DotPrinter.h"

using namespace std;

int main(void)
{
	std::uniform_int_distribution<int> distribution(0, 1);
	std::mt19937 engine;
	engine.seed(42);
	auto generator = std::bind(distribution, engine);

    MultiList<int,2,0> main;
    MultiList<int,2,1> sub;
    
    for(int i=0;i<50;++i)
    {
    	auto tmp = new MultiList<int,2,0>::element(i);


    	main.push_back(tmp);
    	//if(i%2==0) sub.push_back(tmp);
    	if(generator()<1 || i==0) sub.push_back(tmp);
    }

    cout << "Printing..." << endl;

    ofstream fout("test.dot");
    if(!fout.is_open())
    {
    	cerr << "Unable to open test.dot" << endl;
    	return 1;
    }

    DotPrinter<int,2> printer;
    printer.addMultiList(main.begin().get());
    printer.print(fout);
    fout.close();

    return 0;
}
