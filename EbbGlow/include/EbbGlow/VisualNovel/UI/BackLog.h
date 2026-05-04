#pragma once

#include <EbbGlow/Core/ECS/SubSystem.h>
#include <EbbGlow/UI/UI.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	struct Item
	{
		std::string text;
		std::string exText;
		std::string voicePath;
		std::string sceneName;

		Rect region;
		const VisualNovelConfig* cfg;
		rsc::SharedFont font;

		Item(const std::string& text, const std::string& exText, const std::string& voicePath, const std::string& sceneName, Rect region, const VisualNovelConfig& cfg)
			: text(text), exText(exText), voicePath(voicePath), sceneName(sceneName), region(region), cfg(&cfg),
			font(utils::DynamicLoadFont(cfg.fontData, text + exText + '.', cfg.textSize)) {
		}
	};

	void DrawItem(const Item& item, Vec2 offset) noexcept;

	struct BackLogCom
	{
		float wheelDeltaCount;
		int32_t index;

		bool animationUp;
		bool animationDown;
		float animationTime;

		float drawOffsetY;

		std::vector<core::entity> voiceButton;
		std::vector<core::entity> jumpButton;
		std::vector<Item> items;

		core::Layer* layer;

		std::string returnName;

		BackLogCom(const VisualNovelConfig& cfg, core::Layer* layer, std::string_view returnName, ScriptLoader& scLoader);
	};

	class BackLogDraw : public core::DrawBase
	{
	private:
		const BackLogCom& com;
		const VisualNovelConfig& cfg;

	public:
		BackLogDraw(const BackLogCom& com, const VisualNovelConfig& cfg) : com(com), cfg(cfg) {}
		void draw() override;
	};

	class BackLogSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<BackLogCom>* coms;
		ScriptLoader* scLoader;

	public:
		BackLogSystem(ScriptLoader* scLoader) :
			scLoader(scLoader), world(&scLoader->world),
			coms(scLoader->world.getDoubleBuffer<BackLogCom>()) {}
		

		void update() override;
	};

	inline void ApplyLogView(core::World2D& world, ScriptLoader& scLoader)
	{
		world.addPool<BackLogCom>();
		world.addSystem(std::move(BackLogSystem(&scLoader)));
	}
}