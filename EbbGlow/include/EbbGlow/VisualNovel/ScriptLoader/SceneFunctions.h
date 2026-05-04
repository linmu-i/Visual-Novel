#pragma once

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	void Scene_TextScene(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_Chr(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_Button(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_ImageBox(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_SetStr(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_SetNum(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_BeginKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_AddKeyFrame(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_EndKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_SetBgm(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_SetVoice(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_ColorTween(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_BackLogCom(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_TextBoxCom(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_ButtonWithAttachment(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_SaveBlocks(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_LoadBlocks(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_SetBackLog(ScriptLoader* loader, const std::vector<std::string>& args);
	void Scene_PushBackLog(ScriptLoader* loader, const std::vector<std::string>& args);
}