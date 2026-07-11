#include <EbbGlow/UI/YUI/YUIView.h>

namespace ebbglow::ui::yui
{
	std::optional<Rect> GetFinalViewPort(
		core::ComponentPool<ViewPortCom>* vPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id)
	{
		if (!vPool || !transPool) return std::nullopt;
		auto* viewPort = vPool->get(id);
		if (!viewPort) return std::nullopt;
		Transform finalTrans = GetFinalTransform(GetTransforms(transPool, id));

		bool hasValue = false;
		Rect result = Rect{ Vec2::Zero(), utils::ScreenSize()};
		if (viewPort->rect)
		{
			result = viewPort->rect->offsetOf(finalTrans.position).scaleAround(finalTrans.pivot, finalTrans.scale);
			hasValue = true;
		}

		for (auto& otherId : viewPort->others)
		{
			auto* otherViewPort = vPool->get(otherId);
			if (!otherViewPort) continue;
			if (!otherViewPort->rect) continue;
			Transform otherFinalTrans = GetFinalTransform(GetTransforms(transPool, otherId));
			result = result.intersection(
				otherViewPort->rect
				->scaleAround(otherFinalTrans.pivot, otherFinalTrans.scale)
				.offsetOf(otherFinalTrans.position));
			hasValue = true;
		}
		return hasValue ? std::make_optional(result) : std::nullopt;
	}

	void ViewPortAttachTo(ViewPortCom& child, const ViewPortCom& parent, core::entity parentId)
	{
		child.others.clear();
		child.others.reserve(parent.others.size() + 1);
		child.others.insert(child.others.begin(), parent.others.begin(), parent.others.end());
		child.others.push_back(parentId);
	}

	void ViewPortAttachTo(core::World2D& world, core::entity child, core::entity parent)
	{
		auto* c = world.getDoubleBuffer<ViewPortCom>()->inactive()->get(child);
		if (!c) return;
		auto* p = world.getDoubleBuffer<ViewPortCom>()->active()->get(parent);
		if (!p) return;
		ViewPortAttachTo(*c, *p, parent);
	}
}