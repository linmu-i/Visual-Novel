#include <EbbGlow/UI/YUI/UILayout/ContextStack.h>

namespace ebbglow::ui::yui::layout
{
	static thread_local ContextStack contextStack(nullptr, {});

	ContextStack& GetContextStack()
	{
		return contextStack;
	}
	void InitContextStack(core::World2D& world, std::vector<core::entity> idList)
	{
		contextStack = ContextStack(&world, std::move(idList));
	}
}