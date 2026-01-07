#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/VisualNovel/UI/Log.h>

namespace ebbglow::visualnovel
{
	void ItemDraw::draw()
	{
		auto& cfg = com.cfg;
		auto texts = utils::TextLineCalculateWithWordWrap(com.text, cfg.textSize, cfg.textSize * 0.1f, com.font, cfg.ScreenWidth * 0.6666667f);
		auto exText = utils::TextLineCalculateWithWordWrap(com.exText, cfg.textSize, cfg.textSize * 0.1f, com.font, cfg.ScreenWidth * 0.6666667f);
		if (exText.size() > 1)
		{
			auto tmp = utils::ToCodepoints(".");
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
		}
		gfx::DrawTextCodepoints(com.font, exText[0], com.region.position(), cfg.textSize, cfg.textSize * 0.1f);
		float heightCount = 0.0f;
		float spacing = com.region.height * 0.08;
		if (!exText.empty())
		{
			heightCount += utils::MeasureTextSize(com.font, utils::ToUTF8Text(exText[0]), cfg.textSize, cfg.textSize * 0.1f).y;
			gfx::DrawRectangleGradientH
			(
				Rect{ com.region.x, com.region.y + spacing + heightCount - 0.8f, com.region.x + cfg.ScreenWidth * 0.3333333f, com.region.y + spacing + heightCount + 0.8f },
				ColorR8G8B8A8(255, 255, 255, 255),
				ColorR8G8B8A8(0,0,0,0)
			);
			heightCount += spacing * 2;
		}
		for (int i = 0; i < texts.size(); ++i)
		{
			float textHeight = utils::MeasureTextSize(com.font, utils::ToUTF8Text(texts[i]), cfg.textSize, cfg.textSize * 0.1f).y;
			if (heightCount + textHeight > com.region.height) break;
			if (heightCount + textHeight * 2 + spacing <= com.region.height)
			{
				gfx::DrawTextCodepoints(com.font, texts[i], Vec2{ com.region.x, com.region.y + heightCount }, cfg.textSize, cfg.textSize * 0.1f);
				heightCount += textHeight + spacing;
			}
			else
			{
				auto tmp = utils::ToCodepoints(".");
				texts[i].push_back(tmp.back());
				texts[i].push_back(tmp.back());
				texts[i].push_back(tmp.back());
				gfx::DrawTextCodepoints(com.font, texts[i], Vec2{ com.region.x, com.region.y + heightCount }, cfg.textSize, cfg.textSize * 0.1f);
				break;
			}
		}
	}

	void LogSystem::update()
	{

	}
}