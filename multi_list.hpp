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
};

template<typename T, size_t Arity>
class MultiList
{
	// sanity check
	static_assert(Arity>0,"MultiList cannot have \'arity\' paramter <1.");

public:

	typedef MultiListElement<T,Arity> node_type;

	template<size_t N>
	class iterator
	{
		static_assert(N>=0 && N<Arity,"Iterator MultiList index out-of-bounds.");

		MultiListElement<T,Arity>* data;

	public:
		iterator():data(nullptr){}
		iterator(MultiListElement<T,Arity>* p):data(p){}
		bool operator!=(const iterator& i){ return data!=i.data; }
		T& operator*(){ return data->data; }

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

		MultiListElement<T,Arity>* get()
		{
			return data;
		}

		template<size_t NNew>
		iterator<NNew> project() // convert to an iterator over a different list
		{
			static_assert(NNew>0,"Index < 0 at \"template<size_t> iterator<size_t>::project()\".");
			static_assert(NNew<Arity,"Index out of bounds at \"template<size_t> iterator<size_t>::project()\".");
			return iterator<NNew>(data);
		}
	}; // iterator

	MultiList()
	{
		for(size_t i=0; i<Arity; ++i)
			start[i] = rstart[i] = nullptr;
	}

	template<size_t N>
	void push_back(const T& t)
	{
		if(count[N]>0)
		{
			start[N] = rstart[N]->template insert_after<N>(new node_type(t));
		}
		else
		{
			start[N] = rstart[N] = new node_type(t);
		}
		++count[N];
	}

	template<size_t NNew, size_t NExist>
	void push_back(const iterator<NExist>& itr)
	{
		static_assert(NExist!=NNew,"Cannot push_back existing element onto containing list.");
		if(count[NNew]>0)
		{
			rstart[NNew] = start[NNew]->template insert_after<NNew>(*itr);
		}
		else
		{
			start[NNew] = rstart[NNew] = *itr;
		}
		++count[NNew];
	}

	template<size_t N>
	T back()
	{
		return *rstart[N];
	}

	template<size_t N>
	iterator<N> begin()
	{
		return iterator<N>(start[N]);
	}

	template<size_t N>
	iterator<N> end()
	{
		return iterator<N>();
	}

	template<size_t N>
	iterator<N> last()
	{
		return iterator<N>(rstart[N]);
	}

	template<size_t N> friend class List;

	template<size_t N>
	class List {
	public:
		List(MultiList<T,Arity>& ml)
		:mlist(ml)
		{
		}

		iterator<N> begin()
		{
			return mlist.template begin<N>();
		}

		iterator<N> end()
		{
			return mlist.template end<N>();
		}

		iterator<N> last()
		{
			return mlist.template last<N>();
		}

	private:
		MultiList<T,Arity>& mlist;
	};

private:

	std::array<node_type*,Arity> start;
	std::array<node_type*,Arity> rstart;
	std::array<size_t,Arity> count;
};

#endif /* MULTI_LIST_HPP_ */
