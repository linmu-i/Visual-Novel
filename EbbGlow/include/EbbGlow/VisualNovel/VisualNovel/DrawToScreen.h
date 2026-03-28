#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	class DrawToScreenDraw : public core::DrawBase
	{
	private:
		ScriptLoader& scLoader;
	public:
		DrawToScreenDraw(ScriptLoader& scLoader) : scLoader(scLoader) {}
		void draw() override;
	};

	class DrawToScreenSystem : public core::SystemBase
	{
	private:
		ScriptLoader* scLoader;
		core::Layer* layer;
		DrawToScreenDraw drawPackage;

	public:
		DrawToScreenSystem(ScriptLoader& scLoader, core::Layer* layer) : scLoader(&scLoader), layer(layer), drawPackage(scLoader) {}
		void update() override;
	};

	void ApplyDrawToScreen(core::World2D& world, ScriptLoader& scLoader, core::Layer* layer);
}