#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/YUIBasic.h>

namespace ebbglow::ui::yui
{
	struct ScrollContainer
	{
		core::entity panelId;
		float height = 0;
		float maxOffset = 0;
		float minOffset = 0;
		float offset = 0;
		float speed = 1.0;
	};

	class ScrollContainerSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ScrollContainer>* scPool;
		core::DoubleComs<TransformCom>* transPool;
		core::DoubleComs<ControlCom>* controlPool;

	public:
		ScrollContainerSystem(core::DoubleComs<ScrollContainer>* scPool, core::DoubleComs<TransformCom>* transPool, core::DoubleComs<ControlCom>* controlPool)
			: scPool(scPool), transPool(transPool), controlPool(controlPool) {}
		ScrollContainerSystem(core::World2D& world)
			: scPool(world.getDoubleBuffer<ScrollContainer>()), transPool(world.getDoubleBuffer<TransformCom>()), controlPool(world.getDoubleBuffer<ControlCom>()) {}

		void update() override;
	};

	void ApplyScrollContainer(core::World2D& world);
}