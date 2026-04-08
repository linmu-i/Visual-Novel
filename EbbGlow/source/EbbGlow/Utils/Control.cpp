#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <raylib.h>

#include <EbbGlow/Utils/Control.h>
#include <EbbGlow/Utils/Input.h>
//#include <glad/glad.h>

#include "RLTypesCast.h"

namespace ebbglow
{
	static unsigned char* u8LoadFileData(const char* u8FileName, int* dataSize)
	{
		*dataSize = 0;
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
			data[size] = '\0';
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
		::SetExitKey(::KEY_NULL);
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

	static std::vector<rsc::SharedRenderTexture2D>& GetRenderTextureStack()
	{
		thread_local static std::vector<rsc::SharedRenderTexture2D> stack;
		return stack;
	}

	void BeginTextureMode(rsc::SharedRenderTexture2D& renderTexture)
	{
		auto& stack = GetRenderTextureStack();
		if (!stack.empty()) ::EndTextureMode();
		stack.push_back(renderTexture);
		::BeginTextureMode(*(RenderTexture*)renderTexture.get());
	}

	void EndTextureMode()
	{
		auto& stack = GetRenderTextureStack();
		::EndTextureMode();
		if (!stack.empty())
		{
			stack.pop_back();
			if (!stack.empty())
			{
				::BeginTextureMode(*(RenderTexture*)stack.back().get());
			}
		}
	}

	void ClearTextureMode()
	{
		auto& stack = GetRenderTextureStack();
		if (!stack.empty())
		{
			::EndTextureMode();
			stack.clear();
		}
	}

	static std::vector<rsc::SharedShader>& GetShaderStack()
	{
		thread_local static std::vector<rsc::SharedShader> stack;
		return stack;
	}

	void BeginShaderMode(rsc::SharedShader& shader)
	{
		auto& stack = GetShaderStack();
		if (!stack.empty()) ::EndShaderMode();
		stack.push_back(shader);
		::BeginShaderMode(*(Shader*)shader.get());
	}

	void EndShaderMode()
	{
		auto& stack = GetShaderStack();
		::EndShaderMode();
		if (!stack.empty())
		{
			stack.pop_back();
			if (!stack.empty())
			{
				::BeginShaderMode(*(Shader*)stack.back().get());
			}
		}
	}

	void ClearShaderMode()
	{
		auto& stack = GetShaderStack();
		if (!stack.empty())
		{
			::EndShaderMode();
			stack.clear();
		}
	}

	static std::vector<Camera2D>& GetCamera2DStack()
	{
		thread_local static std::vector<Camera2D> stack;
		return stack;
	}

	void BeginMode2D(const Camera2D& camera2D)
	{
		auto& stack = GetCamera2DStack();
		if (!stack.empty()) ::EndMode2D();
		stack.push_back(camera2D);
		::BeginMode2D(RLCamera2D(camera2D));
	}

	void EndMode2D()
	{
		auto& stack = GetCamera2DStack();
		::EndMode2D();
		if (!stack.empty())
		{
			stack.pop_back();
			if (!stack.empty())
			{
				::BeginMode2D(RLCamera2D(stack.back()));
			}
		}
	}

	static std::vector<Rect>& GetScissorStack()
	{
		thread_local static std::vector<Rect> stack;
		return stack;
	}

	void BeginScissorMode(Rect region)
	{
		auto& stack = GetScissorStack();
		if (!stack.empty()) ::EndScissorMode();
		stack.push_back(region);
		::BeginScissorMode(region.x, region.y, region.width, region.height);
	}

	void EndScissorMode()
	{
		auto& stack = GetScissorStack();
		::EndScissorMode();
		if (!stack.empty())
		{
			stack.pop_back();
			if (!stack.empty())
			{
				auto& region = stack.back();
				::BeginScissorMode(region.x, region.y, region.width, region.height);
			}
		}
	}

	float GetFrameTime()
	{
		return ::GetFrameTime();
	}
}