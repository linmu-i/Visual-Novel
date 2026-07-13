#pragma once

#include <EbbGlow/UI/YUI/ScrollContainer/YUIScrollContainer.h>

namespace ebbglow::ui::yui
{
	struct ScrollContainerList
	{
		core::entity scId;
		std::vector<core::entity> itemIds;
		std::vector<float> itemHeights;
	};

	class ScrollContainerListSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ScrollContainerList>* scListPool;
		core::DoubleComs<ScrollContainer>* scPool;
		core::World2D* world;

		struct AddItemInfo
		{
			core::entity scListId;
			int index;
			float itemHeight;

		};

		std::vector<std::pair<core::entity, int>> toRemove;
		std::vector<AddItemInfo> toAdd;
	public:
		ScrollContainerListSystem(core::DoubleComs<ScrollContainerList>* scListPool, core::DoubleComs<ScrollContainer>* scPool, core::World2D& world)
			: scListPool(scListPool), scPool(scPool), world(&world) {
		}
		ScrollContainerListSystem(core::World2D& world) :
			scListPool(world.getDoubleBuffer<ScrollContainerList>()), scPool(world.getDoubleBuffer<ScrollContainer>()),
			world(&world){
		}
		void update() override;
		core::entity addItem(core::entity scListId, float itemHeight, int index = -1);
		void addItem(core::entity scListId, core::entity itemId, float itemHeight, int index = -1);

		core::entity removeItem(core::entity scListId, int index);
		void removeItem(core::entity scListId, core::entity itemId);
	};
}