#pragma once

#include <EbbGlow/UI/YUI/YUITransform.h>
#include <EbbGlow/Utils/Functions.h>

namespace ebbglow::ui::yui
{
	struct ViewPortCom
	{
		std::optional<Rect> rect;
		std::vector<core::entity> others;
	};

	std::optional<Rect> GetFinalViewPort(
		core::ComponentPool<ViewPortCom>* vPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id);

	void ViewPortAttachTo(ViewPortCom& child, const ViewPortCom& parent, core::entity parentId);
	void ViewPortAttachTo(core::World2D& world, core::entity child, core::entity parent);
}