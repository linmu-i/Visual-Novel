#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/GlobalFunctions.h>
#include <EbbGlow/VisualNovel/ScriptLoader/SceneFunctions.h>
#include <EbbGlow/VisualNovel/ScriptLoader/SceneTypes.h>
#include <EbbGlow/VisualNovel/UI/UIState.h>
#include <EbbGlow/VisualNovel/UI/BackLog.h>
#include <EbbGlow/VisualNovel/VisualNovel/ColorTween.h>
#include <EbbGlow/VisualNovel/VisualNovel/JumpAttachment.h>


namespace ebbglow::visualnovel
{
	inline void ImportMainLib(ScriptLoader& scLoader)
	{
		scLoader.registerGlobalFunction("String", Global_String);
		scLoader.registerGlobalFunction("Number", Global_Number);
		scLoader.registerGlobalFunction("Include", Global_Include);
		scLoader.registerGlobalFunction("BeginScene", Global_BeginScene);
		scLoader.registerGlobalFunction("I18nText", Global_I18nText);
		scLoader.registerGlobalFunction("BackLogScene", Global_BackLogScene);

		scLoader.registerSceneType("TextScene", SceneType_TextScene);
		scLoader.registerSceneType("SelectScene", SceneType_SelectScene);
		scLoader.registerSceneType("DelayScene", SceneType_DelayScene);
		scLoader.registerSceneType("BlankScene", SceneType_BlankScene);

		scLoader.registerSceneFunction("TextScene", Scene_TextScene);
		scLoader.registerSceneFunction("Chr", Scene_Chr);
		scLoader.registerSceneFunction("Button", Scene_Button);
		scLoader.registerSceneFunction("ImageBox", Scene_ImageBox);
		scLoader.registerSceneFunction("SetString", Scene_SetStr);
		scLoader.registerSceneFunction("SetNumber", Scene_SetNum);
		scLoader.registerSceneFunction("BeginKeyFrameAnimation", Scene_BeginKeyFrameAnimation);
		scLoader.registerSceneFunction("AddKeyFrame", Scene_AddKeyFrame);
		scLoader.registerSceneFunction("EndKeyFrameAnimation", Scene_EndKeyFrameAnimation);
		scLoader.registerSceneFunction("SetBgm", Scene_SetBgm);
		scLoader.registerSceneFunction("SetVoice", Scene_SetVoice);
		scLoader.registerSceneFunction("ColorTween", Scene_ColorTween);
		scLoader.registerSceneFunction("BackLogCom", Scene_BackLogCom);
		scLoader.registerSceneFunction("JumpButton", Scene_JumpButton);
		
		scLoader.registerPredefinedVariable("SCENE_ARGS_LIST", [](ScriptLoader* scLoader, const std::vector<std::string>& args)
			{
				int32_t offset = 0;
				if (args.size() > 0) offset = static_cast<int32_t>(round(GetNumber(args[0], '\0', *scLoader)));//std::stoi(args[0]);
				return &(scLoader->sceneArgs[offset]);
			});
		scLoader.registerPredefinedVariable("BACK_LOG_RETURN_NAME", [](ScriptLoader* scLoader, const std::vector<std::string>& args) { return &(scLoader->backLogRetName); });
		scLoader.registerPredefinedVariable("to_string", [](ScriptLoader* scLoader, const std::vector<std::string>& args)
			{
				thread_local static std::string buffer;
				if (args.size() < 1)
				{
					buffer = "";
				}
				else
				{
					buffer = std::to_string(GetNumber(args[0], '\0', *scLoader));
				}
				return &buffer;
			});
	}

	inline void ApplyVisualNovel(core::World2D& world, VisualNovelConfig& cfg, ScriptLoader& scLoader)
	{
		ui::ApplyAnimation(world);
		ui::ApplyButtonEx(world);
		ui::ApplyImageBoxEx(world);
		ui::ApplyKeyFramesAnimation(world);
		ui::ApplyTextBoxEx(world);
		ApplyMainTextBox(world, cfg);
		ApplyTextScene(world, scLoader);
		ApplySelectScene(world, scLoader);
		ApplyDelayScene(world, scLoader);
		//ApplyUIState(world, cfg, scLoader);
		ApplyLogView(world, scLoader);
		ApplyColorTween(world, scLoader);
		ApplyJumpAttachment(world, scLoader);
	}
}