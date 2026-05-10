#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <format>
#include <filesystem>
#include <functional>

#include <EbbGlow/Utils/Resource.h>

namespace ebbglow::visualnovel
{
	struct MainLibRsc
	{
		rsc::SharedTexture2D textBoxBackGround;

		rsc::SharedTexture2D chrNameBackGround;
		float chrNameOffsetX = 0.0f;

		static constexpr struct LayerDef
		{
			constexpr static uint8_t backGroundLayer = 0;
			constexpr static uint8_t illustrationLayer = 2;
			constexpr static uint8_t textBoxBackGroundLayer = 3;
			constexpr static uint8_t textBoxLayer = 4;
			constexpr static uint8_t ButtonLayer = 10;

			constexpr static uint8_t SLBlockLayer = 6;
			constexpr static uint8_t SLBlockBtLayer = 7;
		} LayerDefine;

		rsc::SharedTexture2D voiceBtImg;
		rsc::SharedTexture2D voiceBtHvrImg;
		rsc::SharedTexture2D voiceBtPrsImg;

		rsc::SharedTexture2D delBtImg;
		rsc::SharedTexture2D delBtHvrImg;
		rsc::SharedTexture2D delBtPrsImg;
	};

	inline MainLibRsc& GetMainLibRsc()
	{
		static MainLibRsc rsc;
		return rsc;
	}

	struct VisualNovelConfig
	{
		struct I18n
		{
			uint8_t mainLanguage = 0;
			uint8_t secondaryLanguage = 1;
			uint8_t uiLanguage = 0;

			bool secondLanguageShow = true;
		} i18n;
		struct Text
		{
			float textSpeed = 1.0f;
			float textSize = 20;
			std::string fontPath;
			rsc::SharedFile fontData;
			bool showReadText = false;
			Color readTextColor = colors::White;
		} text;
		struct Win
		{
			std::string title;

			int32_t width = 1920;
			int32_t height = 1080;

			bool fullScreen = true;

			int32_t fps = 120;
		} win;
		struct VirtualScreen
		{
			int32_t width = 0;
			int32_t height = 0;

			float drawRatio = 0.0f;
			Vec2 drawOffset = { 0,0 };
		} virtualScreen;

		struct Audio
		{
			std::vector<float> volumes;
		} audio;

		struct Save
		{
			std::string pathFormat;
			std::function<std::filesystem::path(size_t)> getSavePath = [](size_t index) { return std::filesystem::path(std::format("save/{:03d}.sav", index)); };
			size_t maxSaveCount = 80;
		} save;
	};

	void ReadVisualNovelConfig(const std::filesystem::path& path, VisualNovelConfig& cfg);
	void WriteVisualNovelConfig(const std::filesystem::path& path, const VisualNovelConfig& cfg);
	void CalculateVirtualScreen(VisualNovelConfig& cfg);
}

namespace ebbglow
{
	namespace vn = visualnovel;
}