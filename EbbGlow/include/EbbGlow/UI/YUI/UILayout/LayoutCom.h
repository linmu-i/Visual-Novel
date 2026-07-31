#pragma once

#include <functional>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/UI/YUI/UILayout/Layout.h>

namespace ebbglow::ui::yui::layout
{
	struct [[nodiscard]] UICreator
	{
		core::World2D* world = nullptr;
		std::vector<core::entity> idList = {};

		std::vector<core::entity> operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Column
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Row
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Spacer
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()();
	};

	struct [[nodiscard]] Box
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		void operator()(std::function<void()> func);
	};

	struct [[nodiscard]] Button
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;

		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;

		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		Color color = 0x2563ebff;
		Color borderColor = 0x00000000;
		Color textColor = 0xffffffff;

		uint8_t layerDepth = 8;

		rsc::SharedFont font;
		float fontSize = 24.0f;
		float spacing = 1.0f;

		std::string text;

		float roundness = 0.2f;

		std::function<void(core::World2D&, core::entity)> onClick;

		void operator()();
	};

	struct [[nodiscard]] Text
	{
		SizeMode widthMode = SizeMode::Weight;
		float widthValue = 1.0f;
		SizeMode heightMode = SizeMode::Weight;
		float heightValue = 1.0f;
		AlignmentType alignment = Alignment::Center | Alignment::Middle;

		Color textColor = 0x2563ebff;
		uint8_t layerDepth = 8;
		rsc::SharedFont font;
		float textSize = 24.0f;
		float spacing = 1.0f;
		std::string text;

		void operator()();
	};
}