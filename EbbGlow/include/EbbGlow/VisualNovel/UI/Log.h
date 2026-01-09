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
		float scale;
		const VisualNovelConfig* cfg;
		rsc::SharedFont font;

		Item(const std::string& text, const std::string& exText, const std::string& voicePath, const std::string& sceneName, Rect region, float scale, const VisualNovelConfig& cfg)
			: text(text), exText(exText), voicePath(voicePath), sceneName(sceneName), region(region), scale(scale), cfg(&cfg) {
		}
	};

	class ItemDraw : public core::DrawBase
	{
	private:
		const Item& com;

	public:
		ItemDraw(const Item& com) : com(com) {}
		void draw() override;
	};

	void DrawItem(const Item& item) noexcept;

	struct LogCom
	{
		float wheelDeltaCount;
		uint32_t index;

		bool animationUp;
		bool animationDown;
		float animationTime;

		rsc::SharedRenderTexture2D textureBuf;
		std::vector<core::entity> voiceButton;
		std::vector<core::entity> jumpButton;
		std::vector<Item> items;
	};

	class LogDraw : public core::DrawBase
	{
	private:
		const LogCom& com;

	public:
		LogDraw(const LogCom& com) : com(com) {}
		void draw() override;
	};

	class LogSystem : public core::SystemBase
	{
	private:
		core::World2D* world;
		core::DoubleComs<LogCom>* coms;
		ScriptLoader* scLoader;

		core::Layer layerBuf;
		
		core::SubSystem<ui::ButtonExCom, ui::ButtonExSystem> buttonSubSys;

	public:
		LogSystem(ScriptLoader* scLoader) :
			scLoader(scLoader), world(&scLoader->world), coms(scLoader->world.getDoubleBuffer<LogCom>()), buttonSubSys(scLoader->world) {}
		

		void update() override;
	};
}