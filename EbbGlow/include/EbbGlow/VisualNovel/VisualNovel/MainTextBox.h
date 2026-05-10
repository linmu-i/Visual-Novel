#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	struct MainTextBoxCom
	{
		Vec2 pos;

		std::vector<std::vector<int32_t>> textL0;
		std::vector<std::vector<int32_t>> textL1;
		std::vector<Vec2> textL0Size;
		std::vector<Vec2> textL1Size;

		std::vector<Rect> textL0ActiveRegion;
		std::vector<Rect> textL1ActiveRegion;

		rsc::SharedFont font;
		ColorR8G8B8A8 textColor;

		float textSize;
		float spacing;
		float lineSpacing;
		float speed;

		float activePixels;

		float totalPixel;
		float l1OffsetY;
		float timeCount;

		core::Layer* layer;
		VisualNovelConfig* cfg;

		MainTextBoxCom() : textSize(0.0f), spacing(0.0f), lineSpacing(0.0f), speed(0.0f), activePixels(0.0f), totalPixel(0.0f),
			l1OffsetY(0.0f), layer(nullptr), cfg(nullptr), timeCount(0.0f) {}

		MainTextBoxCom(Vec2 position, float width, const std::string& textL0, const std::string& textL1, const rsc::SharedFile& fontData,
			float textSize, float spacing, float lineSpacing, float speed, core::Layer* layer, VisualNovelConfig& cfg, ColorR8G8B8A8 textColor);
	};

	class MainTextBoxDraw : public core::DrawBase
	{
	private:
		MainTextBoxCom& com;

	public:
		MainTextBoxDraw(MainTextBoxCom& com) : com(com) {}
		void draw() override;
	};

	class MainTextBoxSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<MainTextBoxCom>* textBoxs;
		rsc::SharedRenderTexture2D textureTmp;
		const VisualNovelConfig& cfg;

	public:
		MainTextBoxSystem(core::DoubleComs<MainTextBoxCom>* textBoxs, const VisualNovelConfig& cfg) : textBoxs(textBoxs), textureTmp(rsc::SharedRenderTexture(cfg.virtualScreen.width, cfg.virtualScreen.height)), cfg(cfg) {}

		void update() override;

		void skip(core::entity id)
		{
			auto& com = *textBoxs->inactive()->get(id);
			com.activePixels = com.totalPixel;
		}
	};

	inline void ApplyMainTextBox(core::World2D& world, const VisualNovelConfig& cfg)
	{
		world.addPool<MainTextBoxCom>();
		world.addSystem(MainTextBoxSystem(world.getDoubleBuffer<MainTextBoxCom>(), cfg));
	}
}