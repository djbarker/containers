#include <iostream>
#include "list.hpp"

using namespace std;

typedef MultiList<int,3> mytype;

int main(void)
{
    mytype* list0 = new mytype(0);   
    mytype* list1 = list0;
    mytype* list2 = list0;
    for(int i=1;i<30;++i)
    {
        list0 = list0->insert_after<0>(new mytype(i));

        if(i%2==0) list1 = list1->insert_after<1>(list0);
        if(i%3==0) list2 = list2->insert_after<2>(list0);
    }
    
    // print the lists (backwards)
    auto tmp = list0;
    while(tmp!=nullptr)
    {
        cout << *tmp << ", ";
        tmp = tmp->conns[0].prev;
    }
    cout << endl;

    tmp = list1;
    while(tmp!=nullptr)
    {
        cout << *tmp << ", ";
        tmp = tmp->conns[1].prev;
    }
    cout << endl;

    tmp = list2;
    while(tmp!=nullptr)
    {
        cout << *tmp << ", ";
        tmp = tmp->conns[2].prev;
    }
    cout << endl;

    return 0;
}
