#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	struct ColorTweenCom
	{
		ColorR8G8B8A8 begin;
		ColorR8G8B8A8 end;

		float duration;
		float timeCount;

		core::Layer* layer;

		ColorTweenCom(ColorR8G8B8A8 begin, ColorR8G8B8A8 end, float duration, core::Layer* layer) :
			begin(begin), end(end), duration(duration), timeCount(0.0f), layer(layer) {
		}
	};

	class ColorTweenDraw : public core::DrawBase
	{
	private:
		ColorR8G8B8A8 color;
		Rect dest;

	public:
		ColorTweenDraw(ColorR8G8B8A8 color, Rect dest) : color(color), dest(dest) {}
		void draw() override;
	};

	class ColorTweenSystem : public core::SystemBase
	{
	private:
		core::DoubleComs<ColorTweenCom>* coms;
		Rect dest;

	public:
		ColorTweenSystem(core::World2D* world, ScriptLoader* scLoader) : coms(world->getDoubleBuffer<ColorTweenCom>()),
			dest(scLoader->cfg.drawOffset, Vec2{ static_cast<float>(scLoader->cfg.VirtualScreenWidth), static_cast<float>(scLoader->cfg.VirtualScreenHeight) }) {
		}

		void update() override;
	};

	void ApplyColorTween(core::World2D& world, ScriptLoader& scLoader) noexcept;
}