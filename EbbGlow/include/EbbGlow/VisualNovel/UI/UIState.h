#pragma once
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	struct UIState
	{

		bool logActive;
		std::string retScene;
	};

	class UIStateSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<UIState>* state;
		ScriptLoader* scLoader;
		core::World2D* world;

	public:
		UIStateSystem(core::DoubleComs<UIState>* state, ScriptLoader& scLoader, core::World2D& world) :
			state(state), scLoader(&scLoader), world(&world) {}
		void update() override;
	};
}