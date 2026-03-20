#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
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
		ScriptLoader* scLoader;
	public:
		JumpAttachmentSystem(core::DoubleComs<JumpAttachmentCom>* coms, core::MessageManager* msgMgr, ScriptLoader* scLoader) :
			coms(coms), msgMgr(msgMgr), scLoader(scLoader) {}
		void update() override;
	};

	inline void ApplyJumpAttachment(core::World2D& world, ScriptLoader& scLoader)
	{
		world.addPool<JumpAttachmentCom>();
		world.addSystem(JumpAttachmentSystem(world.getDoubleBuffer<JumpAttachmentCom>(), world.getMessageManager(), &scLoader));
	}
}