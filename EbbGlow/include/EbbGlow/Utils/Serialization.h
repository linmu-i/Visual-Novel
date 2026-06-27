#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>
#include <deque>
#include <array>
#include <unordered_map>
#include <concepts>
#include <iostream>
#include <bit>
#include <filesystem>

namespace ebbglow::utils
{
	template<typename T>
	concept TrivialCopyable = std::is_trivially_copyable_v<T>;

	template<TrivialCopyable T>
	constexpr T ByteSwap(T data)
	{
		auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(data);
		for (size_t i = 0; i < bytes.size() / 2; ++i)
		{
			std::swap(bytes[i], bytes[bytes.size() - 1 - i]);
		}
		return std::bit_cast<T>(bytes);
	}

	template<TrivialCopyable T>
	constexpr T ToLittleEndian(T data)
	{
		if constexpr (std::endian::native == std::endian::big)
		{
			return ByteSwap(data);
		}
		else
		{
			return data;
		}
	}

	template<TrivialCopyable T>
	constexpr T FromLittleEndian(T data)
	{
		if constexpr (std::endian::native == std::endian::big)
		{
			return ByteSwap(data);
		}
		else
		{
			return data;
		}
	}

	template<TrivialCopyable T>
	constexpr T ToBigEndian(T data)
	{
		if constexpr (std::endian::native == std::endian::little)
		{
			return ByteSwap(data);
		}
		else
		{
			return data;
		}
	}

	template<TrivialCopyable T>
	constexpr T FromBigEndian(T data)
	{
		if constexpr (std::endian::native == std::endian::little)
		{
			return ByteSwap(data);
		}
		else
		{
			return data;
		}
	}

	template<typename T>
	concept BoolContext = requires(T t)
	{
		{ static_cast<bool>(t) };
	};

	template<typename OS>
	concept OutStream = requires(OS os, const char* data, uint64_t size)
	{
		{ os.write(data, size) } -> BoolContext;
	};

	template<typename IS>
	concept InStream = requires(IS is, char* data, uint64_t size)
	{
		{ is.read(data, size) } -> BoolContext;
	};

	template<OutStream OS, std::integral IntT>
	bool Serialize(OS& os, IntT value)
	{
		value = ToLittleEndian(value);
		return static_cast<bool>(os.write(reinterpret_cast<const char*>(&value), sizeof(value)));
	}

	template<OutStream OS, std::floating_point FloatT>
	bool Serialize(OS& os, FloatT value)
	{
		value = ToLittleEndian(value);
		return static_cast<bool>(os.write(reinterpret_cast<const char*>(&value), sizeof(value)));
	}

	template<OutStream OS>
	bool Serialize(OS& os, const std::string& value)
	{
		if (!Serialize(os, static_cast<uint64_t>(value.size())))
			return false;
		return static_cast<bool>(os.write(reinterpret_cast<const char*>(value.data()), static_cast<uint64_t>(value.size())));
	}

	template<OutStream OS>
	bool Serialize(OS& os, const std::u8string& value)
	{
		if (!Serialize(os, static_cast<uint64_t>(value.size())))
			return false;
		return static_cast<bool>(os.write(reinterpret_cast<const char*>(value.data()), static_cast<uint64_t>(value.size())));
	}

	template<OutStream OS>
	bool Serialize(OS& os, const std::filesystem::path& value)
	{
		std::u8string tmp = value.generic_u8string();
		if (!Serialize(os, static_cast<uint64_t>(tmp.size())))
			return false;
		return static_cast<bool>(os.write(reinterpret_cast<const char*>(tmp.data()), static_cast<uint64_t>(tmp.size())));
	}

	template<OutStream OS>
	bool Serialize(OS& os, const char* data, uint64_t size)
	{
		if (!data) return false;
		if (!Serialize(os, size))
			return false;
		return static_cast<bool>(os.write(data, size));
	}

	template<OutStream OS, typename Sequence>
	bool SerializeSequence(OS& os, const Sequence& seq)
	{
		if (!Serialize(os, static_cast<uint64_t>(seq.size())))
			return false;
		for (const auto& item : seq)
		{
			if (!Serialize(os, item))
				return false;
		}
		return true;
	}

	template<OutStream OS, typename DataT>
	bool Serialize(OS& os, const std::vector<DataT>& vec)
	{
		return SerializeSequence(os, vec);
	}

	template<OutStream OS, typename DataT>
	bool Serialize(OS& os, const std::list<DataT>& lst)
	{
		return SerializeSequence(os, lst);
	}

	template<OutStream OS, typename DataT>
	bool Serialize(OS& os, const std::set<DataT>& st)
	{
		return SerializeSequence(os, st);
	}

	template<OutStream OS, typename DataT>
	bool Serialize(OS& os, const std::deque<DataT>& deq)
	{
		return SerializeSequence(os, deq);
	}

	template<OutStream OS, typename DataT, size_t N>
	bool Serialize(OS& os, const std::array<DataT, N>& arr)
	{
		for (const auto& item : arr)
		{
			if (!Serialize(os, item))
				return false;
		}
		return true;
	}

	template<OutStream OS, typename KeyT, typename ValueT>
	bool Serialize(OS& os, const std::unordered_map<KeyT, ValueT>& map)
	{
		if (!Serialize(os, static_cast<uint64_t>(map.size())))
			return false;
		for (const auto& [key, value] : map)
		{
			if (!Serialize(os, key))
				return false;
			if (!Serialize(os, value))
				return false;
		}
		return true;
	}

	template<OutStream OS, typename FirstT, typename SecondT>
	bool Serialize(OS& os, const std::pair<FirstT, SecondT>& pair)
	{
		if (!Serialize(os, pair.first))
			return false;
		if (!Serialize(os, pair.second))
			return false;
		return true;
	}

	template<InStream IS, std::integral IntT>
	bool Deserialize(IS& is, IntT& value)
	{
		IntT temp = {};
		if (!is.read(reinterpret_cast<char*>(&temp), sizeof(temp))) return false;
		value = FromLittleEndian(temp);
		return true;
	}

	template<InStream IS, std::floating_point FloatT>
	bool Deserialize(IS& is, FloatT& value)
	{
		FloatT temp = {};
		if (!is.read(reinterpret_cast<char*>(&temp), sizeof(temp))) return false;
		value = FromLittleEndian(temp);
		return true;
	}

	template<InStream IS>
	bool Deserialize(IS& is, char* data, uint64_t* size, uint64_t maxSize)
	{
		if (!data || !size) return false;
		if (!Deserialize(is, *size))
		{
			*size = 0;
			return false;
		}
		if (*size > maxSize)
		{
			*size = 0;
			return false;
		}
		return static_cast<bool>(is.read(data, *size));
	}

	template<InStream IS>
	bool Deserialize(IS& is, std::string& str)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::string tmp;
		tmp.resize(size);
		if (!static_cast<bool>(is.read(reinterpret_cast<char*>(tmp.data()), size))) return false;
		str = std::move(tmp);
		return true;
	}

	template<InStream IS>
	bool Deserialize(IS& is, std::u8string& str)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::u8string tmp;
		tmp.resize(size);
		if (!static_cast<bool>(is.read(reinterpret_cast<char*>(tmp.data()), size))) return false;
		str = std::move(tmp);
		return true;
	}

	template<InStream IS>
	bool Deserialize(IS& is, std::filesystem::path& path)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::u8string tmp;
		tmp.resize(size);
		if (!static_cast<bool>(is.read(reinterpret_cast<char*>(tmp.data()), size))) return false;
		path = std::filesystem::path{ std::move(tmp) };
		return true;
	}

	template<InStream IS, typename DataT>
	bool Deserialize(IS& is, std::vector<DataT>& vec)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::vector<DataT> tmp;
		tmp.resize(size);
		for (auto& item : tmp)
		{
			if (!Deserialize(is, item))
				return false;
		}
		vec = std::move(tmp);
		return true;
	}

	template<InStream IS, typename DataT>
	bool Deserialize(IS& is, std::list<DataT>& lst)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::list<DataT> tmp;
		for (uint64_t i = 0; i < size; ++i)
		{
			DataT item = {};
			if (!Deserialize(is, item))
				return false;
			tmp.push_back(std::move(item));
		}
		lst = std::move(tmp);
		return true;
	}

	template<InStream IS, typename DataT>
	bool Deserialize(IS& is, std::set<DataT>& st)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::set<DataT> tmp;
		for (uint64_t i = 0; i < size; ++i)
		{
			DataT item = {};
			if (!Deserialize(is, item))
				return false;
			tmp.insert(std::move(item));
		}
		st = std::move(tmp);
		return true;
	}

	template<InStream IS, typename DataT>
	bool Deserialize(IS& is, std::deque<DataT>& deq)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::deque<DataT> tmp;
		for (uint64_t i = 0; i < size; ++i)
		{
			DataT item = {};
			if (!Deserialize(is, item))
				return false;
			tmp.push_back(std::move(item));
		}
		deq = std::move(tmp);
		return true;
	}

	template<InStream IS, typename DataT, size_t N>
	bool Deserialize(IS& is, std::array<DataT, N>& arr)
	{
		for (auto& item : arr)
		{
			if (!Deserialize(is, item))
				return false;
		}
		return true;
	}

	template<InStream IS, typename KeyT, typename ValueT>
	bool Deserialize(IS& is, std::unordered_map<KeyT, ValueT>& map)
	{
		uint64_t size = {};
		if (!Deserialize(is, size))
			return false;
		std::unordered_map<KeyT, ValueT> tmp;
		for (uint64_t i = 0; i < size; ++i)
		{
			KeyT key = {};
			ValueT value = {};
			if (!Deserialize(is, key))
				return false;
			if (!Deserialize(is, value))
				return false;
			tmp.emplace(std::move(key), std::move(value));
		}
		map = std::move(tmp);
		return true;
	}

	template<InStream IS, typename FirstT, typename SecondT>
	bool Deserialize(IS& is, std::pair<FirstT, SecondT>& pair)
	{
		FirstT tmpFirst = {};
		if (!Deserialize(is, tmpFirst))
			return false;
		SecondT tmpSecond = {};
		if (!Deserialize(is, tmpSecond))
			return false;
		pair.first = std::move(tmpFirst);
		pair.second = std::move(tmpSecond);
		return true;
	}
}