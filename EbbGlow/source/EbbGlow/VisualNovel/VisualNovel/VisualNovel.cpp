#include <mINI/ini.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>
#include <EbbGlow/Utils/Functions.h>

namespace ebbglow::visualnovel
{
	struct DefaultConfig {
		static constexpr bool win_fullScreen = true;
		static constexpr int win_width = 1920;
		static constexpr int win_height = 1080;
		static constexpr const char* win_title = "";

		static constexpr float text_textSpeed = 1.0f;
		static constexpr int text_textSize = 32;
		static constexpr const char* text_fontPath = R"(resource/font/Noto_Sans_SC/static/NotoSansSC-SemiBold.ttf)";

		static constexpr int i18n_mainLanguage = 0;
		static constexpr int i18n_secondaryLanguage = 2;
		static constexpr int i18n_uiLanguage = 0;

		static constexpr int maxSaveCount = 80;

		static constexpr const char* savePath = "save/{:03d}.sav";

		static constexpr const char* audio_volumes = "0.8";
	};

	void ReadVisualNovelConfig(const std::filesystem::path& path, VisualNovelConfig& cfg)
	{
		if (!std::filesystem::exists(path))
		{
			cfg.win.fullScreen = DefaultConfig::win_fullScreen;
			cfg.win.width = DefaultConfig::win_width;
			cfg.win.height = DefaultConfig::win_height;
			cfg.win.title = DefaultConfig::win_title;

			cfg.text.textSpeed = DefaultConfig::text_textSpeed;
			cfg.text.textSize = DefaultConfig::text_textSize;
			cfg.text.fontPath = DefaultConfig::text_fontPath;
			cfg.text.fontData = rsc::SharedFile(DefaultConfig::text_fontPath);

			cfg.i18n.mainLanguage = DefaultConfig::i18n_mainLanguage;
			cfg.i18n.secondaryLanguage = DefaultConfig::i18n_secondaryLanguage;
			cfg.i18n.uiLanguage = DefaultConfig::i18n_uiLanguage;

			cfg.audio.volumes.push_back(0.8f);
			cfg.save.getSavePath = [](size_t index) {
				return std::filesystem::path(std::format(DefaultConfig::savePath, index));
				};

			cfg.save.maxSaveCount = DefaultConfig::maxSaveCount;
			return;
		}

		mINI::INIFile iniFile{ path };
		mINI::INIStructure cfgIni;
		iniFile.read(cfgIni);

		std::string value;
		if ((value = cfgIni["Graphics"]["fullScreen"]) != "")
		{
			cfg.win.fullScreen = (value == "true");
		}
		else
		{
			cfg.win.fullScreen = DefaultConfig::win_fullScreen;
		}

		if ((value = cfgIni["Graphics"]["windowWidth"]) != "")
		{
			cfg.win.width = std::stoi(value);
		}
		else
		{
			cfg.win.width = DefaultConfig::win_width;
		}

		if ((value = cfgIni["Graphics"]["windowHeight"]) != "")
		{
			cfg.win.height = std::stoi(value);
		}
		else
		{
			cfg.win.height = DefaultConfig::win_height;
		}

		if ((value = cfgIni["Graphics"]["windowTitle"]) != "")
		{
			cfg.win.title = value;
		}
		else
		{
			cfg.win.title = DefaultConfig::win_title;
		}

		if ((value = cfgIni["Text"]["textSpeed"]) != "")
		{
			cfg.text.textSpeed = std::stof(value);
		}
		else
		{
			cfg.text.textSpeed = DefaultConfig::text_textSpeed;
		}

		if ((value = cfgIni["Text"]["textSize"]) != "")
		{
			cfg.text.textSize = std::stof(value);
		}
		else
		{
			cfg.text.textSize = DefaultConfig::text_textSize;
		}

		if ((value = cfgIni["Text"]["fontPath"]) != "")
		{
			cfg.text.fontData = rsc::SharedFile(value.c_str());
			cfg.text.fontPath = value;
		}
		else
		{
			cfg.text.fontData = rsc::SharedFile(DefaultConfig::text_fontPath);
			cfg.text.fontPath = DefaultConfig::text_fontPath;
		}

		if ((value = cfgIni["I18n"]["mainLanguage"]) != "")
		{
			cfg.i18n.mainLanguage = static_cast<uint8_t>(std::stoi(value));
		}
		else
		{
			cfg.i18n.mainLanguage = DefaultConfig::i18n_mainLanguage;
		}

		if ((value = cfgIni["I18n"]["secondaryLanguage"]) != "")
		{
			cfg.i18n.secondaryLanguage = static_cast<uint8_t>(std::stoi(value));
		}
		else
		{
			cfg.i18n.secondaryLanguage = DefaultConfig::i18n_secondaryLanguage;
		}

		if ((value = cfgIni["I18n"]["uiLanguage"]) != "")
		{
			cfg.i18n.uiLanguage = static_cast<uint8_t>(std::stoi(value));
		}
		else
		{
			cfg.i18n.uiLanguage = DefaultConfig::i18n_uiLanguage;
		}

		if ((value = cfgIni["Save"]["maxSaveCount"]) != "")
		{
			cfg.save.maxSaveCount = static_cast<size_t>(std::stoi(value));
		}
		else
		{
			cfg.save.maxSaveCount = DefaultConfig::maxSaveCount;
		}

		if ((value = cfgIni["Save"]["savePathFormat"]) != "")
		{
			cfg.save.pathFormat = value;
			cfg.save.getSavePath = [value](size_t index) {
				return std::filesystem::path(std::vformat(value, std::make_format_args(index)));
			};
		}
		else
		{
			cfg.save.pathFormat = DefaultConfig::savePath;
			cfg.save.getSavePath = [](size_t index) {
				return std::filesystem::path(std::format(DefaultConfig::savePath, index));
			};
		}

		{
			if ((value = cfgIni["Audio"]["volumes"]) == "")
			{
				value = DefaultConfig::audio_volumes;
			}

			cfg.audio.volumes.clear();
			size_t start = 0, end = 0;
			while ((end = value.find(',', start)) != std::string::npos)
			{
				cfg.audio.volumes.push_back(std::stof(value.substr(start, end - start)));
				start = end + 1;
			}
			if (start < value.size())
			{
				cfg.audio.volumes.push_back(std::stof(value.substr(start)));
			}
		}
	}

	void WriteVisualNovelConfig(const std::filesystem::path& path, const VisualNovelConfig& cfg)
	{
		mINI::INIFile iniFile{ path };
		mINI::INIStructure cfgIni;

		cfgIni["Graphics"]["fullScreen"] = cfg.win.fullScreen ? "true" : "false";
		cfgIni["Graphics"]["windowWidth"] = std::to_string(cfg.win.width);
		cfgIni["Graphics"]["windowHeight"] = std::to_string(cfg.win.height);
		cfgIni["Graphics"]["windowTitle"] = cfg.win.title;

		cfgIni["Text"]["textSpeed"] = std::format("{}", cfg.text.textSpeed);
		cfgIni["Text"]["textSize"] = std::format("{}", cfg.text.textSize);
		cfgIni["Text"]["fontPath"] = cfg.text.fontPath;

		cfgIni["I18n"]["mainLanguage"] = std::to_string(cfg.i18n.mainLanguage);
		cfgIni["I18n"]["secondaryLanguage"] = std::to_string(cfg.i18n.secondaryLanguage);
		cfgIni["I18n"]["uiLanguage"] = std::to_string(cfg.i18n.uiLanguage);

		cfgIni["Save"]["maxSaveCount"] = std::to_string(cfg.save.maxSaveCount);
		cfgIni["Save"]["savePathFormat"] = cfg.save.pathFormat;   // 新增字段

		{
			std::string volumesStr;
			for (size_t i = 0; i < cfg.audio.volumes.size(); ++i) {
				if (i != 0) volumesStr += ',';
				std::string s = std::format("{}", cfg.audio.volumes[i]);
				if (s.find('.') == std::string::npos)
					s += ".0";
				volumesStr += s;
			}
			cfgIni["Audio"]["volumes"] = volumesStr;
		}

		iniFile.write(cfgIni);
	}

	void CalculateVirtualScreen(VisualNovelConfig& cfg)
	{
		auto ScX = ebbglow::utils::ScreenSize().x;
		auto ScY = ebbglow::utils::ScreenSize().y;

		if (ScX / ScY > 16.0f / 9.0f)
		{
			cfg.virtualScreen.height = static_cast<int>(ScY);
			cfg.virtualScreen.width = static_cast<int>(ScY * (16.0f / 9.0f));
			cfg.virtualScreen.drawOffset = { (ScX - cfg.virtualScreen.width) / 2.0f, 0.0f };
			cfg.virtualScreen.drawRatio = static_cast<float>(cfg.virtualScreen.height) / 1080.0f;
		}
		else
		{
			cfg.virtualScreen.width = static_cast<int>(ScX);
			cfg.virtualScreen.height = static_cast<int>(ScX / (16.0f / 9.0f));
			cfg.virtualScreen.drawOffset = { 0.0f, (ScY - cfg.virtualScreen.height) / 2.0f };
			cfg.virtualScreen.drawRatio = static_cast<float>(cfg.virtualScreen.width) / 1920.0f;
		}
	}
}