#include <EbbGlow/VisualNovel/UI/UIState.h>
#include <EbbGlow/Utils/Input.h>

namespace ebbglow::visualnovel
{
	void UIStateSystem::update()
	{
		state->active()->forEach([this](core::entity id, UIState& act)
			{
				auto& ina = *state->inactive()->get(id);
				if (act.logActive && input::MouseWheelDelta() > 0)
				{
					{
						auto it = scLoader->sceneView.find(scLoader->backLogScene);
						if (it != scLoader->sceneView.end())
						{
							scLoader->backLogRetName = scLoader->sceneName;
							auto logViewSceneIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
							scLoader->loadScene(logViewSceneIt);
							
							
							ina.logActive = false;
						}
					}
				}
			});
	}
}