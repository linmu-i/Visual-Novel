#pragma once

#include <filesystem>

#include <EbbGlow/VisualNovel/ScriptLoader/ScriptLoader.h>

namespace ebbglow::visualnovel
{
	bool SaveToFile(const std::filesystem::path& path, const ScriptLoader& scLoader);
	bool SaveToStream(std::ostream& os, const ScriptLoader& scLoader);

	bool LoadFromFile(const std::filesystem::path& path, ScriptLoader& scLoader);
	bool LoadFromStream(std::istream& is, ScriptLoader& scLoader);
}