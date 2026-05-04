#pragma once

#include <EbbGlow/UI/Button/ButtonEx.h>
#include <EbbGlow/Components/Children.h>
#include <EbbGlow/VisualNovel/ScriptLoader/SaveLoad.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>

namespace ebbglow::visualnovel
{
	struct SLBlock
	{
		Vec2 pos;
		rsc::SharedTexture2D image;
		static constexpr Vec2 imgOffset = { 9, 27 };
		
		Color color = 0x66ccff00;

		std::string attText;
		static constexpr Vec2 attTxtOffset = { 306, 9 };

		std::vector<std::vector<int>> textCodepoints;
		static constexpr Vec2 txtLine0Offset = { 306, 60 };
		static constexpr Vec2 txtLine1Offset = { 306, 120 };
		static constexpr float txtMaxLength = 400;

		std::string time;
		static constexpr Vec2 timeOffset = { 384, 9 };

		rsc::SharedFont font;
		static constexpr float textSize = 28;

		const VisualNovelConfig* cfg;
		core::Layer* layer;

		std::string indexStr;

		SLBlock(Vec2 pos, const rsc::SharedTexture2D& img, Color color, const std::string& attText, const std::vector<std::vector<int>>& textCodepoints,
			std::string time, const rsc::SharedFont& font, const VisualNovelConfig* cfg, core::Layer* layer, std::string indexStr) :
			pos(pos), image(img), color(color), attText(attText), textCodepoints(textCodepoints), time(time), font(font), cfg(cfg), layer(layer), indexStr(indexStr) {}
	};

	class SLBlockDraw : public core::DrawBase
	{
	private:
		const SLBlock& sl;

	public:
		SLBlockDraw(const SLBlock& sl) : sl(sl) {}
		void draw() override;
	};

	class SLBlockSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<SLBlock>* coms;

	public:
		SLBlockSystem(core::World2D* world) : coms(world->getDoubleBuffer<SLBlock>()) {}
		void update() override;
	};

	enum class SOrL : uint8_t
	{
		Save, Load
	};

	class SLControlSystem : public core::SystemBase
	{
	private:
		std::vector<std::optional<ArchiveInfo>> infoList;
		bool updateInfoListFlag;
		bool saveFlag;
		bool loadFlag;
		bool rebuildFlag;
		size_t beginIndex;
		ScriptLoader* scLoader;
		std::vector<core::entity> idList;

	public:
		SLControlSystem(ScriptLoader* scLoader) : scLoader(scLoader), updateInfoListFlag(false), saveFlag(false), loadFlag(false), rebuildFlag(false), beginIndex(0) {}
		void update() override;
		void registerUpdateInfoList() { updateInfoListFlag = true; }
		void registerBuildSave() { saveFlag = true; }
		void registerBuildLoad() { loadFlag = true; }
		void registerRebuild() { rebuildFlag = true; }
		void setBeginIndex(size_t index) { beginIndex = index; }
		std::vector<core::entity>& getIdList() { return idList; }
	};

	struct SaveAttachment
	{
		std::vector<std::optional<ArchiveInfo>>* infoList;
		std::filesystem::path filePath;
		uint64_t index;
	};

	class SaveAttachmentSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<SaveAttachment>* coms;
		ScriptLoader* scLoader;
		core::MessageManager* msgMgr;
		int32_t btReleaseId;
		SLControlSystem* slCtrlSys;

	public:
		SaveAttachmentSystem(core::DoubleComs<SaveAttachment>* coms, ScriptLoader* scLoader, core::MessageManager* msgMgr, SLControlSystem* slCtrlSys) :
			coms(coms), scLoader(scLoader), msgMgr(msgMgr), btReleaseId(msgMgr->getMessageTypeManager().getId<ui::ButtonReleaseMsg>()), slCtrlSys(slCtrlSys) {}
		void update() override;
	};

	struct LoadAttachment
	{
		std::vector<std::optional<ArchiveInfo>>* infoList;
		std::filesystem::path filePath;
		uint64_t index;
	};

	class LoadAttachmentSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<LoadAttachment>* coms;
		ScriptLoader* scLoader;
		core::MessageManager* msgMgr;
		int32_t btReleaseId;
		SLControlSystem* slCtrlSys;

	public:
		LoadAttachmentSystem(core::DoubleComs<LoadAttachment>* coms, ScriptLoader* scLoader, core::MessageManager* msgMgr, SLControlSystem* slCtrlSys) :
			coms(coms), scLoader(scLoader), msgMgr(msgMgr), btReleaseId(msgMgr->getMessageTypeManager().getId<ui::ButtonReleaseMsg>()), slCtrlSys(slCtrlSys) {
		}
		void update() override;
	};

	inline void ApplySLBlock(ScriptLoader& scLoader)
	{
		auto& world = scLoader.world;
		world.addPool<SLBlock>();
		world.addSystem(SLBlockSystem(&world));

		world.addSystem(SLControlSystem(&scLoader));

		world.addPool<SaveAttachment>();
		world.addSystem(SaveAttachmentSystem(world.getDoubleBuffer<SaveAttachment>(), &scLoader, world.getMessageManager(), world.getSystem<SLControlSystem>()));

		world.addPool<LoadAttachment>();
		world.addSystem(LoadAttachmentSystem(world.getDoubleBuffer<LoadAttachment>(), &scLoader, world.getMessageManager(), world.getSystem<SLControlSystem>()));
	}
}