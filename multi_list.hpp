#ifndef MULTI_LIST_HPP_
#define MULTI_LIST_HPP_

#include <iostream>
#include <stdexcept>
#include <array>
#include <memory>

template<typename T, size_t N>
struct MultiListElement
{

    // sanity check 
    static_assert(N>0,"MultiListElement cannot have \'arity\' paramter <1.");

    // forwarding constructor
    template<typename... Ts>
    MultiListElement(Ts... vs):data(vs...){}

    // non-copyable
    MultiListElement<T,N>(const MultiListElement<T,N>&) = delete;

    T data;

    // struct for storing connections
    struct Connection {
        MultiListElement<T,N>* prev;
        MultiListElement<T,N>* next;

        Connection()
        :prev(nullptr)
        ,next(nullptr)
        {
        }

        Connection(MultiListElement<T,N>* p, MultiListElement<T,N>* n)
        :prev(p)
        ,next(n)
        {
        }
    };

    // connections
    std::array<Connection,N> conns;

    // convenience functions
    template<size_t L>
    MultiListElement<T,N>* prev() const
    {
    	return conns[L].prev;
    }

    template<size_t L>
	MultiListElement<T,N>* next() const
	{
		return conns[L].next;
	}

    // run-time versions
   MultiListElement<T,N>* prev(size_t L) const
	{
	   return conns[L].prev;
	}

   	MultiListElement<T,N>* next(size_t L) const
   	{
   		return conns[L].next;
   	}

    // cast to data type
    operator T& ()
    {
        return data;
    }

    T* operator-> ()
    {
    	return &data;
    }

    // push onto specified list
    template<size_t L>
    MultiListElement<T,N>* insert_before(MultiListElement<T,N>* el) throw()
    {
        if(conns[L].prev!=nullptr) conns[L].prev->conns[L].next = el;
        el->conns[L].prev = conns[L].prev;
        el->conns[L].next = this;
        conns[L].prev = el;
        return el;
    }

    template<size_t L>
    MultiListElement<T,N>* insert_after(MultiListElement<T,N>* el) throw()
    {
        el->conns[L].next = conns[L].next;
        el->conns[L].prev = this;
        if(conns[L].next!=nullptr) conns[L].next->conns[L].prev = el;
        conns[L].next = el;
        return el;
    }

    template<size_t L>
    void insert_before(T&& t)
    {
        insert_before<L>(new MultiListElement<T,N>(t));
    }

    template<size_t L>
    void insert_after(T&& t)
    {
        insert_after<L>(new MultiListElement<T,N>(t));
    }

    /*
     * Custom memory allocation routines.
     */
    static void* operator new(size_t size) throw(std::bad_alloc)
    {        
        if(pool==nullptr)
        {
            // allocate contiguous storage            
            pool = new MultiListElement<T,N>[pool_size];

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
            MultiListElement<T,N>* element = (MultiListElement<T,N>*)p_in;
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
                pool->insert_before<0>(element);
                for(int i=1;i<N;++i) {
                    pool->conns[i].next = nullptr;
                    pool->conns[i].prev = nullptr;
                }
            }
    }

private:

    static constexpr size_t pool_size = 100;
    static MultiListElement<T,N>* pool;
    static size_t news;
    static size_t dels;
};

template<typename T, size_t N> MultiListElement<T,N>* MultiListElement<T,N>::pool = nullptr;
template<typename T, size_t N> size_t MultiListElement<T,N>::news = 0;
template<typename T, size_t N> size_t MultiListElement<T,N>::dels = 0;

template<typename T, size_t A, size_t N>
class MultiList
{

	// sanity checks
	static_assert(A>N,"Index out-of-bounds, N must be < A.");
	static_assert(A>0,"MultiList cannot have \'arity\' paramter <1.");

public:

	using element = MultiListElement<T,A>;

	class iterator
	{
		MultiListElement<T,A>* data;

	public:
		iterator():data(nullptr){}
		iterator(MultiListElement<T,A>* p):data(p){}
		bool operator!=(const iterator& i){ return data!=i.data; }
		T& operator*(){ return data->data; }

		MultiListElement<T,A>* get()
		{
			return data;
		}

		iterator& operator++()
		{
			data=data->template next<N>();
			return *this;
		}

		iterator& operator--()
		{
			data=data->template prev<N>();
			return *this;
		}
	}; // iterator

	MultiList()
	:start(nullptr)
	,rstart(nullptr)
	,count(0)
	{
	}

	void push_back(MultiListElement<T,A>* el)
	{
		if(count>0)
		{
			rstart = rstart->template insert_after<N>(el);
		}
		else
		{
			start = rstart = el;
		}
		++count;
	}

	iterator begin()
	{
		return iterator(start);
	}

	iterator end()
	{
		return iterator();
	}

private:

	MultiListElement<T,A>* start;
	MultiListElement<T,A>* rstart;
	size_t count;
};

// convenience template alias
template<typename T, size_t N> using DualList = MultiList<T,2,N>;
template<typename T> using DualElement = MultiListElement<T,2>;

#endif /* MULTI_LIST_HPP_ */
