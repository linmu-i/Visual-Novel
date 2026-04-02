#pragma once

#include <EbbGlow/Core/World.h>

namespace ebbglow::components
{
	struct ChildrenBlock
	{
		core::World2D* world;
		std::vector<core::entity> childrenIds;

		ChildrenBlock(core::World2D* world) : world(world) {}
		ChildrenBlock(core::World2D* world, const std::vector<core::entity>& ids) : world(world), childrenIds(ids) {}
		ChildrenBlock(core::World2D* world, std::vector<core::entity>&& ids) : world(world), childrenIds(std::move(ids)) {}
		~ChildrenBlock();
	};

	struct Children
	{
		std::shared_ptr<ChildrenBlock> block;

		Children() : block(std::make_shared<ChildrenBlock>(nullptr)) {}
		Children(core::World2D* world) : block(std::make_shared<ChildrenBlock>(world)) {}
		Children(core::World2D* world, const std::vector<core::entity>& ids) : block(std::make_shared<ChildrenBlock>(world, ids)) {}
		Children(core::World2D* world, std::vector<core::entity>&& ids) : block(std::make_shared<ChildrenBlock>(world, std::move(ids))) {}
		void add(core::entity id);
		void remove(core::entity id);
	};
}