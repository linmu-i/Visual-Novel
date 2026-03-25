#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/VisualNovel/UI/BackLog.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/VisualNovel/UI/UIState.h>

namespace ebbglow::visualnovel
{
	void DrawItem(const Item& item) noexcept
	{
		auto& cfg = *item.cfg;
		auto texts = utils::TextLineCalculateWithWordWrap(item.text, cfg.textSize, cfg.textSize * 0.1f, item.font, cfg.virtualScreenWidth * 0.6666667f);
		auto exText = utils::TextLineCalculateWithWordWrap(item.exText, cfg.textSize, cfg.textSize * 0.1f, item.font, cfg.virtualScreenWidth * 0.6666667f);
		if (exText.size() > 1)
		{
			auto tmp = utils::ToCodepoints(".");
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
		}
		float heightCount = 0.0f;
		float spacing = item.region.height * 0.06;
		if (!exText.empty())
		{
			gfx::DrawTextCodepoints(item.font, exText[0], item.region.position(), cfg.textSize, cfg.textSize * 0.1f);
			heightCount += utils::MeasureTextSize(item.font, utils::ToUTF8Text(exText[0]), cfg.textSize, cfg.textSize * 0.1f).y;
			gfx::DrawRectangleGradientH
			(
				Rect{ item.region.x, item.region.y + spacing + heightCount - 0.8f, item.region.x + cfg.virtualScreenWidth * 0.3333333f, 1.6f },
				ColorR8G8B8A8(255, 255, 255, 255),
				ColorR8G8B8A8(255, 255, 255, 0)
			);
			heightCount += spacing * 2;
		}
		float lineSpacing = 0.0f;
		for (int i = 0; i < texts.size(); ++i)
		{
			float textHeight = utils::MeasureTextSize(item.font, utils::ToUTF8Text(texts[i]), cfg.textSize, cfg.textSize * 0.1f).y;
			if (heightCount + textHeight > item.region.height) break;
			if (heightCount + textHeight * 2 + lineSpacing <= item.region.height || i == texts.size() - 1)
			{
				gfx::DrawTextCodepoints(item.font, texts[i], Vec2{ item.region.x, item.region.y + heightCount }, cfg.textSize, cfg.textSize * 0.1f);
				heightCount += textHeight + lineSpacing;
			}
			else
			{
				auto tmp = utils::ToCodepoints(".");
				texts[i].push_back(tmp.back());
				texts[i].push_back(tmp.back());
				texts[i].push_back(tmp.back());
				gfx::DrawTextCodepoints(item.font, texts[i], Vec2{ item.region.x, item.region.y + heightCount }, cfg.textSize, cfg.textSize * 0.1f);
				break;
			}
		}
		gfx::DrawLine(Vec2{ item.region.x, item.region.y + item.region.height }, Vec2{ item.region.x + cfg.virtualScreenWidth * 0.6666667f, item.region.y + item.region.height }, ColorR8G8B8A8(128, 128, 128, 255), 1.0f);
	}

	static Item CreateItem(const BackLogView& logView, ScriptLoader& loader, int index, Vec2 texSize) noexcept
	{
		return Item
		{
			logView.text,
			logView.exText,
			logView.voice,
			logView.sceneName,

			Rect{texSize.x / 6.0f, index * (texSize.y / 6.0f), texSize.x * 0.6666667f, (texSize.y / 6.0f)},
			loader.cfg
		};
	}

	static std::vector<Item> CreateItemList(ScriptLoader& loader, int index, Vec2 texSize) noexcept
	{
		std::vector<Item> items;
		for (int i = 0; i < 6; ++i)
		{
			if (index + i == 0)
			{
				items.push_back(CreateItem(loader.backLogTmp, loader, 5, texSize));
			}
			else
			{
				items.push_back(CreateItem(loader.backLogViews[loader.backLogViews.size() - index - i], loader, 5 - i, texSize));
			}
		}
		return items;
	}

	BackLogCom::BackLogCom(const VisualNovelConfig& cfg, core::Layer* layer, std::string_view returnName, ScriptLoader& scLoader)
		: wheelDeltaCount(0.0f), index(0),
		animationUp(false), animationDown(false), animationTime(0.0f),
		drawOffsetY(0.0f), returnName(returnName),
		textureBuf(cfg.virtualScreenWidth, cfg.virtualScreenHeight * 5.0f / 6.0f),
		layer(layer)
	{
		auto& views = scLoader.backLogViews;
		items.push_back(CreateItem(scLoader.backLogTmp, scLoader, std::clamp<size_t>(views.size() + 1, 1, 5), textureBuf.size()));
		for (int i = 0; i < std::min<size_t>(views.size(), 4); ++i)
		{
			items.push_back(CreateItem(views[views.size() - i - 1], scLoader, std::clamp<size_t>(views.size(), 1, 4) - i, textureBuf.size()));
		}
		auto& world = scLoader.world;
	}

	void BackLogDraw::draw()
	{
		Rect origin{0, com.drawOffsetY, static_cast<float>(com.textureBuf.width()), -com.textureBuf.height() * 5.0f / 6.0f};
		gfx::DrawTextureRegionToRegion(com.textureBuf, origin, Rect{ 0, cfg.virtualScreenHeight / 6.0f, static_cast<float>(cfg.virtualScreenWidth), cfg.virtualScreenHeight * 0.75f });
	}

	void BackLogSystem::update()
	{
		coms->active()->forEach([this](core::entity id, BackLogCom& act)
			{
				auto& ina = *coms->inactive()->get(id);
				auto& logView = scLoader->backLogViews;

				ina.wheelDeltaCount += input::MouseWheelDelta();
				int32_t delta = act.wheelDeltaCount > 0.0 ? floor(act.wheelDeltaCount) : ceil(act.wheelDeltaCount);
				int32_t newIndex = std::clamp(act.index + delta, 0, std::max(static_cast<int>(logView.size()) - 4, 0));
					
				float itemHeight = act.textureBuf.height() / 6.0f;
				Vec2 texSize = act.textureBuf.size();

				float drawOffset = 0.0f;

				if ((act.index <= 0 && act.wheelDeltaCount < 0.0f)
					|| (act.index >= std::max(static_cast<int>(logView.size()) - 4, 0) && act.wheelDeltaCount > 0.0f))
					ina.wheelDeltaCount = 0.0f;
				
				if (newIndex != act.index)
				{
					ina.index = newIndex;

					ina.wheelDeltaCount = act.wheelDeltaCount - delta;

					if (delta > 0)
					{
						ina.animationUp = false;
						ina.animationDown = true;
					}
					else if (delta < 0)
					{
						ina.animationUp = true;
						ina.animationDown = false;
					}

					if (logView.size() < 6)
					{
						ina.items.push_back(CreateItem(scLoader->backLogTmp, *scLoader, logView.size(), texSize));
						for (int i = 0; i < logView.size(); ++i)
						{
							ina.items[i] = CreateItem(logView[i], *scLoader, i, texSize);
						}
					}
					else
					{
						if (delta < 0)
						{
							ina.items = CreateItemList(*scLoader, newIndex, texSize);
							ina.drawOffsetY = itemHeight;
						}
						else
						{
							ina.items = CreateItemList(*scLoader, newIndex - 1, texSize);
							ina.drawOffsetY = 0;
						}
					}
				}
				else if (act.animationDown || act.animationUp)
				{
					constexpr float animationDuration = 0.2f;


					ina.animationTime += GetFrameTime();

					if (act.animationTime > animationDuration)//动画结束
					{
						ina.animationTime = 0.0f;
						ina.animationUp = false;
						ina.animationDown = false;
					}
					else
					{
						if (act.animationUp)//动画插值
							ina.drawOffsetY = itemHeight - act.animationTime / animationDuration * itemHeight;
						else
							ina.drawOffsetY = act.animationTime / animationDuration * itemHeight;
					}
				}

				BeginTextureMode(act.textureBuf);
				gfx::ClearBackground(colors::Blank);
				for (auto& item : act.items)
				{
					DrawItem(item);
				}
				for (auto& dl : layerBuf)
				{
					dl->draw();
				}
				EndTextureMode();
				
				act.layer->push_back(std::make_unique<BackLogDraw>(act, scLoader->cfg));

				if (input::KeyPressed(input::Keyboard::Escape) || input::MousePressed(input::MouseButton::Right))
				{
					scLoader->loadScene(act.returnName);
				}
			});
	}
}