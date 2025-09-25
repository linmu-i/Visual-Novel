#pragma once

#include "UI.h"
#include "World.h"
#include "raylibRAII.h"

namespace galgame
{
	const Vector2 StandardTextBoxDefaultPosition = { GetScreenWidth() / 6, GetScreenHeight() / 3 * 2};

	struct StandardTextBox
	{
		bool drawing;

		rlRAII::FontRAII font;

		float timeCount;

		std::vector<rlRAII::Texture2DRAII> textL0;
		std::vector<rlRAII::Texture2DRAII> textL1;

		float textSize;
		float spacing;
		float textHeight;
		float lineSpacing;

		float totalHeightL0;
		float totalHeightL1;

		int totalPixel;
		int activePixel = 0;

		float speed;
		
		Vector2 pos;

		
		
		StandardTextBox(const std::string& textL0, const std::string& textL1, float textSize, rlRAII::FileRAII fontData, float speed, Vector2 pos, float width) :
			textSize(textSize), spacing(textSize * 0.1f), lineSpacing(textSize * 0.3f), speed(speed), timeCount(0.0f), drawing(true),
			font(DynamicLoadFontFromMemory((textL0 + textL1).c_str(), fontData.fileName(), fontData.get(), fontData.size(), textSize)),
			textHeight(MeasureTextEx(font.get(), textL0.c_str(), textSize, spacing).y), pos(pos)
		{
			std::vector<std::vector<int>> t0 = TextLineCaculateWithWordWrap(textL0.c_str(), textSize, spacing, font.get(), width);//主文本占2/3屏，1280px
			
			totalPixel = 0;

			for (auto& s : t0)
			{
				auto text = LoadUTF8(s.data(), s.size());
				Vector2 textureSize = MeasureTextEx(font.get(), text, textSize, spacing);
				RenderTexture tmpTexture = LoadRenderTexture(textureSize.x, textureSize.y);
				BeginTextureMode(tmpTexture);
				ClearBackground(BLANK);
				DrawTextCodepoints(font.get(), s.data(), s.size(), { 0,0 }, textSize, spacing, WHITE);
				EndTextureMode();
				this->textL0.push_back(rlRAII::Texture2DRAII(tmpTexture.texture));
				tmpTexture.texture = {};
				UnloadRenderTexture(tmpTexture);
				UnloadUTF8(text);
			}

			std::vector<std::vector<int>> t1 = TextLineCaculateWithWordWrap(textL1.c_str(), textSize, spacing, font.get(), width);

			for (auto& s : t1)
			{
				auto text = LoadUTF8(s.data(), s.size());
				Vector2 textureSize = MeasureTextEx(font.get(), text, textSize, spacing);
				RenderTexture tmpTexture = LoadRenderTexture(textureSize.x, textureSize.y);
				BeginTextureMode(tmpTexture);
				ClearBackground(BLANK);
				DrawTextCodepoints(font.get(), s.data(), s.size(), { 0,0 }, textSize, spacing, WHITE);
				EndTextureMode();
				this->textL1.push_back(rlRAII::Texture2DRAII(tmpTexture.texture));
				tmpTexture.texture = {};
				UnloadRenderTexture(tmpTexture);
				UnloadUTF8(text);
			}

			totalHeightL0 = textHeight * t0.size() + lineSpacing * t0.size();
			totalHeightL1 = textHeight * t1.size() + lineSpacing * t1.size();
			
			
			for (auto& t : this->textL0)
			{
				totalPixel += t.get().width;
			}
			for (auto& t : this->textL1)
			{
				totalPixel += t.get().width;
			}
		}
	};

	class StdTextBoxEnd : public ecs::MessageBase {};

	class StandardTextBoxDraw : public ecs::DrawBase
	{
	private:
		rlRAII::RenderTexture2DRAII texture;
		Vector2 pos;

	public:
		StandardTextBoxDraw(Vector2 position, rlRAII::RenderTexture2DRAII texture) : pos(position), texture(texture) {}
		void draw() override
		{
			DrawTextureRec(texture.get().texture, { 0, 0, float(texture.get().texture.width), -float(texture.get().texture.height) }, pos, WHITE);
			//DrawTextureV(texture.get().texture, { 500,500 }, WHITE);
		}
	};

	class StandardTextBoxSystem : public ecs::SystemBase
	{
	private:
		ecs::DoubleComs<StandardTextBox>* textBoxs;
		ecs::Layers* layers;
		int layerDepth;
		rlRAII::RenderTexture2DRAII textureTmp;

	public:
		StandardTextBoxSystem(ecs::DoubleComs<StandardTextBox>* textBoxs, ecs::Layers* layers, int layerDepth) : textBoxs(textBoxs), layers(layers), layerDepth(layerDepth) {}

		void update() override
		{
			textBoxs->active()->forEach
			(
				[this](ecs::entity id, StandardTextBox& comActive) 
				{
					auto& comInactive = *(textBoxs->inactive()->get(id));

					if (!comActive.drawing)
					{
						comActive.activePixel = comActive.totalPixel;
						comInactive.activePixel = comInactive.totalPixel;
					}

					int activePixelTmp = comActive.timeCount * comActive.speed * 2000;

					if (activePixelTmp - comActive.activePixel > 10)
					{
						comInactive.activePixel = activePixelTmp;
					}

					if (comActive.activePixel < comActive.totalPixel)
					{
						comInactive.timeCount += GetFrameTime();
						comActive.timeCount += GetFrameTime();
						//comInactive.activePixel = activePixelTmp;
					}
					
					
					if (activePixelTmp - comActive.activePixel > 10 && comActive.activePixel < comActive.totalPixel)
					{
						textureTmp = LoadRenderTexture(1280, int(comActive.totalHeightL0 + comActive.totalHeightL1));

						int activePixel0 = activePixelTmp;
						int activePixel1 = activePixelTmp;
						
						int spacing = comActive.textHeight + comActive.lineSpacing;
						BeginTextureMode(textureTmp.get());
						ClearBackground(BLANK);
						for (int activeLine = 0; activeLine < comActive.textL0.size() && activePixel0; ++activeLine)
						{
							if (activePixel0 >= comActive.textL0[activeLine].get().width)
							{
								DrawTextureRec(comActive.textL0[activeLine].get(), { 0, 0, float(comActive.textL0[activeLine].get().width), -float(comActive.textL0[activeLine].get().height) }, { 0, float(spacing * activeLine) }, WHITE);
								activePixel0 -= comActive.textL0[activeLine].get().width;
							}
							else
							{
								DrawTextureRec(comActive.textL0[activeLine].get(), { 0, 0, float(activePixel0), -float(comActive.textL0[activeLine].get().height) }, { 0, float(spacing * activeLine) }, WHITE);
								activePixel0 = 0;
							}
						}
						for (int activeLine = 0; activeLine < comActive.textL1.size() && activePixel1; ++activeLine)
						{
							if (activePixel1 >= comActive.textL1[activeLine].get().width)
							{
								DrawTextureRec(comActive.textL1[activeLine].get(), { 0, 0, float(comActive.textL1[activeLine].get().width), -float(comActive.textL1[activeLine].get().height) }, { 0, float(spacing * activeLine + spacing * comActive.textL0.size()) }, WHITE);
								activePixel1 -= comActive.textL1[activeLine].get().width;
							}
							else
							{
								DrawTextureRec(comActive.textL1[activeLine].get(), { 0, 0, float(activePixel1), -float(comActive.textL1[activeLine].get().height) }, { 0, float(spacing * activeLine + spacing * comActive.textL0.size()) }, WHITE);
								activePixel1 = 0;
							}
						}
						EndTextureMode();
						
					}

					(*layers)[layerDepth].push_back(std::make_unique<StandardTextBoxDraw>(comActive.pos, textureTmp));
				}
			);
		}
	};

	void ApplyStandardTextBox(ecs::World2D& world)
	{
		world.addPool<StandardTextBox>();
		world.addSystem(StandardTextBoxSystem(world.getDoubleBuffer<StandardTextBox>(), world.getUiLayer(), 10));
	}

	class SceneBase
	{
	public:
		virtual void update() = 0;
	};

	struct SceneIterator
	{
		std::vector<std::unique_ptr<SceneBase>>* sceneSequence;
		size_t index;
	};

	
}

namespace gal = galgame;