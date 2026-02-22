#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Input.h>
//#include <glad/glad.h>

#include "RLTypesCast.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <raylib.h>

namespace ebbglow
{
	static unsigned char* u8LoadFileData(const char* u8FileName, int* dataSize)
	{
		if (u8FileName == nullptr) return nullptr;
		std::filesystem::path filePath(reinterpret_cast<const char8_t*>(u8FileName));
		
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (!file.is_open()) return nullptr;

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		unsigned char* data = static_cast<unsigned char*>(RL_MALLOC(static_cast<size_t>(size)));
		if (!data) return nullptr;

		if (file.read(reinterpret_cast<char*>(data), size))
		{
			*dataSize = static_cast<int>(size);
			return data;
		}

		RL_FREE(data);
		return nullptr;
	}

	static char* u8LoadFileText(const char* u8FileName)
	{
		if (u8FileName == nullptr) return nullptr;
		std::filesystem::path filePath(reinterpret_cast<const char8_t*>(u8FileName));

		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (!file.is_open()) return nullptr;

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		char* data = static_cast<char*>(RL_MALLOC(static_cast<size_t>(size) + 1));
		if (!data) return nullptr;

		if (file.read(reinterpret_cast<char*>(data), size))
		{
			data[size] = '\0'; // Null-terminate the text
			return data;
		}
		RL_FREE(data);
		return nullptr;
	}

	static bool u8SaveFileData(const char* u8FileName, void* data, int dataSize)
	{
		if (u8FileName == nullptr || data == nullptr || dataSize <= 0) return false;
		std::filesystem::path filePath(reinterpret_cast<const char8_t*>(u8FileName));
		std::ofstream file(filePath, std::ios::binary);
		if (!file.is_open()) return false;
		file.write(reinterpret_cast<const char*>(data), dataSize);
		return file.good();
	}

	static bool u8SaveFileText(const char* u8FileName, char* text)
	{
		if (u8FileName == nullptr || text == nullptr) return false;
		std::filesystem::path filePath(reinterpret_cast<const char8_t*>(u8FileName));
		std::ofstream file(filePath);
		if (!file.is_open()) return false;
		file << text;
		return file.good();
	}
	
	void Init(int width, int height, const std::string& title)
	{
		::InitWindow(width, height, title.c_str());
		::InitAudioDevice();
		input::InitInput();
		::SetLoadFileDataCallback(u8LoadFileData);
		::SetLoadFileTextCallback(u8LoadFileText);
		::SetSaveFileDataCallback(u8SaveFileData);
		::SetSaveFileTextCallback(u8SaveFileText);
	}

	bool WindowShouldClose() noexcept
	{
		return ::WindowShouldClose();
	}
	void SetConfigFlag(uint32_t flags)
	{
		::SetConfigFlags(flags);
	}
	void SetTargetFPS(int32_t fps)
	{
		::SetTargetFPS(fps);
	}

	void BeginDrawing()
	{
		::BeginDrawing();
	}
	void EndDrawing()
	{
		input::SwapActiveInputBuffers();
		::EndDrawing();
	}

	void BeginTextureMode(rsc::SharedRenderTexture2D renderTexture)
	{
		::BeginTextureMode(*(RenderTexture*)renderTexture.get());
	}
	void EndTextureMode()
	{
		::EndTextureMode();
	}

	void BeginShaderMode(rsc::SharedShader shader)
	{
		::BeginShaderMode(*(Shader*)shader.get());
	}
	void EndShaderMode()
	{
		::EndShaderMode();
	}

	void BeginMode2D(const Camera2D& camera2D)
	{
		::BeginMode2D(RLCamera2D(camera2D));
	}
	void EndMode2D()
	{
		::EndMode2D();
	}

	float GetFrameTime()
	{
		return ::GetFrameTime();
	}
}