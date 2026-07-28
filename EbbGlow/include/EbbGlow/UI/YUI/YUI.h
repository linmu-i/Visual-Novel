#pragma once

#include <EbbGlow/UI/YUI/YUITransform.h>
#include <EbbGlow/UI/YUI/YUIControl.h>
#include <EbbGlow/UI/YUI/YUIView.h>
#include <EbbGlow/UI/YUI/YUILayerCom.h>

#include <EbbGlow/UI/YUI/ImageBox/YUIImageBox.h>
#include <EbbGlow/UI/YUI/Button/YUIButton.h>
#include <EbbGlow/UI/YUI/TextBox/YUITextBox.h>
#include <EbbGlow/UI/YUI/ScrollContainer/YUIScrollContainer.h>

#include <EbbGlow/UI/YUI/YUILayout.h>

namespace ebbglow::ui::yui
{
	void ApplyYUI(core::World2D& world);
}