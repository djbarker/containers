#include <iostream>
#include "list.hpp"

using namespace std;

struct mystruct {
    int val;

    mystruct():val(42){}
    mystruct(int v):val(v){}
};

typedef DualList<mystruct> mytype;

int main(void)
{
    mytype* list0 = new mytype(0);   
    mytype* list1 = list0;
    for(int i=1;i<30;++i)
    {
        auto tmp = new mytype(i);
        list0->insert_after<0>(tmp);
        list0 = tmp;//list0 = list0->conns[0].next;

        if(i%2==0)
        {
            list1->insert_after<1>(list0);
            list1 = list1->conns[1].next;
        }
    }
    
    // print the lists (backwards)
    auto tmp = list0;
    while(tmp!=nullptr)
    {
        mystruct j = *tmp;
        cout << j.val << ",\t";
        tmp = tmp->conns[0].prev;
    }
    cout << endl;

    tmp = list1;
    while(tmp!=nullptr)
    {
        mystruct j = *tmp;
        cout << j.val << ",\t";
        tmp = tmp->conns[1].prev;
    }
    cout << endl;

    return 0;
}
