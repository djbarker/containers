#include <iostream>
#include <stdexcept>
#include <memory>

template<typename T>
struct List {
    T data;
    List<T>* next;
    List<T>* prev;

    // forwarding constructor
    template<typename... Ts>
    List(Ts... vs):data(vs...){} 

    // cast to data type
    operator T& ()
    {
        return data;
    }

    static void* operator new(size_t size) throw(std::bad_alloc)
    {        
        if(pool==nullptr)
        {
            // allocate contiguous storage            
            pool = new List<T>[pool_size];

            // link the pool elements together
            pool[0].prev = nullptr;
            pool[0].next = pool+1;
            for(size_t i=1; i<pool_size-1; ++i) {
                pool[i].prev = pool+i-1;
                pool[i].next = pool+i+1;
            }
            pool[pool_size-1].prev = pool+pool_size-1;
            pool[pool_size-1].next = nullptr;
            return (void*)pool;
        }
        else
        {
            // "pop_front"
            if(pool->next!=nullptr) pool->next->prev = nullptr;
            auto out = pool;
            pool = pool->next;
            return (void*)out;
        }
    }

    static void operator delete(void* p_in)
    {
            List<T>* element = (List<T>*)p_in;
            if(pool==nullptr)
            {
                pool = element;
                pool->next = nullptr;
                pool->prev = nullptr; // not strictly necessary
            }
            else
            {
                // "push_front"
                pool->prev = element;
                element->next = pool;
                pool = element;          
            }
    }

    static constexpr size_t pool_size = 10;
    static List<T>* pool;
    static size_t news;
    static size_t dels;
};

template<typename T> List<T>* List<T>::pool = nullptr;
template<typename T> size_t   List<T>::news = 0;
template<typename T> size_t   List<T>::dels = 0;
