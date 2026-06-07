#pragma once

#include <mutex>
#include <vector>

#include <EbbGlow/Core/ECS/ECSType.h>
#include <EbbGlow/Utils/ThreadPool.h>

namespace ebbglow::core
{
	class ComponentPoolBase
	{
	public:
		ComponentPoolBase() = default;
		virtual ComponentPoolBase& remove(const entity id) = 0;
		virtual ~ComponentPoolBase() = default;
	};

	template<typename T>
	class ComponentPool : public ComponentPoolBase
	{
	private:
		std::vector<T> dataVector;
		std::vector<entity> indexToEntity;
		std::vector<EntityIndexT> entityToIndex;
		mutable std::mutex mtx;

	public:
		ComponentPool() = default;

		ComponentPool& add(const entity id, const T& dat)
		{
			if (id.index() < entityToIndex.size() && entityToIndex[id.index()] != InvalidIndex)
			{
				dataVector[entityToIndex[id.index()]] = dat;
				return *this;
			}
			indexToEntity.push_back(id);
			dataVector.push_back(dat);
			if (id.index() >= entityToIndex.size())
			{
				entityToIndex.resize(static_cast<size_t>(id.index()) + 20, InvalidIndex);
			}
			entityToIndex[id.index()] = dataVector.size() - 1;
			return *this;
		}
		ComponentPool& add(const entity id, T&& dat)
		{
			if (id.index() < entityToIndex.size() && entityToIndex[id.index()] != InvalidIndex)
			{
				dataVector[entityToIndex[id.index()]] = std::move(dat);
				return *this;
			}
			indexToEntity.push_back(id);
			dataVector.push_back(std::move(dat));
			if (id.index() >= entityToIndex.size())
			{
				entityToIndex.resize(static_cast<size_t>(id.index()) + 20, InvalidIndex);
			}
			entityToIndex[id.index()] = dataVector.size() - 1;
			return *this;
		}
		ComponentPool& remove(const entity id) override
		{
			if (id.index() >= entityToIndex.size())
			{
				return *this;
			}
			if (entityToIndex[id.index()] == InvalidIndex)
			{
				return *this;
			}
			else
			{
				const size_t idx = entityToIndex[id.index()];
				const size_t lastIdx = dataVector.size() - 1;

				if (idx != dataVector.size() - 1)
				{
					entityToIndex[indexToEntity[lastIdx].index()] = idx;
					dataVector[idx] = std::move(dataVector[lastIdx]);
					indexToEntity[idx] = indexToEntity[lastIdx];
				}

				dataVector.pop_back();
				indexToEntity.pop_back();
				entityToIndex[id.index()] = InvalidIndex;

				return *this;
			}
		}

		//call func(id, Compornent, args...)
		template<typename Func, typename ...Args>
		void forEach(Func&& func, Args&& ...args)
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				func(indexToEntity[i], dataVector[i], args...);
			}
		}
		template<typename Func, typename ...Args>
		void forEachConst(Func&& func, Args&& ...args) const
		{
			for (size_t i = 0; i < dataVector.size(); i++)
			{
				func(indexToEntity[i], dataVector[i], args...);
			}
		}

		template<typename Func, typename ...Args>
		void forEach(utils::ThreadPool& tp, Func&& func, Args&& ...args)
		{
			size_t sectionSize = dataVector.size() / tp.size();
			std::vector<std::future<void>> result;
			for (size_t i = 0; i < tp.size(); i++)
			{
				size_t startIdx = i * sectionSize;
				size_t endIdx = (i == tp.size() - 1) ? dataVector.size() : (i + 1) * sectionSize;
				result.push_back(tp.enqueue([this, &func, &args..., startIdx, endIdx]()
					{
						for (size_t j = startIdx; j < endIdx; j++)
						{
							func(indexToEntity[j], dataVector[j], args...);
						}
					}));
			}
			for (auto& f : result)
			{
				f.get();
			}
		}
		template<typename Func, typename ...Args>
		void forEachConst(utils::ThreadPool& tp, Func&& func, Args&& ...args) const
		{
			size_t sectionSize = dataVector.size() / tp.size();
			std::vector<std::future<void>> result;
			for (size_t i = 0; i < tp.size(); i++)
			{
				size_t startIdx = i * sectionSize;
				size_t endIdx = (i == tp.size() - 1) ? dataVector.size() : (i + 1) * sectionSize;
				result.push_back(tp.enqueue([this, &func, &args..., startIdx, endIdx]()
					{
						for (size_t j = startIdx; j < endIdx; j++)
						{
							func(indexToEntity[j], dataVector[j], args...);
						}
					}));
			}
			for (auto& f : result)
			{
				f.get();
			}
		}

		T* get(const entity id)
		{
			if (id.index() >= entityToIndex.size())
			{
				return nullptr;
			}
			if (entityToIndex[id.index()] == InvalidIndex)
			{
				return nullptr;
			}
			else
			{
				return &dataVector[entityToIndex[id.index()]];
			}
		}
		const T* get(const entity id) const
		{
			if (id.index() >= entityToIndex.size())
			{
				return nullptr;
			}
			if (entityToIndex[id.index()] == InvalidIndex)
			{
				return nullptr;
			}
			else
			{
				return &dataVector[entityToIndex[id.index()]];
			}
		}
		size_t size() const
		{
			return (dataVector.size());
		}
		bool empty() const
		{
			return dataVector.empty();
		}
		//必须接收锁以维持
		std::unique_lock<std::mutex> getLock()
		{
			return std::unique_lock<std::mutex>(mtx);
		}
		void reserve(size_t size)
		{
			dataVector.reserve(size);
			indexToEntity.reserve(size);
		}
		void clear()
		{
			dataVector.clear();
			indexToEntity.clear();
			entityToIndex.clear();
		}
		void shrink_to_fit()//尽量不使用
		{
			dataVector.shrink_to_fit();
			indexToEntity.shrink_to_fit();
			while (!entityToIndex.empty() && entityToIndex.back() == InvalidIndex)
			{
				entityToIndex.pop_back();
			}
			entityToIndex.shrink_to_fit();
		}
		void sync(const ComponentPool<T>* other)//仅对数据进行同步，请配合双缓冲使用
		{
			dataVector = other->dataVector;
		}
	};
}