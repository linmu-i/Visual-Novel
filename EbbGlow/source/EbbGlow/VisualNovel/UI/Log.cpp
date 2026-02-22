#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/VisualNovel/UI/Log.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Math.h>

namespace ebbglow::visualnovel
{
	void ItemDraw::draw()
	{
		auto& cfg = *com.cfg;
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
				ColorR8G8B8A8(255,255,255,0)
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

	void DrawItem(const Item& item) noexcept
	{
		auto& cfg = *item.cfg;
		auto texts = utils::TextLineCalculateWithWordWrap(item.text, cfg.textSize, cfg.textSize * 0.1f, item.font, cfg.ScreenWidth * 0.6666667f);
		auto exText = utils::TextLineCalculateWithWordWrap(item.exText, cfg.textSize, cfg.textSize * 0.1f, item.font, cfg.ScreenWidth * 0.6666667f);
		if (exText.size() > 1)
		{
			auto tmp = utils::ToCodepoints(".");
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
		}
		//gfx::DrawTextCodepoints(item.font, exText[0], item.region.position(), cfg.textSize, cfg.textSize * 0.1f);
		float heightCount = 0.0f;
		float spacing = item.region.height * 0.08;
		if (!exText.empty())
		{
			heightCount += utils::MeasureTextSize(item.font, utils::ToUTF8Text(exText[0]), cfg.textSize, cfg.textSize * 0.1f).y;
			gfx::DrawRectangleGradientH
			(
				Rect{ item.region.x, item.region.y + spacing + heightCount - 0.8f, item.region.x + cfg.ScreenWidth * 0.3333333f, 1.6f },
				ColorR8G8B8A8(255, 255, 255, 255),
				ColorR8G8B8A8(255, 255, 255, 0)
			);
			heightCount += spacing * 2;
		}
		for (int i = 0; i < texts.size(); ++i)
		{
			float textHeight = utils::MeasureTextSize(item.font, utils::ToUTF8Text(texts[i]), cfg.textSize, cfg.textSize * 0.1f).y;
			if (heightCount + textHeight > item.region.height) break;
			if (heightCount + textHeight * 2 + spacing <= item.region.height)
			{
				gfx::DrawTextCodepoints(item.font, texts[i], Vec2{ item.region.x, item.region.y + heightCount }, cfg.textSize, cfg.textSize * 0.1f);
				heightCount += textHeight + spacing;
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
	}

	static Item CreateItem(const LogView& logView, ScriptLoader& loader, int index, Vec2 texSize) noexcept
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
				items.push_back(CreateItem(loader.logTmp, loader, 5, texSize));
			}
			else
			{
				//items.push_back(CreateItem(loader.logView[loader.logView.size() - index - i], loader, 5 - i, texSize));
				items.push_back(CreateItem(loader.logView[loader.logView.size() - index - i], loader, 5 - i, texSize));
			}
		}
		return items;
	}

	LogCom::LogCom(const VisualNovelConfig& cfg, core::Layer* layer, std::string_view returnName, ScriptLoader& scLoader)
		: wheelDeltaCount(0.0f), index(0),
		animationUp(false), animationDown(false), animationTime(0.0f),
		drawOffsetY(0.0f), returnName(returnName),
		textureBuf(cfg.ScreenWidth, cfg.ScreenHeight * 5.0f / 6.0f),
		layer(layer)
	{
		auto& views = scLoader.logView;
		items.push_back(CreateItem(scLoader.logTmp, scLoader, std::clamp<size_t>(views.size() + 1, 1, 5), textureBuf.size()));
		for (int i = 0; i < std::min<size_t>(views.size(), 4); ++i)
		{
			items.push_back(CreateItem(views[views.size() - i - 1], scLoader, std::clamp<size_t>(views.size(), 1, 4) - i, textureBuf.size()));
		}
	}

	void LogDraw::draw()
	{

		Rect origin{0, com.drawOffsetY, static_cast<float>(com.textureBuf.width()), -com.textureBuf.height() * 5.0f / 6.0f};
		
		//gfx::DrawTextureRegionToRegion(com.textureBuf, Rect{ origin.position() - dst.position(), origin.coverage()}, dst);
		gfx::DrawTextureRegionToRegion(com.textureBuf, origin, Rect{ 0, cfg.ScreenHeight / 8.0f, static_cast<float>(cfg.ScreenWidth), cfg.ScreenHeight * 0.75f });
		//gfx::DrawTextureRegionToRegion(com.textureBuf, origin, Rect{ 0, cfg.ScreenHeight / 8.0f, static_cast<float>(cfg.ScreenWidth), cfg.ScreenHeight * 0.75f });
	}

	void LogSystem::update()
	{
		coms->active()->forEach([this](core::entity id, LogCom& act)
			{
				auto& ina = *coms->inactive()->get(id);
				auto& logView = scLoader->logView;

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
						ina.items.push_back(CreateItem(scLoader->logTmp, *scLoader, logView.size(), texSize));
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
							ina.drawOffsetY = 0;
						}
						else
						{
							ina.items = CreateItemList(*scLoader, newIndex - 1, texSize);
							ina.drawOffsetY = itemHeight;
						}
					}
				}
				else if (act.animationDown || act.animationUp)
				{
					constexpr float animationDuration = 0.2f;


					ina.animationTime += GetFrameTime();

					if (act.animationTime > animationDuration)
					{
						ina.animationTime = 0.0f;
						ina.animationUp = false;
						ina.animationDown = false;
					}
					else
					{
						if (act.animationUp)//delta > 0)
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
				
				act.layer->push_back(std::make_unique<LogDraw>(act, scLoader->cfg));
			});
	}
}