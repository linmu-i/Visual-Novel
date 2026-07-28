#pragma once

#include <EbbGlow/Utils/Types.h>

namespace ebbglow::ui::yui::layout
{
	using AlignmentType = uint8_t;

	//若为含有主轴和交叉轴的控件，则忽略主轴方向上的对齐。
	struct Alignment
	{
		constexpr static AlignmentType Left = 0b00000001;
		constexpr static AlignmentType Center = 0b00000010;
		constexpr static AlignmentType Right = 0b00000100;

		constexpr static AlignmentType Top = 0b00001000;
		constexpr static AlignmentType Middle = 0b00010000;
		constexpr static AlignmentType Bottom = 0b00100000;
	};

	enum class SizeMode
	{
		Weight,
		Pixels,
		Auto
	};

	struct UILayout
	{
		SizeMode widthMode = SizeMode::Auto;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Auto;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;
	};

	struct UISize
	{
		Vec2 Size;
	};
}