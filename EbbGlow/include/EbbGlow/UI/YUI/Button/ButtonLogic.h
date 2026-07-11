#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/YUIBasic.h>

namespace ebbglow::ui::yui
{
	struct ButtonLogic
	{
		Rect buttonRect;
		bool isPressed;
		bool isHovered;
	};
}