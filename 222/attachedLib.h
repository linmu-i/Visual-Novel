#pragma once

#include <string>
inline uint64_t strHash(const std::string& str)
{
	constexpr uint64_t offset_basis = 0xCBF29CE484222325ui64;
	constexpr uint64_t prime = 0x100000001B3ui64;

	uint64_t hash = offset_basis;
	for (auto it = str.begin(); it != str.end(); it++)
	{
		hash ^= static_cast<uint8_t>(*it);
		hash *= prime;
	}
	return hash;
}

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <future>

class ThreadPool
{
private:
	size_t size;
	bool stop;

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;

	std::condition_variable condition;
	std::condition_variable waitCondition;
	std::mutex threadMutex;

	std::atomic<size_t> activeCount;

public:

	ThreadPool(size_t size = std::max(std::thread::hardware_concurrency(), unsigned int(1))) : size(size), stop(false), activeCount(0)
	{
		for (size_t i = 0; i < size; i++)
		{
			workers.push_back(std::thread([this]()
				{
					while (true)
					{
						std::function<void()> task;
						{
							std::unique_lock lock(threadMutex);

							condition.wait(lock, [this]() {return stop || !tasks.empty(); });
							if (stop && tasks.empty())
							{
								return;
							}
							task = std::move(tasks.front());
							tasks.pop();
						}
						++activeCount;
						task();
						--activeCount;
						{
							std::lock_guard lock(threadMutex);
							if (activeCount == 0 && tasks.empty())
							{
								waitCondition.notify_all();
							}
						}
					}
				}));
		}
	}
	~ThreadPool()
	{
		{
			std::lock_guard lock(threadMutex);
			stop = true;
		}
		condition.notify_all();
		for (auto& worker : workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
	}
	template<typename Func, typename ...Args>
	auto enqueue(Func&& func, Args&& ...args) -> std::future<decltype(func(args...))>
	{
		using return_type = decltype(func(args...));
		auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			[func = std::forward<Func>(func),
			args_tuple = std::move(argsTuple)]() mutable
			{
				return std::apply(func, args_tuple);
			}
		);

		std::future<return_type> result = task->get_future();

		{
			std::lock_guard lock(threadMutex);
			tasks.emplace([task]() { (*task)(); });
		}
		condition.notify_one();
		return result;
	}
	void wait()
	{
		std::unique_lock lock(threadMutex);

		waitCondition.wait(lock, [this]() {return activeCount == 0 && tasks.empty(); });
	}
};

/*
#include <vector>
#include <stack>

template<typename T>
class reuseableArray
{
private:
	std::vector<size_t> prevVector;
	std::vector<T> dataVector;
	std::vector<size_t> nextVector;
	std::stack<size_t> inactiveStack;
	size_t activeBegin;
	size_t activeEnd;
	size_t activeCount;

public:
	reuseableArray() : activeBegin(SIZE_MAX), activeEnd(SIZE_MAX), activeCount(0) {}

	class iterator
	{
	private:
		size_t idx;
		reuseableArray& reVec;
	public:
		iterator(size_t i, reuseableArray& rV) : idx(i), reVec(rV) {}
		friend class reuseableArray;
		T& operator*()
		{
			return reVec.dataVector[idx];
		}
		const T& operator*() const
		{
			return reVec.dataVector[idx];
		}
		iterator operator++()
		{
			if (this->idx != SIZE_MAX)
			{
				idx = reVec.nextVector[idx];
			}
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp = *this;
			if (this->idx != SIZE_MAX)
			{
				idx = reVec.nextVector[idx];
			}
			return tmp;
		}
		iterator operator--()
		{
			if (idx == SIZE_MAX)
			{
				idx == reVec.activeEnd;
			}
			else
			{
				if (reVec.prevVector[idx] != SIZE_MAX)
				{
					idx = reVec.prevVector[idx];
				}
			}
			return *this;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			if (idx == SIZE_MAX)
			{
				idx = reVec.activeEnd;
			}
			else
			{
				if (reVec.prevVector[idx] != SIZE_MAX)
				{
					idx = reVec.prevVector[idx];
				}
			}
			return tmp;
		}
		bool operator==(const iterator iter)
		{
			return &(iter.reVec) == &(this->reVec) && iter.idx == this->idx;
		}
		bool operator!=(const iterator iter)
		{
			return !(*this == iter);
		}
	};

	reuseableArray& insert(const T& d)
	{
		if (!inactiveStack.empty())
		{
			size_t idx = inactiveStack.top();
			dataVector[idx] = d;
			if (nextVector[idx] != SIZE_MAX)
			{
				prevVector[nextVector[idx]] = idx;
			}
			else
			{
				activeEnd = idx;
			}
			if (prevVector[idx] != SIZE_MAX)
			{
				nextVector[prevVector[idx]] = idx;
			}
			else
			{
				activeBegin = idx;
			}
			inactiveStack.pop();
		}
		else if (inactiveStack.empty() && !dataVector.empty())
		{
			dataVector.push_back(d);
			prevVector.push_back(dataVector.size() - 2);
			nextVector.push_back(SIZE_MAX);
			nextVector[dataVector.size() - 2] = dataVector.size() - 1;
			activeEnd = dataVector.size() - 1;
		}
		else if (inactiveStack.empty() && dataVector.empty())
		{
			dataVector.push_back(d);
			prevVector.push_back(SIZE_MAX);
			nextVector.push_back(SIZE_MAX);
			activeBegin = 0;
		}
		activeCount++;
		return *this;
	}
	reuseableArray erase(const iterator iter)
	{
		if (nextVector[iter.idx] != SIZE_MAX && prevVector[iter.idx] != SIZE_MAX)
		{
			nextVector[prevVector[iter.idx]] = nextVector[iter.idx];
			prevVector[nextVector[iter.idx]] = prevVector[iter.idx];
		}
		else if (nextVector[iter.idx] == SIZE_MAX && prevVector[iter.idx] != SIZE_MAX)
		{
			nextVector[prevVector[iter.idx]] = SIZE_MAX;
			activeEnd = prevVector[iter.idx];
		}
		else if (nextVector[iter.idx] != SIZE_MAX && prevVector[iter.idx] == SIZE_MAX)
		{
			prevVector[nextVector[iter.idx]] = SIZE_MAX;
			activeBegin = nextVector[iter.idx];
		}
		else
		{
			activeBegin = SIZE_MAX;
			activeEnd = SIZE_MAX;
		}
		inactiveStack.push(iter.idx);
		activeCount--;
		dataVector[iter.idx].~T();
		return *this;
	}
	void clear()
	{
		dataVector.clear();
		prevVector.clear();
		nextVector.clear();
		activeBegin = SIZE_MAX;
		activeEnd = SIZE_MAX;
		while (!inactiveStack.empty())
		{
			inactiveStack.pop();
		}
	}
	iterator begin()
	{
		return iterator(activeBegin, *this);
	}
	iterator end()
	{
		return iterator(SIZE_MAX, *this);
	}
	size_t size()
	{
		return activeCount;
	}
	bool empty()
	{
		return activeCount == 0ull;
	}
};*/