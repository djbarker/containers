#include <vector>
#include <cstdlib>
#include <cassert>

namespace fast_erase
{
	template<class T, size_t BlockSize>
	class deque
	{
		typedef ::csph::deque<T, BlockSize> _deq_t;

	public:

		// iterator class
		class iterator {
		public:
			iterator(const iterator& itr)
				:_idx(itr._idx)
				, _deque(itr._deque)
			{
			}

			iterator& operator= (const iterator& itr) {
				if (&itr != this) {
					_idx = itr._idx;
					_deque = itr._deque;
				}
				return *this;
			}

			bool operator==(const iterator& itr) const {
				return (_idx == itr._idx && _deque == itr._deque);
			}

			bool operator!=(const iterator& itr) const {
				return !(*this == itr);
			}

			T& operator* () {
				return (*_deque)[_idx];
			}

			T* operator-> () {
				return &((*_deque)[_idx]);
			}

			operator T*() { // a bit of a hack for compatability
				return (this->operator->());
			}

			// pre
			iterator& operator++ () {
				_idx++;
				return *this;
			}

			// post
			iterator operator++ (int) {
				auto temp = *this;
				++(*this);
				return temp;
			}

		private:
			explicit iterator(size_t idx, _deq_t* data)
				:_idx(idx)
				, _deque(data)
			{
			}

			size_t _idx;
			_deq_t* _deque;
			friend _deq_t;
		};
		// end iterator

	public:

		deque()
			:num_stored(0)
			, lb_count(BlockSize)
		{
		}

		deque(size_t N)
			:num_stored(0)
			, lb_count(BlockSize)
		{
			for (size_t i = 0; i<N; ++i) push_back(T());
		}

		deque(const _deq_t& data)
			:num_stored(0)
			, lb_count(BlockSize)
		{
			for (size_t i = 0; i<data.num_stored; ++i) push_back(data[i]);
		}

		deque(_deq_t&& data)
		{
			blocks = std::move(data.blocks);

			lb_count = data.lb_count;
			num_stored = data.num_stored;
			data.num_stored = 0;
			data.lb_count = BlockSize;
		}

		~deque(){
			for (size_t i = 0; i<num_stored; ++i) (*this)[i].~T();
			for (size_t i = 0; i<blocks.size(); ++i) free(blocks[i]);
		}

		_deq_t& operator= (_deq_t&& data) {
			if (this != &data)	{
				// destruct anything we're currently holding and release the memory
				for (size_t i = 0; i<num_stored; ++i) (*this)[i].~T();
				for (T* block_start : blocks) free(block_start);
				
				blocks = std::move(data.blocks);

				lb_count = data.lb_count;
				num_stored = data.num_stored;
				data.num_stored = 0;
				data.lb_count = BlockSize;
			}
			return *this;
		}

		void push_back(const T& val){
			if (lb_count == BlockSize){
				blocks.push_back(static_cast<T*>(malloc(BlockSize*sizeof(T))));
				lb_count = 0;
			}
			new (blocks.back() + lb_count) T(val);
			++lb_count;
			++num_stored;
		}

		T& operator[] (size_t idx) {
			const size_t block_idx = (size_t)(idx / BlockSize);
			return blocks[block_idx][idx - block_idx*BlockSize];
		}

		void erase(size_t idx) {
			assert(idx >= 0 && idx<num_stored && "csph::deque<T,N>::erase(size_t) Invalid idx!");
			(*this)[idx].~T();
			memcpy(&(*this)[idx], &(*this)[num_stored - 1], sizeof(T));
			--num_stored;
			if (lb_count == 0) {
				_free_back(); 
				lb_count = BlockSize;
			}
			--lb_count;
		}

		iterator begin() { return iterator((size_t)0, this); }
		iterator end()   { return iterator(num_stored, this); }
		size_t size() const { return num_stored; }
		T& front() { return blocks[0][0]; }
		T& back()  { return blocks.back()[lb_count - 1]; }

		void clear() {
			for (size_t i = 0; i<num_stored; ++i) (*this)[i].~T();
			for (T* block_start : blocks) free(block_start);
			lb_count = BlockSize;
			num_stored = 0;
		}

		void resize(size_t N) {
			resize(N, T());
		}

		void resize(size_t N, const T& val) {
			if (N>num_stored) {
				for (size_t i = num_stored; i<N; ++i) push_back(val);
			}
			else {
				size_t num_before = num_stored;
				for (size_t i = N; i<num_before; ++i) erase(N);
			}
		}

		bool contains(T* ptr) const {
			bool out = false;
			for (size_t i = 0; i<blocks.size() - 1; ++i) out = out || (blocks[i] <= ptr && ptr<(blocks[i] + BlockSize));
			out = out || (blocks.back() <= ptr && ptr<(blocks.back() + lb_count));
			return out;
		}

		void erase(T* ptr) {
			assert(contains(ptr) && "csph::deque<T,N>::erase(T*) Invalid pointer!");
			erase(_ptr2idx(ptr));
		}

		void append(_deq_t& data) {
			if (data.num_stored == 0) return;

			num_stored += data.num_stored;
			size_t gap = (BlockSize - lb_count); // how much space in our last block

			// not enough elements in data's last block to fill our last block - just take everything
			if (data.lb_count<gap) {
				memcpy(blocks.back() + lb_count
					, data.blocks.back()
					, data.lb_count*sizeof(T));
				gap -= data.lb_count;
				lb_count += data.lb_count;
				data.num_stored -= data.lb_count;
				data.lb_count = BlockSize;
				data._free_back();
			}

			// fill our last block with elements from data's last block
			if (data.num_stored > 0) {
				memcpy(blocks.back() + lb_count
					, data.blocks.back() + data.lb_count - gap
					, gap*sizeof(T));
			}
			
			if (data.lb_count == gap) data._free_back();

			for (T* block_start : data.blocks) blocks.push_back(block_start);
			lb_count = data.lb_count - gap;

			data.blocks.clear(); // memory now owned by this
			data.lb_count = BlockSize;
			data.num_stored = 0;
		}

		_deq_t split(size_t idx) {
			assert(0 <= idx && idx < num_stored && "csph::deque<T,B>::split(size_t) Invalid index!");

			_deq_t out; // empty deque - nothing allocated
			size_t block = (size_t)idx / BlockSize;

			if (block != (blocks.size() - 1)) // we're splitting in the middle somewhere
			{
				// if the last block is not empty, store it for later
				_deq_t tmp;
				if (lb_count != 0) {
					tmp.blocks.push_back(blocks.back());
					blocks.pop_back();
					tmp.lb_count = lb_count;
					tmp.num_stored = lb_count;
					lb_count = BlockSize;
				}

				// give whole blocks after block to out and then disown them
				if (block != blocks.size() - 1) {
					out.blocks.insert(out.blocks.end(), blocks.begin() + block + 1, blocks.end());
					blocks.erase(blocks.begin() + block + 1, blocks.end());
				}

				// elements to copy from block
				out.blocks.push_back(_get_block());
				if (block != blocks.size() - 1) {
					size_t gap = BlockSize - (idx - block*BlockSize);
					memcpy(out.blocks.back(), blocks[block] + BlockSize - gap, gap*sizeof(T));
					lb_count = BlockSize - gap;
					out.lb_count = gap;
				}
				else {
					auto dest = out.blocks.back();
					auto src = blocks[block] + (idx - block*BlockSize);
					auto cnt = (lb_count - (idx - block*BlockSize))*sizeof(T);
					memcpy(dest, src, cnt);
					out.lb_count = (lb_count - (idx - block*BlockSize));
					lb_count = (idx - block*BlockSize);
				}

				if (lb_count == 0){
					_free_back();
					lb_count = BlockSize;
				}
				if (out.lb_count == 0) out.lb_count = BlockSize;

				out.append(tmp);
			} else { // splitting in last block
				out.blocks.push_back(_get_block());
				auto dest = out.blocks.back();
				auto src = blocks.back() + (idx - block*BlockSize);
				auto cnt = (lb_count - (idx - block*BlockSize))*sizeof(T);
				memcpy(dest, src, cnt);
				out.lb_count = (lb_count - (idx - block*BlockSize));
				lb_count = (idx - block*BlockSize);
			}

			// update counters
			out.num_stored = num_stored - idx;
			num_stored = idx;

			return out;
		}

	private:
		std::vector<T*> blocks; // pointers to allocated memory
		size_t lb_count;		// how many items in the last block
		size_t num_stored;		// how mnay items stored in total

		size_t _ptr2idx(T* ptr) const {
			for (size_t i = 0; i < blocks.size(); ++i)
			if (blocks[i] <= ptr && ptr < (blocks[i] + BlockSize))
				return (i*BlockSize) + (size_t)(ptr - blocks[i]);
			throw std::runtime_error("Invalid pointer in csph::deque<T,B>::_ptr2idx(T*)!");
		}

		void _free_back() {
			free(blocks.back());
			blocks.pop_back();
		}

		void _memcpy_back(T* data) { // data must point to a valid object which is then released without being destructed
			if (lb_count == BlockSize) {
				blocks.push_back(_get_block());
				lb_count = 0;
			}
			memcpy(blocks.back() + lb_count, data, sizeof(T));
			++num_stored;
			++lb_count;
		}

		static T* _get_block() {
			return static_cast<T*>(malloc(BlockSize*sizeof(T)));
		}
	};
}