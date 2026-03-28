#include <EbbGlow/VisualNovel/VisualNovel/DrawToScreen.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::visualnovel
{
	void DrawToScreenDraw::draw()
	{
		scLoader.swapTmpLayers();
		gfx::DrawTexture(scLoader.tmpRenderTexture, scLoader.cfg.drawOffset);
		//gfx::DrawCircle({ 200,300 }, 100.0f, { 255, 255, 255, 255 });
	}

	void DrawToScreenSystem::update()
	{
		BeginTextureMode(scLoader->tmpRenderTexture);
		for (auto& layer : scLoader->getActTmpLayers())
		{
			for (auto& draw : layer)
			{
				draw->draw();
			}
		}
		EndTextureMode();
		for (auto& layer : scLoader->getActTmpLayers())
		{
			layer.clear();
		}
		layer->push_back(std::make_unique<DrawToScreenDraw>(*scLoader));
	}

	void ApplyDrawToScreen(core::World2D& world, ScriptLoader& scLoader, core::Layer* layer)
	{
		world.addSystem<DrawToScreenSystem>(DrawToScreenSystem(scLoader, layer));
	}
}