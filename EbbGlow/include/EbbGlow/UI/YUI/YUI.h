#pragma once

#include <EbbGlow/UI/YUI/YUITransform.h>
#include <EbbGlow/UI/YUI/YUIControl.h>
#include <EbbGlow/UI/YUI/YUIView.h>
#include <EbbGlow/UI/YUI/YUILayerCom.h>

#include <EbbGlow/UI/YUI/ImageBox/YUIImageBox.h>

namespace ebbglow::ui::yui
{
	inline void ApplyYUI(core::World2D& world)
	{
		world.addPool<TransformCom>();
		world.addPool<ControlCom>();
		world.addPool<ViewPortCom>();
		world.addPool<LayerCom>();

		ApplyImageBox(world);
	}
}