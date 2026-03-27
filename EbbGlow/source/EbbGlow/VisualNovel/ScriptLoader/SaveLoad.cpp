#include <fstream>

#include <EbbGlow/VisualNovel/ScriptLoader/SaveLoad.h>
#include <EbbGlow/Utils/Serialization.h>

namespace ebbglow::visualnovel
{
	constexpr uint64_t BackLogViewVersion = 0;
	constexpr uint64_t VariableViewVersion = 0;
	constexpr uint64_t ScriptLoaderVersion = 0;

	bool SaveToFile(const std::filesystem::path& path, const ScriptLoader& scLoader)
	{
		std::ofstream ofs(path, std::ios::binary);
		if (!ofs)
		{
			return false;
		}
		return SaveToStream(ofs, scLoader);
	}

	template<utils::OutStream OS>
	bool Serialize(OS& os, const BackLogView& backLogView)
	{
		if (!utils::Serialize(os, BackLogViewVersion)) return false;
		if (!utils::Serialize(os, backLogView.sceneName)) return false;
		if (!utils::Serialize(os, backLogView.text)) return false;
		if (!utils::Serialize(os, backLogView.exText)) return false;
		if (!utils::Serialize(os, backLogView.voice)) return false;
		if (!utils::Serialize(os, backLogView.music)) return false;
		return true;
	}

	template<utils::InStream IS>
	bool Deserialize(IS& is, BackLogView& backLogView)
	{
		uint64_t version = {};
		if (!utils::Deserialize(is, version)) return false;
		if (version > BackLogViewVersion) return false;
		if (version >= 0)
		{
			if (!utils::Deserialize(is, backLogView.sceneName)) return false;
			if (!utils::Deserialize(is, backLogView.text)) return false;
			if (!utils::Deserialize(is, backLogView.exText)) return false;
			if (!utils::Deserialize(is, backLogView.voice)) return false;
			if (!utils::Deserialize(is, backLogView.music)) return false;
		}
		return true;
	}

	template<utils::OutStream OS>
	bool Serialize(OS& os, const VariableView& view)
	{
		if (!utils::Serialize(os, VariableViewVersion)) return false;
		if (!utils::Serialize(os, view.index)) return false;
		if (!utils::Serialize(os, view.length)) return false;

		return true;
	}

	template<utils::InStream IS>
	bool Deserialize(IS& is, VariableView& view)
	{
		uint64_t version = {};
		if (!utils::Deserialize(is, version)) return false;
		if (version > VariableViewVersion) return false;
		if (version >= 0)
		{
			if (!utils::Deserialize(is, view.index)) return false;
			if (!utils::Deserialize(is, view.length)) return false;
		}
		return true;
	}

	bool SaveToStream(std::ostream& os, const ScriptLoader& scLoader)
	{
		if (!utils::Serialize(os, ScriptLoaderVersion)) return false;
		if (!utils::Serialize(os, scLoader.musicMgr.getBgmPath())) return false;
		if (!utils::Serialize(os, scLoader.musicMgr.getBgmVolumeIndex())) return false;
		if (!utils::Serialize(os, scLoader.numberStorage)) return false;
		if (!utils::Serialize(os, scLoader.textStorage)) return false;
		if (!utils::Serialize(os, scLoader.numberView)) return false;
		if (!utils::Serialize(os, scLoader.textView)) return false;
		if (!utils::Serialize(os, scLoader.backLogViews)) return false;
		if (!utils::Serialize(os, scLoader.saveSceneName)) return false;

		return true;
	}

	bool LoadFromFile(const std::filesystem::path& path, ScriptLoader& scLoader)
	{
		std::ifstream ifs(path, std::ios::binary);
		if (!ifs)
		{
			return false;
		}
		return LoadFromStream(ifs, scLoader);
	}
	bool LoadFromStream(std::istream& is, ScriptLoader& scLoader)
	{
		uint64_t version = {};
		if (!utils::Deserialize(is, version)) return false;
		if (version > ScriptLoaderVersion) return false;
		if (version >= 0)
		{
			std::string bgmPath;
			uint16_t bgmVolumeIndex = {};
			if (!utils::Deserialize(is, bgmPath)) return false;
			if (!utils::Deserialize(is, bgmVolumeIndex)) return false;
			scLoader.musicMgr.SetBgm(rsc::SharedMusic(bgmPath.c_str()), scLoader.cfg.volumes[bgmVolumeIndex], bgmPath, bgmVolumeIndex);
			if (!utils::Deserialize(is, scLoader.numberStorage)) return false;
			if (!utils::Deserialize(is, scLoader.textStorage)) return false;
			if (!utils::Deserialize(is, scLoader.numberView)) return false;
			if (!utils::Deserialize(is, scLoader.textView)) return false;
			if (!utils::Deserialize(is, scLoader.backLogViews)) return false;
			if (!utils::Deserialize(is, scLoader.saveSceneName)) return false;
			scLoader.loadScene(scLoader.saveSceneName);
			
		}

		return true;
	}
}