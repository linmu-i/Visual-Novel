#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	void ApplyButtonAttachment(core::World2D& world, ScriptLoader& scLoader);

	using ButtonAttCB_t = std::function<void(ScriptLoader&, core::entity, const std::vector<std::string>&)>;

	void RegisterButtonAttachment(const std::string& name, const ButtonAttCB_t& callback);
	void CreateButtonAttachment(ScriptLoader& world, core::entity id, const Command& cmd);

	struct JumpAttachmentCom
	{
		std::string sceneName;
		JumpAttachmentCom(std::string_view sceneName) : sceneName(sceneName) {}
	};

	class JumpAttachmentSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<JumpAttachmentCom>* coms;
		core::MessageManager* msgMgr;
		core::MessageTypeId buttonReleaseId;
		ScriptLoader* scLoader;
	public:
		JumpAttachmentSystem(core::DoubleComs<JumpAttachmentCom>* coms, core::MessageManager* msgMgr, ScriptLoader* scLoader) :
			coms(coms), msgMgr(msgMgr), scLoader(scLoader), buttonReleaseId(msgMgr->getMessageTypeManager().getId<ui::ButtonReleaseMsg>()) {}
		void update() override;
	};

	struct BackLogAttachmentCom
	{
		BackLogView backLog;
		BackLogAttachmentCom(std::string_view sceneName, std::string_view text, std::string_view exText, std::string_view attText, std::string_view voice) :
			backLog(std::string(sceneName), std::string(text), std::string(exText), std::string(voice), std::string(attText)) {
		}
	};

	class BackLogAttachmentSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<BackLogAttachmentCom>* coms;
		core::MessageManager* msgMgr;
		core::MessageTypeId buttonReleaseId;
		ScriptLoader* scLoader;

	public:
		BackLogAttachmentSystem(core::DoubleComs<BackLogAttachmentCom>* coms, core::MessageManager* msgMgr, ScriptLoader* scLoader) :
			coms(coms), msgMgr(msgMgr), scLoader(scLoader), buttonReleaseId(msgMgr->getMessageTypeManager().getId<ui::ButtonReleaseMsg>()) {
		}

		void update() override;
	};
}