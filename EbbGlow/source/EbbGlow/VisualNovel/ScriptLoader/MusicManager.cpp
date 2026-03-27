#include <EbbGlow/VisualNovel/ScriptLoader/MusicManager.h>

#include <raylib.h>

namespace ebbglow::visualnovel
{
	void MusicManager::update()
	{
		if (bgm.valid())
		{
			UpdateMusicStream(*static_cast<Music*>(bgm.get()));
		}
	}

	void MusicManager::SetBgm(const rsc::SharedMusic& music, float volume, const std::string& path, uint16_t volumeIndex) noexcept
	{
		if (bgm.valid())
		{
			StopMusicStream(*static_cast<Music*>(bgm.get()));
		}
		bgm = music;
		if (bgm.valid())
		{
			SetMusicVolume(*static_cast<Music*>(bgm.get()), volume);
			PlayMusicStream(*static_cast<Music*>(bgm.get()));
		}

		bgmPath = path;
		bgmVolumeIndex = volumeIndex;
	}

	void MusicManager::SetVoice(const rsc::SharedSound& sound, float volume, const std::string& path, uint16_t volumeIndex) noexcept
	{
		if (voice.valid())
		{
			StopSound(*static_cast<Sound*>(voice.get()));
		}
		voice = sound;
		if (voice.valid())
		{
			SetSoundVolume(*static_cast<Sound*>(voice.get()), volume);
			PlaySound(*static_cast<Sound*>(voice.get()));
		}

		voicePath = path;
		voiceVolumeIndex = volumeIndex;
	}

	void MusicManager::StopBgm() noexcept
	{
		if (bgm.valid())
		{
			StopMusicStream(*static_cast<Music*>(bgm.get()));
			bgm = rsc::SharedMusic{};
		}
	}

	void MusicManager::StopVoice() noexcept
	{
		if (voice.valid())
		{
			StopSound(*static_cast<Sound*>(voice.get()));
			voice = rsc::SharedSound{};
		}
	}

	void MusicManager::PauseBgm() noexcept
	{
		if (bgm.valid())
		{
			PauseMusicStream(*static_cast<Music*>(bgm.get()));
		}
	}

	void MusicManager::ResumeBgm() noexcept
	{
		if (bgm.valid())
		{
			ResumeMusicStream(*static_cast<Music*>(bgm.get()));
		}
	}

	void MusicManager::PauseVoice() noexcept
	{
		if (voice.valid())
		{
			PauseSound(*static_cast<Sound*>(voice.get()));
		}
	}

	void MusicManager::ResumeVoice() noexcept
	{
		if (voice.valid())
		{
			ResumeSound(*static_cast<Sound*>(voice.get()));
		}
	}
}