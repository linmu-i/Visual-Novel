#include <EbbGlow/VisualNovel/UI/UIState.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/VisualNovel/UI/Log.h>

namespace ebbglow::visualnovel
{
	void UIStateSystem::update()
	{
		state->active()->forEach([this](core::entity id, UIState& act)
			{
				auto& ina = *state->inactive()->get(id);
				if (act.logActive && input::MouseWheelDelta() > 0)
				{
					ina.retScene = scLoader->sceneName;
					ina.logActive = false;
					//world->createUnit(world->getEntityManager()->getId(), LogCom());
					
				}
			});
	}
}