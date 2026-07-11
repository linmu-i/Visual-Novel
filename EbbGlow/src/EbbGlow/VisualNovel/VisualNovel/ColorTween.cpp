#include <EbbGlow/VisualNovel/VisualNovel/ColorTween.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Functions.h>

namespace ebbglow::visualnovel
{
	void ColorTweenDraw::draw()
	{
		gfx::DrawRect(dest, color);
	}

	void ColorTweenSystem::update()
	{
		coms->active()->forEach([this](core::entity id, ColorTweenCom& act)
			{
				auto& ina = *coms->inactive()->get(id);
				ina.timeCount += utils::GetFrameTime() * 1000;
				if (act.timeCount >= act.duration)
				{
					ina.timeCount = act.duration;
				}
				float t = std::clamp(act.timeCount / act.duration, 0.0f, 1.0f);
				ColorR8G8B8A8 color = ColorR8G8B8A8(
					static_cast<uint8_t>(act.begin.r + (act.end.r - act.begin.r) * t),
					static_cast<uint8_t>(act.begin.g + (act.end.g - act.begin.g) * t),
					static_cast<uint8_t>(act.begin.b + (act.end.b - act.begin.b) * t),
					static_cast<uint8_t>(act.begin.a + (act.end.a - act.begin.a) * t)
				);
				act.layer->push_back(std::make_unique<ColorTweenDraw>(color, dest));
			});
	}

	void ApplyColorTween(core::World2D& world, ScriptLoader& scLoader) noexcept
	{
		world.addPool<ColorTweenCom>();
		world.addSystem(ColorTweenSystem(&world, &scLoader));
	}
}