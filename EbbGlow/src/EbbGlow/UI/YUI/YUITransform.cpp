#include <EbbGlow/UI/YUI/YUITransform.h>

namespace ebbglow::ui::yui
{
	std::vector<Transform> GetTransforms(core::ComponentPool<TransformCom>* pool, core::entity id)
	{
		if (!pool) return {};
		auto* transformCom = pool->get(id);
		if (!transformCom) return {};
		std::vector<Transform> result;
		result.reserve(transformCom->others.size() + 1);
		for (auto& otherId : transformCom->others)
		{
			auto* otherTransform = pool->get(otherId);
			if (!otherTransform) continue;
			result.push_back(otherTransform->transform);
		}
		result.push_back(transformCom->transform);
		return result;
	}

	std::vector<Transform> GetTransforms(core::World2D& world, core::entity id)
	{
		return GetTransforms(world.getDoubleBuffer<TransformCom>()->active(), id);
	}

	Transform GetFinalTransform(std::span<const Transform> trans)
	{
		if (trans.empty()) return Transform{};
		Vec2 pivot = trans[0].pivot + trans[0].position;
		float rot = trans[0].rotation;
		float scl = trans[0].scale;

		for (uint64_t i = 0; i < trans.size() - 1; ++i)
		{
			auto& prev = trans[i];
			auto& next = trans[i + 1];
			Vec2 direction = (next.position + next.pivot) - prev.pivot;
			Vec2 result = direction.rotated(rot) * scl;
			pivot += result;
			rot += next.rotation;
			scl *= next.scale;
		}
		Vec2 finalPosition = pivot - trans.back().pivot;
		return Transform{ finalPosition, trans.back().pivot, rot, scl };
	}

	void TransformAttachTo(TransformCom& child, const TransformCom& parent, core::entity parentId)
	{
		child.others.clear();
		child.others.reserve(parent.others.size() + 1);
		child.others.insert(child.others.begin(), parent.others.begin(), parent.others.end());
		child.others.push_back(parentId);
	}

	void TransformAttachTo(core::World2D& world, core::entity child, core::entity parent)
	{
		auto* c = world.getDoubleBuffer<TransformCom>()->inactive()->get(child);
		if (!c) return;
		auto* p = world.getDoubleBuffer<TransformCom>()->active()->get(parent);
		if (!p) return;
		TransformAttachTo(*c, *p, parent);
	}
}