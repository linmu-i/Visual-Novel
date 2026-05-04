#include <EbbGlow/UI/Button.h>
#include <EbbGlow/VisualNovel/VisualNovel/ButtonAttachment.h>

namespace ebbglow::visualnovel
{
	void ApplyButtonAttachment(core::World2D& world, ScriptLoader& scLoader)
	{
		world.addPool<JumpAttachmentCom>();
		world.addSystem(JumpAttachmentSystem(world.getDoubleBuffer<JumpAttachmentCom>(), world.getMessageManager(), &scLoader));
		RegisterButtonAttachment("JumpAtt", [](ScriptLoader& scLoader, core::entity id, const std::vector<std::string>& args)
			{
				if (args.size() < 1) return;
				scLoader.world.createUnit(id, JumpAttachmentCom{ GetString(args[0], scLoader) });
			});
		RegisterButtonAttachment("AddBackLog", [](ScriptLoader& scLoader, core::entity id, const std::vector<std::string>& args)
			{
				if (args.size() < 4) return;
				scLoader.world.createUnit(id,
					BackLogAttachmentCom{ scLoader.sceneName, args[0], args[1], args[2], GetString(args[3], scLoader) });
			});
	}

	static std::unordered_map<std::string, ButtonAttCB_t>& ButtonAttachmentCreator()
	{
		static std::unordered_map<std::string, ButtonAttCB_t> functions;
		return functions;
	}

	void RegisterButtonAttachment(const std::string& name, const ButtonAttCB_t& callback)
	{
		ButtonAttachmentCreator().emplace(name, callback);
	}

	void CreateButtonAttachment(ScriptLoader& scLoader, core::entity id, const Command& cmd)
	{
		auto it = ButtonAttachmentCreator().find(cmd.name);
		if (it == ButtonAttachmentCreator().end()) return;
		it->second(scLoader, id, cmd.args);
	}

	void JumpAttachmentSystem::update()
	{
		coms->active()->forEach([this](core::entity id, JumpAttachmentCom& com)
			{
				auto* msgList = msgMgr->getMessageList(id);
				for (auto& msg : *msgList)
				{
					if (msg->getType() == buttonReleaseId)
					{
						auto it = scLoader->sceneView.find(com.sceneName);
						if (it != scLoader->sceneView.end())
						{
							auto sceneIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
							scLoader->loadScene(sceneIt);
							break;
						}
					}
				}
			});
	}

	void BackLogAttachmentSystem::update()
	{
		coms->active()->forEach([this](core::entity id, BackLogAttachmentCom& com)
			{
				auto* msgList = msgMgr->getMessageList(id);
				for (auto& msg : *msgList)
				{
					if (msg->getType() == msgMgr->getMessageTypeManager().getId<ui::ButtonReleaseMsg>())
					{
						scLoader->addToBackLog(scLoader->backLogTmp);
						scLoader->backLogTmp = com.backLog;
						break;
					}
				}
			});
	}
}