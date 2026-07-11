#include <EbbGlow/UI/YUI/ImageBox/YUIImageBox.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Control.h>

namespace ebbglow::ui::yui
{
	void ImageBoxDraw::draw()
	{
		if (scissorRect) BeginScissorMode(*scissorRect);

		gfx::DrawTexture(imgBox.img, transform.position, transform.scale, transform.rotation, transform.pivot);

		if (scissorRect) EndScissorMode();
	}

	ImageBoxSystem::ImageBoxSystem(core::World2D& world)
	{
		trans = world.getDoubleBuffer<TransformCom>();
		ctrl = world.getDoubleBuffer<ControlCom>();
		viewPort = world.getDoubleBuffer<ViewPortCom>();
		layer = world.getDoubleBuffer<LayerCom>();
		imgBox = world.getDoubleBuffer<ImageBox>();
	}

	void ImageBoxSystem::update()
	{
		imgBox->active()->forEach([this](core::entity id, const ImageBox& imgBox)
			{
				if (!IsVisible(ctrl->active(), id)) return;
				Transform finalTrans = GetFinalTransform(GetTransforms(trans->active(), id));
				LayerCom& layerCom = *layer->active()->get(id);
				auto scissorRect = GetFinalViewPort(viewPort->active(), trans->active(), id);
				layerCom.layer->push_back(std::make_unique<ImageBoxDraw>(imgBox, scissorRect, finalTrans));
			});
	}

	void ApplyImageBox(core::World2D& world)
	{
		world.addPool<ImageBox>();
		world.addSystem(ImageBoxSystem(world));
	}
}