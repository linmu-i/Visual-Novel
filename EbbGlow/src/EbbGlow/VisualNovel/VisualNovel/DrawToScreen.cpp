#include <EbbGlow/VisualNovel/VisualNovel/DrawToScreen.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	void DrawToScreenDraw::draw()
	{
		auto& virtualScreen = scLoader.cfg.virtualScreen;
		{
			auto mode2DGuard = Mode2DGuard(Camera2D{ Vec2{ 0, 0 }, Vec2{ -virtualScreen.drawOffset.x, -virtualScreen.drawOffset.y }, 0.0f, 1.0f });
			auto scissorGuard = ScissorModeGuard(Rect{ virtualScreen.drawOffset.x, virtualScreen.drawOffset.y, static_cast<float>(virtualScreen.width), static_cast<float>(virtualScreen.height) });
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