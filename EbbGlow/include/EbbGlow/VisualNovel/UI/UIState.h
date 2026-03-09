#pragma once
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	struct UIState
	{

		bool logActive;
		//std::string retScene;
	};

	class UIStateSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<UIState>* state;
		ScriptLoader* scLoader;
		core::World2D* world;
		VisualNovelConfig* cfg;

	public:
		UIStateSystem(core::DoubleComs<UIState>* state, ScriptLoader& scLoader, core::World2D& world, VisualNovelConfig& cfg) :
			state(state), scLoader(&scLoader), world(&world), cfg(&cfg) {}
		void update() override;

		void setLogActive(bool active)
		{
			state->active()->forEach([active, this](core::entity id, UIState& act)
				{
					auto& ina = *state->inactive()->get(id);
					scLoader->backLogRetName = scLoader->sceneName;
					ina.logActive = active;
				});
		}
	};

	inline void ApplyUIState(core::World2D& world, VisualNovelConfig& cfg, ScriptLoader& scLoader)
	{
		world.addPool<UIState>();
		world.createUnit(world.getEntityManager()->getId(), UIState{ true });
		world.addSystem(UIStateSystem(world.getDoubleBuffer<UIState>(), scLoader, world, cfg));
	}
}