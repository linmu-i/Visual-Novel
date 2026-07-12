#include <EbbGlow/UI/YUI/Button/YUIButtonMessage.h>
#include <EbbGlow/UI/Button/ButtonMsg.h>

namespace ebbglow::ui::yui
{
	ButtonMessageSystem::ButtonMessageSystem(core::World2D& world)
		: messageManager(world.getMessageManager()),
		logics(world.getDoubleBuffer<ButtonLogic>()),
		messages(world.getDoubleBuffer<ButtonMessage>()),
		pressMessageTypeId(world.getMessageManager()->getMessageTypeManager().registeredType<ButtonPressMsg>()),
		releaseMessageTypeId(world.getMessageManager()->getMessageTypeManager().registeredType<ButtonReleaseMsg>())
	{
	}
	ButtonMessageSystem::ButtonMessageSystem(core::MessageManager* messageManager, core::DoubleComs<ButtonLogic>* logics, core::DoubleComs<ButtonMessage>* messages)
		: messageManager(messageManager),
		logics(logics),
		messages(messages),
		pressMessageTypeId(messageManager->getMessageTypeManager().registeredType<ButtonPressMsg>()),
		releaseMessageTypeId(messageManager->getMessageTypeManager().registeredType<ButtonReleaseMsg>())
	{
	}
	void ButtonMessageSystem::update()
	{
		messages->active()->forEach([this](core::entity id, ButtonMessage& msg)
			{
				auto& logic = *logics->active()->get(id);
				if (logic.isPressed)
				{
					messageManager->addUnicastMessage(std::make_unique<ButtonPressMsg>(id, pressMessageTypeId), id);
				}
				if (logic.isReleased)
				{
					messageManager->addUnicastMessage(std::make_unique<ButtonReleaseMsg>(id, releaseMessageTypeId), id);
				}
			});
	}
}