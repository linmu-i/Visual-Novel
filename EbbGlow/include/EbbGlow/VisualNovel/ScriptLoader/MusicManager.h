#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>

namespace ebbglow::visualnovel
{
	class MusicManager : public core::SystemBase
	{
	private:
		const VisualNovelConfig& cfg;

		rsc::SharedMusic bgm;
		std::string bgmPath;
		std::uint16_t bgmVolumeIndex;
		rsc::SharedSound voice;
		std::string voicePath;
		std::uint16_t voiceVolumeIndex;

	public:
		MusicManager(const VisualNovelConfig& config) : cfg(config), bgmVolumeIndex(0), voiceVolumeIndex(0) {}

		void update() override;
		void SetBgm(const rsc::SharedMusic& music, float volume, const std::string& path, uint16_t volumeIndex) noexcept;
		void SetVoice(const rsc::SharedSound& sound, float volume, const std::string& path, uint16_t volumeIndex) noexcept;
		void StopBgm() noexcept;
		void StopVoice() noexcept;
		void PauseBgm() noexcept;
		void PauseVoice() noexcept;
		void ResumeBgm() noexcept;
		void ResumeVoice() noexcept;
		const std::string& getBgmPath() const noexcept { return bgmPath; }
		const uint16_t getBgmVolumeIndex() const noexcept { return bgmVolumeIndex; }
		const std::string& getVoicePath() const noexcept { return voicePath; }
		const uint16_t getVoiceVolumeIndex() const noexcept { return voiceVolumeIndex; }
	};
}