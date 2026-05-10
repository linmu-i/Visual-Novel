#pragma once

#include <filesystem>

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	struct ArchiveInfo
	{
		std::string text;
		std::string exText;
		std::string attachmentText;
		rsc::SharedImage screenShoot;
		time_t time = 0;
	};

	bool SaveToFile(const std::filesystem::path& path, const ScriptLoader& scLoader, const ArchiveInfo& info);
	bool SaveToStream(std::ostream& os, const ScriptLoader& scLoader, const ArchiveInfo& info);

	bool LoadFromFile(const std::filesystem::path& path, ScriptLoader& scLoader, ArchiveInfo& info);
	bool LoadFromStream(std::istream& is, ScriptLoader& scLoader, ArchiveInfo& info);
	
	bool ReadArchiveInfo(const std::filesystem::path& path, ArchiveInfo& info);

	inline ArchiveInfo& GetArchiveInfoBuffer()
	{
		static ArchiveInfo info;
		return info;
	}

	class ImageExporter : public core::DrawBase
	{
	private:
		rsc::SharedRenderTexture2D textureBuffer;
		core::Layers* layers;

	public:
		ImageExporter(core::Layers* layers, const VisualNovelConfig& cfg) : layers(layers),
			textureBuffer(cfg.virtualScreen.width, cfg.virtualScreen.height) {}
		void draw() override;
		core::RenderPackageFlags getFlags() override;
	};

	void ExportArchiveInfo(ScriptLoader& scLoader);


}