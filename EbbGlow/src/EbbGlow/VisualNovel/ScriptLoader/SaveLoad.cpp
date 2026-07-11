#include <fstream>
#include <raylib.h>

#include <EbbGlow/VisualNovel/ScriptLoader/SaveLoad.h>
#include <EbbGlow/Utils/Serialization.h>
#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Graphics/Graphics.h>
#include "../src/EbbGlow/Utils/ResourceCreator.h"

namespace ebbglow::visualnovel
{
	constexpr uint64_t BackLogViewVersion	= 0;
	constexpr uint64_t VariableViewVersion	= 0;
	constexpr uint64_t ScriptLoaderVersion	= 0;
	constexpr uint64_t ArchiveInfoVersion	= 0;

	bool SaveToFile(const std::filesystem::path& path, const ScriptLoader& scLoader, const ArchiveInfo& info)
	{
		std::ofstream ofs(path, std::ios::binary);
		if (!ofs)
		{
			return false;
		}
		return SaveToStream(ofs, scLoader, info);
	}

	template<utils::OutStream OS>
	bool Serialize(OS& os, const BackLogView& backLogView)
	{
		if (!utils::Serialize(os, BackLogViewVersion)) return false;
		if (!utils::Serialize(os, backLogView.sceneName)) return false;
		if (!utils::Serialize(os, backLogView.text)) return false;
		if (!utils::Serialize(os, backLogView.exText)) return false;
		if (!utils::Serialize(os, backLogView.voice)) return false;
		if (!utils::Serialize(os, backLogView.attText)) return false;
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
			if (!utils::Deserialize(is, backLogView.attText)) return false;
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

	template<utils::OutStream OS>
	bool Serialize(OS& os, const ArchiveInfo& info)
	{
		if (!utils::Serialize(os, ArchiveInfoVersion)) return false;
		if (!utils::Serialize(os, info.text)) return false;
		if (!utils::Serialize(os, info.exText)) return false;
		if (!utils::Serialize(os, info.attachmentText)) return false;
		if (!rsc::Serialize(os, info.screenShoot)) return false;
		if (!utils::Serialize(os, info.time)) return false;
		return true;
	}

	template<utils::InStream IS>
	bool Deserialize(IS& is, ArchiveInfo& info)
	{
		uint64_t version = {};
		if (!utils::Deserialize(is, version)) return false;
		if (version > ArchiveInfoVersion) return false;
		if (version >= 0)
		{
			if (!utils::Deserialize(is, info.text)) return false;
			if (!utils::Deserialize(is, info.exText)) return false;
			if (!utils::Deserialize(is, info.attachmentText)) return false;
			if (!rsc::Deserialize(is, info.screenShoot)) return false;
			if (!utils::Deserialize(is, info.time)) return false;
		}
		return true;
	}

	bool SaveToStream(std::ostream& os, const ScriptLoader& scLoader, const ArchiveInfo& info)
	{
		if (!utils::Serialize(os, ScriptLoaderVersion)) return false;
		if (!Serialize(os, info)) return false;
		if (!utils::Serialize(os, scLoader.musicMgr.getBgmPath())) return false;
		if (!utils::Serialize(os, scLoader.musicMgr.getBgmVolumeIndex())) return false;
		if (!utils::Serialize(os, scLoader.numberStorage)) return false;
		if (!utils::Serialize(os, scLoader.textStorage)) return false;
		if (!utils::Serialize(os, scLoader.numberView)) return false;
		if (!utils::Serialize(os, scLoader.textView)) return false;
		if (!utils::Serialize(os, scLoader.backLogViews)) return false;
		if (!Serialize(os, scLoader.backLogTmp)) return false;
		if (!utils::Serialize(os, scLoader.retName)) return false;

		return true;
	}

	bool LoadFromFile(const std::filesystem::path& path, ScriptLoader& scLoader, ArchiveInfo& info)
	{
		std::ifstream ifs(path, std::ios::binary);
		if (!ifs)
		{
			return false;
		}
		return LoadFromStream(ifs, scLoader, info);
	}
	bool LoadFromStream(std::istream& is, ScriptLoader& scLoader, ArchiveInfo& info)
	{
		uint64_t version = {};
		if (!utils::Deserialize(is, version)) return false;
		if (version > ScriptLoaderVersion) return false;

		std::string bgmPath;
		uint16_t bgmVolumeIndex = {};
		std::vector<double> tmpNumberStorage;
		std::vector<std::string> tmpTextStorage;
		std::unordered_map<std::string, VariableView> tmpNumberView;
		std::unordered_map<std::string, VariableView> tmpTextView;
		std::deque<BackLogView> tmpBackLogViews;
		BackLogView tmpBackLogTmp;
		std::string tmpRetName;

		if (version >= 0)
		{
			if (!Deserialize(is, info)) return false;
			
			if (!utils::Deserialize(is, bgmPath)) return false;
			if (!utils::Deserialize(is, bgmVolumeIndex)) return false;
			scLoader.musicMgr.SetBgm(rsc::SharedMusic(bgmPath.c_str()), scLoader.cfg.audio.volumes[bgmVolumeIndex], bgmPath, bgmVolumeIndex);

			
			if (!utils::Deserialize(is, tmpNumberStorage)) return false;
			if (!utils::Deserialize(is, tmpTextStorage)) return false;
			if (!utils::Deserialize(is, tmpNumberView)) return false;
			if (!utils::Deserialize(is, tmpTextView)) return false;
			if (!utils::Deserialize(is, tmpBackLogViews)) return false;
			if (!Deserialize(is, tmpBackLogTmp)) return false;
			if (!utils::Deserialize(is, tmpRetName)) return false;
		}

		scLoader.numberStorage = std::move(tmpNumberStorage);
		scLoader.textStorage = std::move(tmpTextStorage);
		scLoader.numberView = std::move(tmpNumberView);
		scLoader.textView = std::move(tmpTextView);
		scLoader.backLogViews = std::move(tmpBackLogViews);
		scLoader.backLogTmp = std::move(tmpBackLogTmp);
		scLoader.retName = std::move(tmpRetName);
		scLoader.loadScene(scLoader.retName);

		return true;
	}

	bool ReadArchiveInfo(const std::filesystem::path& path, ArchiveInfo& info)
	{
		std::ifstream is{ path, std::ios::binary };
		if (!is) return false;
		uint64_t version = {};
		if (!utils::Deserialize(is, version)) return false;
		return Deserialize(is, info);
	}

	void ImageExporter::draw()
	{
		{
			TextureModeGuard guard{ textureBuffer };
			gfx::ClearBackground(colors::Black);
			for (int i = 0; i < 10; ++i)
			{
				for (auto& it : (*layers)[i])
				{
					if (!it->getFlags().hasFlag(core::RenderPackageFlags::ExcludeRecursive)) it->draw();
				}
			}
		}
		rsc::SharedRenderTexture2D tmp{ 256, 144 };
		{
			TextureModeGuard guard{ tmp };
			gfx::ClearBackground(colors::Black);
			gfx::DrawTextureRegionToRegion(
				textureBuffer,
				Rect{ 0.0f, 0.0f, static_cast<float>(textureBuffer.width()), -static_cast<float>(textureBuffer.height()) },
				Rect{ 0.0f, 0.0f, 256.0f, 144.0f },
				Vec2{ 0.0f, 0.0f }, 0.0f,
				colors::White
			);
		}
		GetArchiveInfoBuffer().screenShoot = utils::LoadImageFromTexture(tmp);
	}

	core::RenderPackageFlags ImageExporter::getFlags()
	{
		return core::RenderPackageFlags::ExcludeRecursive;
	}

	void ExportArchiveInfo(ScriptLoader& scLoader)
	{
		ArchiveInfo& info = GetArchiveInfoBuffer();
		auto& view = scLoader.backLogTmp;
		info.text = view.text;
		info.exText = view.exText;
		scLoader.tmpLayers[0].push_back(std::make_unique<ImageExporter>( &scLoader.tmpLayers, scLoader.cfg ));
	}

	
}