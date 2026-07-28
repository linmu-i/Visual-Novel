#pragma once

#include <functional>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/Button/YUIButtonLogic.h>

namespace ebbglow::ui::yui
{
	struct ButtonOnClick
	{
		std::function<void(core::World2D&, core::entity)> callback;
	};

	class ButtonOnClickSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ButtonOnClick>* onClicks;
		core::DoubleComs<ButtonLogic>* logics;
		core::World2D* world;

	public:
		ButtonOnClickSystem(core::World2D& world);
		void update() override;
	};
}