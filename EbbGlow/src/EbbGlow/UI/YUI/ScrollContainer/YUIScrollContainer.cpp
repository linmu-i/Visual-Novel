#include <EbbGlow/UI/YUI/ScrollContainer/YUIScrollContainer.h>
#include <EbbGlow/Utils/Input.h>

namespace ebbglow::ui::yui
{
	void ScrollContainerSystem::update()
	{
		scPool->active()->forEach([this](core::entity id, ScrollContainer& act)
			{
				auto& ina = *scPool->inactive()->get(id);
				ina = act;

				auto& panelAct = *transPool->active()->get(act.panelId);
				auto& panelIna = *transPool->inactive()->get(act.panelId);
				panelIna = panelAct;

				if (!IsControlActive(controlPool->active(), act.panelId)) return;

				panelIna.transform.position.y = act.offset;

				Vec2 mousePos = input::MousePosition();
				if (!IsActivePoint(controlPool->active(), transPool->active(), act.panelId, mousePos)) return;
				ina.offset += input::MouseWheelDelta() * act.speed;
				ina.offset = std::clamp(ina.offset, ina.minOffset, ina.maxOffset);
			});
	}

	void ApplyScrollContainer(core::World2D& world)
	{
		world.addPool<ScrollContainer>();
		world.addSystem(ScrollContainerSystem(world));
	}
}