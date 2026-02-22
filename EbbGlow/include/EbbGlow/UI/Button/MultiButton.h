#pragma once

#include <EbbGlow/Core/Message.h>
#include <EbbGlow/UI/Button/ButtonEx.h>

namespace ebbglow::ui
{
	struct MultiButtonCom
	{
		std::vector<ButtonExCom> buttons;
	};
	
	class MultiButtonSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<MultiButtonCom>* coms;

		core::MessageManager* msgMgr;

		core::MessageTypeId pressId;
		core::MessageTypeId releaseId;

	public:
		MultiButtonSystem(core::DoubleComs<MultiButtonCom>* coms, core::World2D& world) : coms(coms), msgMgr(world.getMessageManager()),
			pressId(world.getMessageManager()->getMessageTypeManager().getId<ButtonPressMsg>()), releaseId(world.getMessageManager()->getMessageTypeManager().getId<ButtonReleaseMsg>()) {}
		void update() override;
	};

	inline void ApplyMultiButton(core::World2D& world)
	{
		world.addPool<MultiButtonCom>();
		world.addSystem(MultiButtonSystem(world.getDoubleBuffer<MultiButtonCom>(), world));
	}
}