#include <EbbGlow/UI/YUI/Button/YUIButtonLogic.h>

namespace ebbglow::ui::yui
{
	void ButtonLogicSystem::update()
	{
		buttonLogics->active()->forEach([this](core::entity id, ButtonLogic& act)
			{
				auto& ina = *buttonLogics->inactive()->get(id);
				ina = act;

				if (!IsControlActive(controls->active(), id))
				{
					ina.isHovered = false;
					ina.isPressed = false;
					ina.isReleased = false;
					ina.isDown = false;
					return;
				}

				auto points = GetActiveInputPoints(controls->active(), transforms->active(), id);
				
				bool allReleased = true;
				bool newPressed = false;

				if (!points.empty())
				{
					ina.isHovered = true;
				}
				else
				{
					ina.isHovered = false;
					ina.isPressed = false;
					ina.isReleased = false;
					ina.isDown = false;
					return;
				}

				if (act.isPressed)
				{
					ina.isPressed = false;
				}
				if (act.isReleased)
				{
					ina.isReleased = false;
				}

				for (auto& point : points)
				{

					if (point.type == input::PointType::Mouse)
					{
						if (!act.isDown && input::MousePressed(input::MouseButton::Left))
						{
							newPressed = true;

						}

						if (act.isDown && (!input::MouseReleased(input::MouseButton::Left) || input::MouseDown(input::MouseButton::Left)))
						{
							allReleased = false;
						}
					}
					else
					{
						if (!act.isDown && input::PointPressed(point.id))
						{
							newPressed = true;

						}

						if (act.isDown && (!input::PointReleased(point.id) || input::PointDown(point.id)))
						{
							allReleased = false;
						}
					}
				}

				if (newPressed)
				{
					ina.isPressed = true;
					ina.isDown = true;
				}
				if (allReleased && act.isDown)
				{
					ina.isReleased = true;
					ina.isDown = false;
				}
			});
	}
}