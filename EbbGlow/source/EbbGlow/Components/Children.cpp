#include <EbbGlow/Components/Children.h>

namespace ebbglow::components
{
	ChildrenBlock::~ChildrenBlock()
	{
		if (world)
		{
			for (auto id : childrenIds)
			{
				world->deleteUnit(id);
			}
		}
	}

	void Children::add(core::entity id)
	{
		block->childrenIds.push_back(id);
	}

	void Children::remove(core::entity id)
	{
		block->childrenIds.erase(std::remove(block->childrenIds.begin(), block->childrenIds.end(), id), block->childrenIds.end());
	}
}