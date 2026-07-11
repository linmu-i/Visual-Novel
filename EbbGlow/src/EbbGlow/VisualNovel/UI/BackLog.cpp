#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/VisualNovel/UI/BackLog.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/VisualNovel/UI/UIState.h>

namespace ebbglow::visualnovel
{
	struct BackLogLayout
	{
		static constexpr Vec2 size = { 1920 * 0.6666667f, 1080 * 5.0f / 36.0f };
		static constexpr Rect drawRegion = Rect{ Vec2{1920 * 0.1666667f, 1080 / 6.0f - size.y }, Vec2{size.x, size.y * 5.0f } };
		static constexpr Rect viewRegion = Rect{ Vec2{1920 * 0.1666667f, 1080 / 6.0f }, Vec2{size.x, size.y * 5.0f } };
	};
	void DrawItem(const Item& item, Vec2 offset) noexcept
	{
		auto& cfg = *item.cfg;
		auto& virtualScreen = cfg.virtualScreen;

		auto texts = utils::TextLineCalculateWithWordWrap(item.text, cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio, item.font, cfg.virtualScreen.width * 0.6666667f);
		auto exText = utils::TextLineCalculateWithWordWrap(item.exText, cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio, item.font, cfg.virtualScreen.width * 0.6666667f);
		if (exText.size() > 1)
		{
			auto tmp = utils::ToCodepoints(".");
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
			exText[0].push_back(tmp.back());
		}
		float heightCount = item.region.height * 0.1f;
		float spacing = item.region.height * 0.06f;
		if (!exText.empty())
		{
			gfx::DrawTextCodepoints(item.font, exText[0], Vec2{ item.region.position().x, item.region.position().y + heightCount } + offset, cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio);
			heightCount += utils::MeasureTextSize(item.font, utils::ToUTF8Text(exText[0]), cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio).y;
			gfx::DrawRectangleGradientH
			(
				Rect{ item.region.x, item.region.y + spacing + heightCount - 0.8f, item.region.x + cfg.virtualScreen.width * 0.3333333f, 1.6f }.offsetOf(offset),
				ColorR8G8B8A8(255, 255, 255, 255),
				ColorR8G8B8A8(255, 255, 255, 0)
			);
			heightCount += spacing * 2;
		}
		float lineSpacing = 0.0f;
		for (int i = 0; i < texts.size(); ++i)
		{
			float textHeight = utils::MeasureTextSize(item.font, utils::ToUTF8Text(texts[i]), cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio).y;
			if (heightCount + textHeight > item.region.height) break;
			if (heightCount + textHeight * 2 + lineSpacing <= item.region.height || i == texts.size() - 1)
			{
				gfx::DrawTextCodepoints(item.font, texts[i], Vec2{ item.region.x, item.region.y + heightCount } + offset, cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio);
				heightCount += textHeight + lineSpacing;
			}
			else
			{
				auto tmp = utils::ToCodepoints(".");
				texts[i].push_back(tmp.back());
				texts[i].push_back(tmp.back());
				texts[i].push_back(tmp.back());
				gfx::DrawTextCodepoints(item.font, texts[i], Vec2{ item.region.x, item.region.y + heightCount } + offset, cfg.text.textSize * virtualScreen.drawRatio, cfg.text.textSize * 0.1f * virtualScreen.drawRatio);
				break;
			}
		}
		gfx::DrawLine(Vec2{ item.region.x, item.region.y + item.region.height } + offset, Vec2{ item.region.x + cfg.virtualScreen.width * 0.6666667f, item.region.y + item.region.height } + offset, ColorR8G8B8A8(128, 128, 128, 255), 1.0f);
	}

	static Item CreateItem(const BackLogView& logView, ScriptLoader& loader, size_t index, const VisualNovelConfig& cfg) noexcept
	{
		float scale = cfg.virtualScreen.drawRatio;
		return Item
		{
			GetString(logView.text, loader),
			GetString(logView.exText, loader),
			logView.voice,
			logView.sceneName,

			Rect{ BackLogLayout::drawRegion.x * scale, (index * (BackLogLayout::size.y) + BackLogLayout::drawRegion.y) * scale, BackLogLayout::size.x * scale, BackLogLayout::size.y * scale },
			loader.cfg
		};
	}

	static std::vector<Item> CreateItemList(ScriptLoader& loader, int index, const VisualNovelConfig& cfg) noexcept
	{
		std::vector<Item> items;
		for (int i = 0; i < 6; ++i)
		{
			if (index + i == 0)
			{
				items.push_back(CreateItem(loader.backLogTmp, loader, 5, cfg));
			}
			else
			{
				items.push_back(CreateItem(loader.backLogViews[loader.backLogViews.size() - index - i], loader, 5 - i, cfg));
			}
		}
		return items;
	}

	BackLogCom::BackLogCom(const VisualNovelConfig& cfg, core::Layer* layer, std::string_view returnName, ScriptLoader& scLoader)
		: wheelDeltaCount(0.0f), index(0),
		animationUp(false), animationDown(false), animationTime(0.0f),
		drawOffsetY(0.0f), returnName(returnName),
		layer(layer)
	{
		auto& views = scLoader.backLogViews;
		items.push_back(CreateItem(scLoader.backLogTmp, scLoader, std::clamp<size_t>(views.size() + 1, 1, 5), cfg));
		for (int i = 0; i < std::min<size_t>(views.size(), 4); ++i)
		{
			items.push_back(CreateItem(views[views.size() - i - 1], scLoader, std::clamp<size_t>(views.size(), 1, 4) - i, cfg));
		}
		auto& world = scLoader.world;
	}

	void BackLogDraw::draw()
	{
		auto& virtualScreen = cfg.virtualScreen;
		{
			auto scissorMode = ScissorModeGuard(Rect{ BackLogLayout::viewRegion.position() * virtualScreen.drawRatio + virtualScreen.drawOffset, BackLogLayout::viewRegion.coverage() * virtualScreen.drawRatio });
			for (auto& item : com.items)
			{
				DrawItem(item, Vec2{ 0, com.drawOffsetY });// + cfg.drawOffset);
			}
		}
		gfx::DrawLine(
			BackLogLayout::viewRegion.position() * virtualScreen.drawRatio,
			Vec2{ BackLogLayout::viewRegion.x + BackLogLayout::viewRegion.width, BackLogLayout::viewRegion.y } * virtualScreen.drawRatio,
			0xffffffff, 1.0f * virtualScreen.drawRatio);

		gfx::DrawLine(
			Vec2{ BackLogLayout::viewRegion.x , BackLogLayout::viewRegion.y + BackLogLayout::viewRegion.height } * virtualScreen.drawRatio,
			Vec2{ BackLogLayout::viewRegion.x + BackLogLayout::viewRegion.width, BackLogLayout::viewRegion.y + BackLogLayout::viewRegion.height } * virtualScreen.drawRatio,
			0xffffffff, 1.0f * virtualScreen.drawRatio);
	}

	void BackLogSystem::update()
	{
		coms->active()->forEach([this](core::entity id, BackLogCom& act)
			{
				auto& ina = *coms->inactive()->get(id);
				auto& logView = scLoader->backLogViews;

				ina.wheelDeltaCount += input::MouseWheelDelta();
				int32_t delta = static_cast<int32_t>(act.wheelDeltaCount > 0.0 ? floor(act.wheelDeltaCount) : ceil(act.wheelDeltaCount));
				int32_t newIndex = std::clamp(act.index + delta, 0, std::max(static_cast<int>(logView.size()) - 4, 0));
					
				float itemHeight = BackLogLayout::size.y * scLoader->cfg.virtualScreen.drawRatio;

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
						ina.items.push_back(CreateItem(scLoader->backLogTmp, *scLoader, logView.size(), scLoader->cfg));
						for (int i = 0; i < logView.size(); ++i)
						{
							ina.items[i] = CreateItem(logView[i], *scLoader, i, scLoader->cfg);
						}
					}
					else
					{
						if (delta < 0)
						{
							ina.items = CreateItemList(*scLoader, newIndex, scLoader->cfg);
							ina.drawOffsetY = itemHeight;
						}
						else
						{
							ina.items = CreateItemList(*scLoader, newIndex - 1, scLoader->cfg);
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
						if (act.animationUp)
						{
							ina.drawOffsetY = 0.0f;
						}
						else
						{
							ina.drawOffsetY = itemHeight;
						}
					}
					else
					{
						if (act.animationUp)//动画插值
							ina.drawOffsetY = itemHeight - act.animationTime / animationDuration * itemHeight;
						else
							ina.drawOffsetY = act.animationTime / animationDuration * itemHeight;
					}
				}
				
				act.layer->push_back(std::make_unique<BackLogDraw>(act, scLoader->cfg));

				if (input::KeyPressed(input::Keyboard::Escape) || input::MousePressed(input::MouseButton::Right))
				{
					scLoader->loadScene(act.returnName);
				}
			});
	}
}