#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/Button/YUIButtonLogic.h>
#include <EbbGlow/UI/YUI/Button/YUIFillButtonVisual.h>

namespace ebbglow::ui::yui
{
	inline void ApplyButton(core::World2D& world)
	{
		world.addPool<ButtonLogic>();
		world.addPool<FillButtonVisual>();

		world.addSystem(ButtonLogicSystem(world));
		world.addSystem(FillButtonVisualSystem(world));
	}
}