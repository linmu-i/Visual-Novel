#pragma once

#include <string>
#include <deque>

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptData.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>
#include <EbbGlow/VisualNovel/ScriptLoader/MusicManager.h>
#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoaderTypes.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/UI/UI.h>

namespace ebbglow::visualnovel
{
	class ScriptLoader
	{
	private:
		//处理函数
		std::unordered_map<std::string, std::function<void(ScriptLoader*, const std::vector<std::string>&)>> globalFunctions;
		std::unordered_map<std::string, std::function<void(ScriptLoader*, const std::vector<std::string>&)>> sceneFunctions;
		std::unordered_map<std::string, std::function<void(ScriptLoader*, const std::vector<std::string>&)>> sceneCreator;


		void Invoker(const Command& cmd, std::unordered_map<std::string, std::function<void(ScriptLoader*, const std::vector<std::string>&)>>& functions);
		void ExecuteFunction(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, const std::vector<std::string>&)>>& functions);
		void ExecuteMacro(rsc::SharedFile::Iterator& it, std::unordered_map<std::string, std::function<void(ScriptLoader*, const std::vector<std::string>&)>>& functions);
		SceneInfo ReadSceneInfo(rsc::SharedFile::Iterator& it);

		constexpr static int32_t maxBackLog = 500;

	public:
		core::World2D& world;
		MusicManager& musicMgr;

		//变量存储
		std::vector<double> numberStorage;
		std::vector<std::string> textStorage;
		std::unordered_map<std::string, VariableView> numberView;
		std::unordered_map<std::string, VariableView> textView;

		std::unordered_map<std::string, std::function<std::string* (ScriptLoader*, const std::vector<std::string>&)>> predefinedTextVariableRef;
		std::unordered_map<std::string, std::function<double* (ScriptLoader*, const std::vector<std::string>&)>> predefinedNumberVariableRef;

		//宏函数
		std::unordered_map<std::string, MacroView> macroView;

		//定位缓存
		std::unordered_map<std::string, int64_t> sceneView;

		//文本回看
		std::deque<BackLogView> backLogViews;

		//初始化数据
		ScriptData scriptData;
		std::string beginSceneName;
		std::string backLogSceneName;
		std::string saveSceneName;
		std::string loadSceneName;

		//运行时数据
		std::vector<core::entity> idList;
		std::vector<core::entity> exIdList;//特殊id列表，用于特殊组件配合特殊场景
		std::vector<std::string> sceneArgs;
		std::string sceneName;
		std::string sceneType;
		rsc::SharedFile::Iterator scIt;//初始化时使用的迭代器，供追加文本时更新迭代器使用

		BackLogView backLogTmp;

		std::string retName;//由任意需要保存当前场景以供回退的组件填写，以供回退定位

		//设置
		VisualNovelConfig& cfg;

		//多语言文本
		std::unordered_map<std::string, std::vector<std::string>> i18nText;

		//渲染缓存
		core::Layers tmpLayers;

		ScriptLoader(core::World2D& world, VisualNovelConfig& cfg, MusicManager* musicMgr) : world(world), cfg(cfg),
			musicMgr(*musicMgr) {
		}
		std::future<void> init(const std::filesystem::path& filePath);
		bool loadScene(rsc::SharedFile::Iterator& it);
		bool loadScene(const std::string& sceneName);
		void start();
		void registerSceneType(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function);
		void registerSceneFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function);
		void registerGlobalFunction(const std::string& name, const std::function<void(ScriptLoader*, std::vector<std::string>)>& function);
		void registerPredefinedVariable(const std::string& name, const std::function<std::string* (ScriptLoader*, const std::vector<std::string>&)> function);
		void registerPredefinedVariable(const std::string& name, const std::function<double* (ScriptLoader*, const std::vector<std::string>&)> function);
		void addToBackLog(const BackLogView& backLogView);
		void addToBackLog(BackLogView&& backLogView);
	};
	
	void SkipSpace(rsc::SharedFile::Iterator& ptr);
	double ParsePrimary(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseTerm(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);
	double ParseExpression(rsc::SharedFile::Iterator& ptr, unsigned char stop, ScriptLoader& scLoader);

	Command Tokenizer(rsc::SharedFile::Iterator& it);
	Command Tokenizer(std::string_view cmd);

	std::string GetStateTag(std::string_view token);
	std::string GetNextString(rsc::SharedFile::Iterator& it, ScriptLoader& scLoader);
	std::string GetString(std::string_view token, ScriptLoader& scLoader);
	double GetNumber(std::string_view token, unsigned char stop, ScriptLoader& scLoader);
	std::string* GetTextVariable(const std::string& name, ScriptLoader& scLoader);
	double* GetNumberVariable(const std::string& name, ScriptLoader& scLoader);
}