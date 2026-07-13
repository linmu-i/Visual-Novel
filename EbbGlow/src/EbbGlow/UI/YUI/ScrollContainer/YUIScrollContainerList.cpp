#include <EbbGlow/UI/YUI/ScrollContainer/YUIScrollContainerList.h>

namespace ebbglow::ui::yui
{
	void ScrollContainerListSystem::update()
	{
		scListPool->active()->forEach([this](core::entity id, ScrollContainerList& act)
			{
				auto& ina = *scListPool->inactive()->get(id);
				ina = act;

				auto scPtr = scPool->inactive()->get(act.scId);
				if (!scPtr) return;

				float totalHeight = 0.0f;
				for (float h : ina.itemHeights)
				{
					totalHeight += h;
				}
				scPtr->minOffset = std::min(0.0f, -totalHeight + scPtr->height);

			});
	}
	core::entity ScrollContainerListSystem::addItem(core::entity scListId, float itemHeight, int index)
	{
		auto scListPtr = scListPool->active()->get(scListId);
		if (!scListPtr) return core::InvalidEntity;

		auto scPtr = scPool->active()->get(scListPtr->scId);
		if (!scPtr) return core::InvalidEntity;

		float targetY = 0.0f;
		if (index < 0 || index >= scListPtr->itemHeights.size())
		{
			for (auto h : scListPtr->itemHeights)
			{
				targetY += h;
			}
		}
		else
		{
			for (int i = 0; i < index; ++i)
			{
				targetY += scListPtr->itemHeights[i];
			}
		}

		

		auto itemId = world->getEntityManager()->getId();
		toAdd.push_back({ scListId, index, itemHeight });
		TransformCom itemTrans{ Transform{ { 0, targetY }, { 0, 0 }, 0.0f, 1.0f }, {} };
		auto& panelTrans = *world->getDoubleBuffer<TransformCom>()->active()->get(scPtr->panelId);

		TransformAttachTo(itemTrans, panelTrans, scPtr->panelId);
		world->createUnit(itemId, itemTrans);
		return itemId;
	}
	void ScrollContainerListSystem::addItem(core::entity scListId, core::entity itemId, float itemHeight, int index)
	{

	}

	core::entity ScrollContainerListSystem::removeItem(core::entity scListId, int index)
	{
		return core::InvalidEntity;
	}
	void ScrollContainerListSystem::removeItem(core::entity scListId, core::entity itemId)
	{

	}
}