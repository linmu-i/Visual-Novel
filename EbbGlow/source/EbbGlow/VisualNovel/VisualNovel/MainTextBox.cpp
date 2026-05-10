#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Functions.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	MainTextBoxCom::MainTextBoxCom(Vec2 position, float width, const std::string& textL0, const std::string& textL1, const rsc::SharedFile& fontData,
		float textSize, float spacing, float lineSpacing, float speed, core::Layer* layer, VisualNovelConfig& cfg, ColorR8G8B8A8 textColor) :
		pos(position), font(utils::DynamicLoadFont(fontData, textL0 + textL1, textSize)), textSize(textSize), spacing(spacing),
		lineSpacing(lineSpacing), layer(layer), cfg(&cfg), textColor(textColor), l1OffsetY(0.0f), activePixels(0.0f), timeCount(0.0f),
		speed(speed)
	{
		float l0PixelCount = 0.0f;
		this->textL0 = utils::TextLineCalculateWithWordWrap(textL0, textSize, spacing, font, width);
		for (auto& line : this->textL0)
		{
			auto text = utils::ToUTF8Text(line);
			textL0Size.push_back(utils::MeasureTextSize(font, text, textSize, spacing));
			l0PixelCount += textL0Size.back().x;
			l1OffsetY += textL0Size.back().y;
			l1OffsetY += lineSpacing;
		}

		float l1PixelCount = 0.0f;
		this->textL1 = utils::TextLineCalculateWithWordWrap(textL1, textSize, spacing, font, width);
		for (auto& line : this->textL1)
		{
			auto text = utils::ToUTF8Text(line);
			textL1Size.push_back(utils::MeasureTextSize(font, text, textSize, spacing));
			l1PixelCount += textL1Size.back().x;
		}

		totalPixel = std::max(l0PixelCount, l1PixelCount);

		textL0ActiveRegion.reserve(this->textL0.size());
		textL1ActiveRegion.reserve(this->textL1.size());
	}

	void MainTextBoxDraw::draw()
	{
		Vec2 pos = com.pos;
		for (int32_t i = 0; i < com.textL0ActiveRegion.size(); ++i)
		{
			ScissorModeGuard guard(com.textL0ActiveRegion[i].offsetOf(com.cfg->virtualScreen.drawOffset));
			gfx::DrawTextCodepoints(com.font, com.textL0[i], pos, com.textSize, com.spacing);
			pos.y += com.textL0Size[i].y;
			pos.y += com.lineSpacing;
		}

		pos = com.pos;
		pos.y += com.l1OffsetY;
		for (int32_t i = 0; i < com.textL1ActiveRegion.size(); ++i)
		{
			ScissorModeGuard guard(com.textL1ActiveRegion[i].offsetOf(com.cfg->virtualScreen.drawOffset));
			gfx::DrawTextCodepoints(com.font, com.textL1[i], pos, com.textSize, com.spacing);
			pos.y += com.textL1Size[i].y;
			pos.y += com.lineSpacing;
		}
	}

	void MainTextBoxSystem::update()
	{
		textBoxs->active()->forEach([this](core::entity id, MainTextBoxCom& act) 
			{
				auto& ina = *textBoxs->inactive()->get(id);

				if (act.activePixels < act.totalPixel)
				{
					ina.timeCount += GetFrameTime();
				}
				else
				{
					ina.activePixels = act.totalPixel + 10.0f;
				}

				float newActPx = act.timeCount * act.speed * 2000;//基准速度2000px/s

				if (newActPx >= act.activePixels + 10.0f)
				{
					ina.activePixels = newActPx;
					
					float tmpPx = newActPx;
					Vec2 tmpPos = act.pos;

					ina.textL0ActiveRegion.clear();
					for (int32_t i = 0; i < act.textL0.size(); ++i)
					{
						Vec2 size = act.textL0Size[i];
						if (tmpPx >= size.x)
						{
							Rect rect{ tmpPos.x, tmpPos.y - size.y / 2.0f, size.x, size.y * 2.0f };
							ina.textL0ActiveRegion.push_back(rect);
							tmpPx -= size.x;

							if (tmpPx < 0.01f) break;
						}
						else
						{
							Rect rect{ tmpPos.x, tmpPos.y - size.y / 2.0f, tmpPx, size.y * 2.0f };
							ina.textL0ActiveRegion.push_back(rect);
							break;
						}
						tmpPos.y += size.y;
						tmpPos.y += act.lineSpacing;
					}

					tmpPx = newActPx;
					tmpPos = act.pos;
					tmpPos.y += act.l1OffsetY;

					ina.textL1ActiveRegion.clear();
					for (int32_t i = 0; i < act.textL1.size(); ++i)
					{
						Vec2 size = act.textL1Size[i];
						if (tmpPx >= size.x)
						{
							Rect rect{ tmpPos.x, tmpPos.y - size.y / 2.0f, size.x, size.y * 2.0f };
							ina.textL1ActiveRegion.push_back(rect);
							tmpPx -= size.x;

							if (tmpPx < 0.01f) break;
						}
						else
						{
							Rect rect{ tmpPos.x, tmpPos.y - size.y / 2.0f, tmpPx, size.y * 2.0f };
							ina.textL1ActiveRegion.push_back(rect);
							break;
						}
						tmpPos.y += size.y;
						tmpPos.y += act.lineSpacing;
					}
				}
				else if (act.activePixels >= act.totalPixel)
				{
					ina.textL0ActiveRegion.clear();
					Vec2 tmpPos = act.pos;
					for (int32_t i = 0; i < act.textL0.size(); ++i)
					{
						Vec2 size = act.textL0Size[i];
						Rect rect{ tmpPos.x, tmpPos.y - size.y / 2.0f, size.x, size.y * 2.0f };
						ina.textL0ActiveRegion.push_back(rect);
						tmpPos.y += size.y;
						tmpPos.y += act.lineSpacing;
					}

					ina.textL1ActiveRegion.clear();
					tmpPos = act.pos;
					tmpPos.y += act.l1OffsetY;
					for (int32_t i = 0; i < act.textL1.size(); ++i)
					{
						Vec2 size = act.textL1Size[i];
						Rect rect{ tmpPos.x, tmpPos.y - size.y / 2.0f, size.x, size.y * 2.0f };
						ina.textL1ActiveRegion.push_back(rect);
						tmpPos.y += size.y;
						tmpPos.y += act.lineSpacing;
					}
				}

				act.layer->push_back(std::make_unique<MainTextBoxDraw>(act));
			});
	}
}