#include <iostream>
#include "list.hpp"

using namespace std;

int main(void)
{
    MultiList<int,2,0> main;
    MultiList<int,2,1> sub;
    
    for(int i=0;i<30;++i)
    {
    	auto tmp = new MultiList<int,2,0>::element(i);

    	main.push_back(tmp);
    	if(i%2==0) sub.push_back(tmp);
    }

    cout << "Printing..." << endl;

    for(auto d : main) cout << d << ", ";
    cout << endl;

    for(auto d : sub) cout << d << ", ";
    cout << endl;

    return 0;
}
