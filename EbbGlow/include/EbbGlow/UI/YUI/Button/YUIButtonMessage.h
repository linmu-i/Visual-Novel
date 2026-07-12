#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Core/Message.h>

#include <EbbGlow/UI/YUI/Button/YUIButtonLogic.h>

namespace ebbglow::ui::yui
{
	struct ButtonMessage {};

	class ButtonMessageSystem : public core::SystemBase
	{
	private:
		core::MessageManager* messageManager;
		core::DoubleComs<ButtonLogic>* logics;
		core::DoubleComs<ButtonMessage>* messages;
		core::MessageTypeId pressMessageTypeId;
		core::MessageTypeId releaseMessageTypeId;

	public:
		ButtonMessageSystem(core::World2D& world);
		ButtonMessageSystem(core::MessageManager* messageManager, core::DoubleComs<ButtonLogic>* logics, core::DoubleComs<ButtonMessage>* messages);
		void update() override;
	};
}