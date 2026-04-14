#pragma once

#include <EbbGlow/Core/Message.h>
#include <EbbGlow/UI/Button/ButtonMsg.h>

namespace ebbglow::components
{
	struct MessageCallback
	{
		core::MessageManager* msgMgr;
		std::vector<std::function<void(core::MessageBase*)>> callbacks;
	};

	class MessageCallbackSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<MessageCallback>* coms;

	public:
		MessageCallbackSystem(core::DoubleComs<MessageCallback>* coms) : coms(coms){}
		void update() override;
	};
}