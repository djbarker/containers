#include <iostream>
#include "list.hpp"

using namespace std;

struct mystruct {
    int val;

    mystruct():val(42){}
    mystruct(int v):val(v){}
};

typedef List<mystruct> mytype;

int main(void)
{
    mytype* list = new mytype(0);   
    for(int i=1;i<30;++i)
    {
        list->next = new mytype(i);
        list->next->prev = list;
        list = list->next;
        cout << list->data.val << endl;
        mystruct j = *list;
    }
    

    return 0;
}
