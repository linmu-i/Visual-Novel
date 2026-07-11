#include <EbbGlow/UI/Button/MultiButton.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/UI/Button/ButtonMsg.h>

namespace ebbglow::ui
{
	void MultiButtonSystem::update()
	{
		coms->active()->forEach([this](core::entity id, MultiButtonCom& actList)
			{
				auto& inaList = *coms->inactive()->get(id);
				for (size_t i = 0; i < actList.buttons.size(); ++i)
				{
					auto& act = actList.buttons[i];
					auto& ina = inaList.buttons[i];

					bool press = false;
					bool release = false;
					int inBoxCount = 0;
					for (auto& p : input::PointList())
					{
						if (act.hitRect.contain(p.position))
						{
							if (p.type == input::PointType::Mouse)
							{
								if (input::MousePressed(input::MouseButton::Left))
								{
									press = true;
								}
								if (input::MouseReleased(input::MouseButton::Left))
								{
									release = true;
								}
							}
							else
							{
								if (input::PointPressed(p.id))
								{
									press = true;
								}
								if (input::PointReleased(p.id))
								{
									release = true;
								}
							}
							++inBoxCount;
						}
					}
					if (!act.pressed)
					{
						if (press && inBoxCount == 1)
						{
							ina.pressed = true;
							msgMgr->addUnicastMessage(std::make_unique<ButtonPressMsg>(id, pressId, i), id);
						}
					}
					else
					{
						if (inBoxCount == 0 || release)
						{
							ina.pressed = false;
							if (release) msgMgr->addUnicastMessage(std::make_unique<ButtonReleaseMsg>(id, releaseId, i), id);
						}
					}
					uint8_t icon;
					if (inBoxCount > 0)
					{
						if (act.pressed)
						{
							icon = 0;
						}
						else
						{
							icon = 1;
						}
					}
					else
					{
						icon = 2;
					}
					(*act.layer).push_back(std::make_unique<ButtonExDraw>(act, icon));
				}
			});
	}
}