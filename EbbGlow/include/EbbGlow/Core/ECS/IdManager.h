#pragma once
#include <EbbGlow/Core/ECS/ECSType.h>
#include <set>

namespace ebbglow::core
{
	class IdManager
	{
	private:
		uint64_t countId = 0;
		std::set<entity> inactive;
	public:
		uint64_t getId();
		void recycleId(uint64_t id);
		size_t size() const;
	};
}