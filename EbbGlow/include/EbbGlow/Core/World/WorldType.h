#pragma once

#include <array>

#include <EbbGlow/Core/ECS.h>
#include <EbbGlow/Core/Message.h>

namespace ebbglow::core
{
	class Layer
	{
	private:
		std::vector<std::unique_ptr<DrawBase>> storageArr;
		std::vector<DrawBase*> drawArr;

	public:
		Layer() {}
		Layer& push_back(std::unique_ptr<DrawBase>&& package)
		{
			storageArr.push_back(std::move(package));
			drawArr.push_back(storageArr.back().get());
			return *this;
		}
		Layer& push_back(DrawBase* package)
		{
			drawArr.push_back(package);
			return *this;
		}
		void clear() noexcept
		{
			storageArr.clear();
			drawArr.clear();
		}
		auto begin() noexcept { return drawArr.begin(); }
		auto end() noexcept { return drawArr.end(); }
	};

	using Layers = std::array<Layer, 16>;

	class AddBufferBase
	{
	protected:
		ComponentPoolBase* pool0;
		ComponentPoolBase* pool1;
	public:
		AddBufferBase(ComponentPoolBase* pool0, ComponentPoolBase* pool1) : pool0(pool0), pool1(pool1) {}
		virtual ~AddBufferBase() = default;
		virtual void addToPool() = 0;
		virtual void deleteUnit(core::entity id) = 0;
	};

	template<typename T>
	class AddBuffer : public AddBufferBase
	{
	private:
		std::vector<std::pair<entity, T>> buffer;

	public:
		AddBuffer(ComponentPoolBase* pool0, ComponentPoolBase* pool1) : AddBufferBase(pool0, pool1) {}
		template<typename U>
		void addBuffer(entity id, U&& data)
		{
			buffer.emplace_back(id, std::forward<U>(data));
		}

		void addToPool() override
		{
			ComponentPool<T>* p0 = static_cast<ComponentPool<T>*>(pool0);
			ComponentPool<T>* p1 = static_cast<ComponentPool<T>*>(pool1);
			for (auto& [id, data] : buffer)
			{
				p0->add(id, data);
				p1->add(id, std::move(data));
			}
			buffer.clear();
		}

		void deleteUnit(core::entity id) override
		{
			buffer.erase(std::remove_if(buffer.begin(), buffer.end(), [id](const std::pair<entity, T>& dat) {return dat.first == id; }), buffer.end());
		}
	};
}