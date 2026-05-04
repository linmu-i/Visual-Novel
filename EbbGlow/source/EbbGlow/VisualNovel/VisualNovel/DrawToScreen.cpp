#include <EbbGlow/VisualNovel/VisualNovel/DrawToScreen.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	void DrawToScreenDraw::draw()
	{
		{
			auto mode2DGuard = Mode2DGuard(Camera2D{ Vec2{ 0, 0 }, Vec2{ -scLoader.cfg.drawOffset.x, -scLoader.cfg.drawOffset.y }, 0.0f, 1.0f });
			auto scissorGuard = ScissorModeGuard(Rect{ scLoader.cfg.drawOffset.x, scLoader.cfg.drawOffset.y, static_cast<float>(scLoader.cfg.virtualScreenWidth), static_cast<float>(scLoader.cfg.virtualScreenHeight) });
			gfx::ClearBackground(ColorR8G8B8A8(0, 0, 0, 255));
			for (auto& layer : scLoader.tmpLayers)
			{
				for (auto& draw : layer)
				{
					draw->draw();
				}
			}
		}
		scLoader.tmpLayers.clear();
	}

	void DrawToScreenSystem::update()
	{
		layer->push_back(&drawPackage);
	}

	void ApplyDrawToScreen(core::World2D& world, ScriptLoader& scLoader, core::Layer* layer)
	{
		world.addSystem<DrawToScreenSystem>(DrawToScreenSystem(scLoader, layer));
	}
}