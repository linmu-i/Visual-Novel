#pragma once

#include <EbbGlow/Core/Message.h>

namespace ebbglow::ui
{
	class ButtonPressMsg : public core::MessageBase
	{
	private:
		int32_t index;
	public:
		ButtonPressMsg(core::entity senderId, core::MessageTypeId typeId, int32_t index = 0) : MessageBase(senderId, typeId), index(index) {}
		int32_t getIndex() const { return index; }
	};

	class ButtonReleaseMsg : public core::MessageBase
	{
	private:
		int32_t index;
	public:
		ButtonReleaseMsg(core::entity senderId, core::MessageTypeId typeId, int32_t index = 0) : MessageBase(senderId, typeId), index(index) {}
		int32_t getIndex() const { return index; }
	};

	class ButtonHoverMsg : public core::MessageBase
	{
	private:
		int32_t index;
	public:
		ButtonHoverMsg(core::entity senderId, core::MessageTypeId typeId, int32_t index = 0) : MessageBase(senderId, typeId), index(index) {}
		int32_t getIndex() const { return index; }
	};
}