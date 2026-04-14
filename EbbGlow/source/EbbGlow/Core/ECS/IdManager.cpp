#include <EbbGlow/Core/ECS/IdManager.h>

namespace ebbglow::core
{
	uint64_t IdManager::getId()
	{
		if (inactive.empty())
		{
			return countId++;
		}
		else
		{
			uint64_t tmp = *inactive.begin();
			inactive.erase(inactive.begin());
			return tmp;
		}
	}

	void IdManager::recycleId(uint64_t id)
	{
		inactive.insert(id);
	}

	size_t IdManager::size() const
	{
		return countId - inactive.size();
	}
}
