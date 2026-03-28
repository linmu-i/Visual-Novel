#include <EbbGlow/VisualNovel/VisualNovel/DrawToScreen.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	void DrawToScreenDraw::draw()
	{
		{
			TextureModeGuard guard(scLoader.tmpRenderTexture);
			gfx::ClearBackground(ColorR8G8B8A8(0, 0, 0, 0));
			for (auto& layer : scLoader.tmpLayers)
			{
				for (auto& draw : layer)
				{
					draw->draw();
				}
			}
		}
		scLoader.tmpLayers.clear();
		gfx::DrawTexture(scLoader.tmpRenderTexture, scLoader.cfg.drawOffset);
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