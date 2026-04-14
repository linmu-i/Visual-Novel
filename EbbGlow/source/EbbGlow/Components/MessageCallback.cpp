#include <EbbGlow/Components/MessageCallback.h>

namespace ebbglow::components
{
	void MessageCallbackSystem::update()
	{
		coms->active()->forEach([this](core::entity id, MessageCallback& act)
			{
				if (!act.msgMgr) return;
				auto* msgList = act.msgMgr->getMessageList(id);
				if (msgList)
				{
					for (auto* msg : *msgList)
					{
						for (auto& callback : act.callbacks)
						{
							callback(msg);
						}
					}
				}
			});
	}
}