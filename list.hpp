#include <iostream>
#include <stdexcept>
#include <array>
#include <memory>

template<typename T, size_t N>
struct MultiList {

    // sanity check 
    static_assert(N>0,"Multilist cannot have \'arity\' paramter <1.");

    T data;

    // struct for storing connections
    struct Connection {
        MultiList<T,N>* prev;
        MultiList<T,N>* next;

        Connection()
        :prev(nullptr)
        ,next(nullptr)
        {
        }

        Connection(MultiList<T,N>* p, MultiList<T,N>* n)
        :prev(p)
        ,next(n)
        {
        }
    };

    // connections
    std::array<Connection,N> conns;

    // forwarding constructor
    template<typename... Ts>
    MultiList(Ts... vs):data(vs...){}

    // non-copyable
    MultiList<T,N>(const MultiList<T,N>&) = delete;

    // cast to data type
    operator T& ()
    {
        return data;
    }

    // push onto specified list
    template<size_t L>
    void insert_before(MultiList<T,N>& el) throw()
    {
        if(conns[L].prev!=nullptr) conns[L].prev->conns[L].next = &el;
        el.conns[L].prev = conns[L].prev;
        el.conns[L].next = this;
        conns[L].prev = &el;
    }

    template<size_t L>
    void insert_after(MultiList<T,N>* el) throw()
    {
        el->conns[L].next = conns[L].next;
        el->conns[L].prev = this;
        if(conns[L].next!=nullptr) conns[L].next->conns[L].prev = el;
        conns[L].next = el;
    }

    template<size_t L>
    void insert_before(T&& t)
    {
        insert_before<L>(new MultiList<T,N>(t));
    }

    template<size_t L>
    void insert_after(T&& t)
    {
        insert_after<L>(new MultiList<T,N>(t));
    }

    /*
     * Custom memory allocation routines.
     */
    static void* operator new(size_t size) throw(std::bad_alloc)
    {        
        if(pool==nullptr)
        {
            // allocate contiguous storage            
            pool = new MultiList<T,N>[pool_size];

            // link the pool elements together using connection zero
            pool[0].conns[0].prev = nullptr;
            pool[0].conns[0].next = pool+1;
            for(size_t i=1; i<pool_size-1; ++i) {
                pool[i].conns[0].prev = pool+i-1;
                pool[i].conns[0].next = pool+i+1;
            }
            pool[pool_size-1].conns[0].prev = pool+pool_size-1;
            pool[pool_size-1].conns[0].next = nullptr;
            auto out = pool;
            pool = pool+1;
            return (void*)out;
        }
        else
        {
            // "pop_front"
            if(pool->conns[0].next!=nullptr) pool->conns[0].next->conns[0].prev = nullptr;
            auto out = pool;
            pool = pool->conns[0].next;
            return (void*)out;
        }
    }

    static void operator delete(void* p_in)
    {
            MultiList<T,N>* element = (MultiList<T,N>*)p_in;
            if(pool==nullptr)
            {
                pool = element;
                // remove connections to other lists
                for(int i=0;i<N;++i) {
                    pool->conns[i].next = nullptr;
                    pool->conns[i].prev = nullptr;
                }
            }
            else
            {
                // "push_front" on list zero
                pool->insert_before<0>(*element);
                for(int i=1;i<N;++i) {
                    pool->conns[i].next = nullptr;
                    pool->conns[i].prev = nullptr;
                }
            }
    }

private:

    static constexpr size_t pool_size = 10;
    static MultiList<T,N>* pool;
    static size_t news;
    static size_t dels;
};

template<typename T, size_t N> MultiList<T,N>* MultiList<T,N>::pool = nullptr;
template<typename T, size_t N> size_t          MultiList<T,N>::news = 0;
template<typename T, size_t N> size_t          MultiList<T,N>::dels = 0;

// convenience template alias
template<typename T> using DualList = MultiList<T,2>;
