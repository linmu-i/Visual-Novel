#include <EbbGlow/UI/Button.h>
#include <EbbGlow/VisualNovel/VisualNovel/JumpAttachment.h>

namespace ebbglow::visualnovel
{
	void JumpAttachmentSystem::update()
	{
		coms->active()->forEach([this](core::entity id, JumpAttachmentCom& com)
			{
				auto* msgList = msgMgr->getMessageList(id);
				for (auto& msg : *msgList)
				{
					if (msg->getType() == msgMgr->getMessageTypeManager().getId<ui::ButtonReleaseMsg>())
					{
						auto it = scLoader->sceneView.find(com.sceneName);
						if (it != scLoader->sceneView.end())
						{
							auto sceneIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
							scLoader->loadScene(sceneIt);
						}
					}
				}
			});
	}
}