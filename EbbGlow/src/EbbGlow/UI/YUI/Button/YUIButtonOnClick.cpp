#include <EbbGlow/UI/YUI/Button/YUIButtonOnClick.h>

namespace ebbglow::ui::yui
{
	ButtonOnClickSystem::ButtonOnClickSystem(core::World2D& world)
		: onClicks(world.getDoubleBuffer<ButtonOnClick>()), logics(world.getDoubleBuffer<ButtonLogic>()), world(&world) {
	}

	void ButtonOnClickSystem::update()
	{
		if (!onClicks || !logics) return;
		onClicks->active()->forEach([this](core::entity entity, ButtonOnClick& onClick)
			{
				auto* logic = logics->active()->get(entity);
				if (logic && logic->isReleased)
				{
					if (onClick.callback)
					{
						onClick.callback(*world, entity);
					}
				}
			});
	}
}