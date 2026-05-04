#pragma once

#include <string>
#include <atomic>
#include <span>

#include <EbbGlow/Utils/Types.h>
#include <EbbGlow/Utils/Serialization.h>

namespace ebbglow::resource
{
	class SharedImage
	{
	private:
		void* image;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedImage() noexcept;
		SharedImage(const char* imagePath) noexcept;
		SharedImage(std::u8string_view imagePath) noexcept;
		SharedImage(const SharedImage& other);
		SharedImage(SharedImage&& other) noexcept;
		~SharedImage();

		SharedImage& operator=(const SharedImage& other);
		SharedImage& operator=(SharedImage&& other) noexcept;

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		int width() const noexcept;
		int height() const noexcept;

		void* get() noexcept { return image; }
		void* get() const noexcept { return image; }
	};

	class SharedTexture
	{
	private:
		void* texture;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedTexture() noexcept;
		SharedTexture(const char* texturePath) noexcept;
		SharedTexture(std::u8string_view texturePath) noexcept;
		SharedTexture(const SharedTexture& other);
		SharedTexture(SharedTexture&& other) noexcept;
		SharedTexture(const SharedImage& img) noexcept;
		~SharedTexture();

		SharedTexture& operator=(const SharedTexture& other);
		SharedTexture& operator=(SharedTexture&& other) noexcept;

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		int width() const noexcept;
		int height() const noexcept;
		Vec2 size() const noexcept;

		void* get() noexcept { return texture; }
		void* get() const noexcept { return texture; }
	};

	using SharedTexture2D = SharedTexture;

	enum class FontType : uint8_t
	{
		Default,
		Sdf
	};

	class SharedFont
	{
	private:
		void* font;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedFont() noexcept;
		SharedFont(const char* fontPath) noexcept;
		SharedFont(const SharedFont& other);
		SharedFont(SharedFont&& other) noexcept;
		~SharedFont();

		SharedFont& operator=(const SharedFont& other);
		SharedFont& operator=(SharedFont&& other) noexcept;

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		void* get() noexcept { return font; }
		void* get() const noexcept { return font; }
	};

	class SharedMusic
	{
	private:
		void* music;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedMusic() noexcept;
		SharedMusic(const SharedMusic& other) noexcept;
		SharedMusic(SharedMusic&& other) noexcept;
		SharedMusic(const char* musicPath) noexcept;
		SharedMusic(std::u8string_view musicPath) noexcept;
		
		SharedMusic& operator=(const SharedMusic& other) noexcept;
		SharedMusic& operator=(SharedMusic&& other) noexcept;
		~SharedMusic();

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		void* get() noexcept { return music; }
		void* get() const noexcept { return music; }
	};

	class SharedShader
	{
	private:
		void* shader;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedShader() noexcept;
		SharedShader(const SharedShader& other) noexcept;
		SharedShader(SharedShader&& other) noexcept;
		SharedShader(const char* shaderPath) noexcept;

		SharedShader& operator=(const SharedShader& other) noexcept;
		SharedShader& operator=(SharedShader&& other) noexcept;
		~SharedShader();

		operator bool() const noexcept { return ref != nullptr; }
		bool valid() const noexcept { return ref != nullptr; }

		void* get() noexcept { return shader; }
		void* get() const noexcept { return shader; }
	};

	class SharedFile
	{
	private:
		unsigned char* fileData;
		std::atomic<size_t>* ref;
		int dataSize;
		char* name;

		friend class ResourceCreator;

	public:
		class Iterator
		{
		private:
			size_t fileSize;
			int64_t offset;
			unsigned char* pointer;

		public:
			Iterator();
			Iterator(size_t size, unsigned char* data, int64_t offset = 0);
			Iterator(const SharedFile& file);
			Iterator(const SharedFile& file, int64_t offset);

			Iterator(const Iterator& other);
			Iterator(Iterator&& other) noexcept;

			Iterator& operator=(const Iterator& other) noexcept;
			Iterator& operator=(Iterator&& other) noexcept;

			Iterator& operator++() noexcept;
			Iterator operator++(int) noexcept;
			Iterator& operator--() noexcept;
			Iterator operator--(int) noexcept;

			unsigned char& operator*() noexcept;
			const unsigned char& operator*() const noexcept;
			unsigned char& operator[](int64_t _offset) noexcept;
			const unsigned char& operator[](int64_t _offset) const noexcept;

			bool eof() const noexcept;
			bool valid() const noexcept;
			void reset() noexcept;
			explicit operator bool() const noexcept;

			Iterator& operator+=(int64_t _offset) noexcept;
			Iterator& operator-=(int64_t _offset) noexcept;
			Iterator operator+(int64_t _offset) const noexcept;
			Iterator operator-(int64_t _offset) const noexcept;
			int64_t operator-(const Iterator& other) const noexcept;

			bool operator==(const Iterator& other) const noexcept;
			bool operator>(const Iterator& other) const noexcept;
			bool operator>=(const Iterator& other) const noexcept;
			bool operator<(const Iterator& other) const noexcept;
			bool operator<=(const Iterator& other) const noexcept;
			bool operator!=(const Iterator& other) const noexcept;

			size_t size() const noexcept;
			size_t remaining() const noexcept;
			int64_t position() const noexcept;
			unsigned char* get() const noexcept;
		};

		SharedFile() noexcept;
		SharedFile(const char* filePath) noexcept;
		SharedFile(unsigned char* fileData, int dataSize, const char* name) noexcept;//由new分配
		SharedFile(std::u8string_view filePath) noexcept;
		SharedFile(const SharedFile& other) noexcept;
		SharedFile(SharedFile&& other) noexcept;

		SharedFile& operator=(const SharedFile& other) noexcept;
		SharedFile& operator=(SharedFile&& other) noexcept;
		~SharedFile() noexcept;

		unsigned char* get() const noexcept { return fileData; }
		int size() const noexcept { return dataSize; }
		const char* fileName() const noexcept { return name; }
		operator bool() const noexcept { return ref; }
		bool valid() const noexcept { return ref; }

		Iterator begin();
		Iterator end();
	};

	class SharedRenderTexture
	{
	private:
		void* renderTexture;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedRenderTexture() noexcept;
		SharedRenderTexture(const SharedRenderTexture& other) noexcept;
		SharedRenderTexture(SharedRenderTexture&& other) noexcept;
		SharedRenderTexture(int width, int height) noexcept;

		SharedRenderTexture& operator=(const SharedRenderTexture& other) noexcept;
		SharedRenderTexture& operator=(SharedRenderTexture&& other) noexcept;
		~SharedRenderTexture();

		operator bool() const noexcept { return ref; }
		bool valid() const noexcept { return ref; }

		int width() const noexcept;
		int height() const noexcept;
		Vec2 size() const noexcept;

		void* get() noexcept { return renderTexture; }
		void* get() const noexcept { return renderTexture; }
	};

	typedef SharedRenderTexture SharedRenderTexture2D;

	class SharedSound
	{
	private:
		void* sound;
		std::atomic<size_t>* ref;

		friend class ResourceCreator;

	public:
		SharedSound() noexcept;
		SharedSound(const SharedSound& other) noexcept;
		SharedSound(SharedSound&& other) noexcept;
		SharedSound(const char* soundPath) noexcept;
		SharedSound(std::u8string_view soundPath) noexcept;

		SharedSound& operator=(const SharedSound& other) noexcept;
		SharedSound& operator=(SharedSound&& other) noexcept;
		~SharedSound();

		operator bool() const noexcept { return ref; }
		bool valid() const noexcept { return ref; }

		void* get() noexcept { return sound; }
		void* get() const noexcept { return sound; }
	};



	class BufferOS
	{
	private:
		std::vector<char> buffer;
		uint64_t dataSize_;
	public:
		BufferOS(uint64_t size) : buffer(size), dataSize_(0) {}
		bool write(const char* data, uint64_t size)
		{
			if (size > buffer.size())
			{
				dataSize_ = 0;
				return false;
			}
			dataSize_ = size;
			std::copy(data, data + size, buffer.data());
			return true;
		}
		const char* data() const
		{
			return buffer.data();
		}

		uint64_t dataSize() const
		{
			return dataSize_;
		}

		std::span<char> span()
		{
			return std::span<char>(buffer.data(), dataSize_);
		}
	};

	class BufferIS
	{
	private:
		std::span<const char> buffer;
	public:
		BufferIS() = default;
		void set(const char* data, uint64_t size)
		{
			buffer = std::span<const char>(data, size);
		}
		bool read(char* buf, uint64_t size)
		{
			if (size > buffer.size()) return false;
			memcpy(buf, buffer.data(), size);
			return true;
		}
	};

	SharedImage CreateImageFromData(const std::vector<char>& data);

	struct ImageDataIterator
	{
		uint64_t index = {};
		std::span<char> data;
		SharedImage img;
		BufferOS buffer{ 16 };

		ImageDataIterator& operator++();
		ImageDataIterator operator++(int);
		bool operator==(const ImageDataIterator& other) const;
		bool operator!=(const ImageDataIterator& other) const;
		std::span<char>& operator*() { return data; }
		const std::span<char>& operator*() const { return data; }
	};

	class ImageDataRange
	{
	private:
		SharedImage img;

	public:
		ImageDataRange(const SharedImage& image) : img(image) {}
		ImageDataIterator begin() const;
		ImageDataIterator end() const;
	};

	template<utils::OutStream OS>
	bool Serialize(OS& os, const SharedImage& image)
	{
		if (!image.valid()) return false;
		ImageDataRange range(image);
		for (auto& data : range)
		{
			os.write(data.data(), data.size());
		}
	}

	template<utils::InStream IS>
	bool Deserialize(IS& is, SharedImage& image)
	{
		std::vector<char> data = {};
		if (!utils::Deserialize(is, data)) return false;
		if (data.empty()) return false;
		image = CreateImageFromData(data);
		return true;
	}
}

namespace ebbglow
{
	namespace rsc = resource;
}