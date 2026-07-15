#include <EbbGlow/UI/YUI/YUIControl.h>

namespace ebbglow::ui::yui
{
	bool IsVisible(core::ComponentPool<ControlCom>* pool, core::entity id)
	{
		if (!pool) return true;
		auto* controlCom = pool->get(id);
		if (!controlCom) return true;
		if (!controlCom->isVisible) return false;

		for (auto& otherId : controlCom->others)
		{
			auto* otherControl = pool->get(otherId);
			if (!otherControl) continue;
			if (!otherControl->isVisible) return false;
		}
		return true;
	}

	bool IsControlActive(core::ComponentPool<ControlCom>* pool, core::entity id)
	{
		if (!pool) return true;
		auto* controlCom = pool->get(id);
		if (!controlCom) return true;
		if (!controlCom->isActive) return false;

		for (auto& otherId : controlCom->others)
		{
			auto* otherControl = pool->get(otherId);
			if (!otherControl) continue;
			if (!otherControl->isActive) return false;
		}
		return true;
	}

	std::vector<input::InputPoint> GetActiveInputPoints(
		core::ComponentPool<ControlCom>* ctrlPool,
		core::ComponentPool<TransformCom>* transPool, core::entity id)
	{
		if (!ctrlPool || !transPool) return input::PointList();
		auto* controlCom = ctrlPool->get(id);
		if (!controlCom) return input::PointList();
		if (!IsControlActive(ctrlPool, id)) return {};


		auto IsActPoint = [](Transform finalTransform, Rect activeArea, Vec2 point)
			{

				Vec2 finalPoint = (point - finalTransform.position).rotatedAround(finalTransform.pivot, -finalTransform.rotation);
				return activeArea.scaleAround(finalTransform.pivot, finalTransform.scale).contain(finalPoint);
			};

		Transform finalTransform = GetFinalTransform(GetTransforms(transPool, id));

		std::vector<input::InputPoint> result = input::PointList();
		result.erase(std::remove_if(result.begin(), result.end(),
			[&](const input::InputPoint& point)
			{
				return !IsActPoint(finalTransform, controlCom->interactiveArea, point.position);
			}), result.end());

		if (result.empty()) return {};

		for (auto& otherId : controlCom->others)
		{
			auto* otherControl = ctrlPool->get(otherId);
			if (!otherControl) continue;
			Transform otherFinalTransform = GetFinalTransform(GetTransforms(transPool, otherId));

			result.erase(std::remove_if(result.begin(), result.end(),
				[&](const input::InputPoint& point)
				{
					return !IsActPoint(otherFinalTransform, otherControl->interactiveArea, point.position);
				}), result.end());
			if (result.empty()) return {};
		}
		return result;
	}

	void ControlAttachTo(ControlCom& child, const ControlCom& parent, core::entity parentId)
	{
		child.others.clear();
		child.others.reserve(parent.others.size() + 1);
		child.others.insert(child.others.begin(), parent.others.begin(), parent.others.end());
		child.others.push_back(parentId);
	}

	void ControlAttachTo(core::World2D& world, core::entity child, core::entity parent)
	{
		auto* c = world.getDoubleBuffer<ControlCom>()->inactive()->get(child);
		if (!c) return;
		auto* p = world.getDoubleBuffer<ControlCom>()->active()->get(parent);
		if (!p) return;
		ControlAttachTo(*c, *p, parent);
	}
}