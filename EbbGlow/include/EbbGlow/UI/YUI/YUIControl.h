#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/UI/YUI/YUITransform.h>

namespace ebbglow::ui::yui
{
	struct ControlCom
	{
		Rect interactiveArea;
		bool isActive = true;
		bool isVisible = true;
		std::vector<core::entity> others;
	};

	bool IsVisible(core::ComponentPool<ControlCom>* pool, core::entity id);

	bool IsControlActive(core::ComponentPool<ControlCom>* pool, core::entity id);

	std::vector<input::InputPoint> GetActiveInputPoints(
		core::ComponentPool<ControlCom>* ctrlPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id);

	void ControlAttachTo(ControlCom& child, const ControlCom& parent, core::entity parentId);
	void ControlAttachTo(core::World2D& world, core::entity child, core::entity parent);
}