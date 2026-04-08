#pragma once

#include <cstdint>
#include <cmath>

namespace ebbglow
{
	struct Vec2
	{
		float x;
		float y;

		constexpr Vec2() noexcept : x(0.0f), y(0.0f) {}
		constexpr Vec2(float x, float y) noexcept : x(x), y(y) {}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(x * x + y * y);
		}
		
		[[nodiscard]] constexpr float lengthSqr() const noexcept
		{
			return x * x + y * y;
		}

		[[nodiscard]] Vec2 normalized() const noexcept
		{
			float len = length();
			if (len < 1e-8f) return Vec2(0.0f, 0.0f);
			return Vec2(x / len, y / len);
		}
	};

	struct ColorR8G8B8A8
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		constexpr ColorR8G8B8A8() : r(0), g(0), b(0), a(0) {}
		constexpr ColorR8G8B8A8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
	};

	using Color = ColorR8G8B8A8;

	struct ColorHSVA
	{
		float h;
		float s;
		float v;
		float a;

		constexpr ColorHSVA() : h(0.0f), s(0.0f), v(0.0f), a(0.0f) {}
		constexpr ColorHSVA(float h, float s, float v, float a) : h(h), s(s), v(v), a(a) {}
	};

	struct Rect
	{
		float x;
		float y;
		float width;
		float height;

		constexpr Rect() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
		constexpr Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
		constexpr Rect(Vec2 position, Vec2 coverage) : x(position.x), y(position.y), width(coverage.x), height(coverage.y) {}

		[[nodiscard]] constexpr bool contain(const Vec2& point) const noexcept
		{
			return (point.x > x && point.y > y && point.x < x + width && point.y < y + height);
		}
		[[nodiscard]] constexpr bool contain(const Rect& other) const noexcept
		{
			return other.x >= x &&
				other.y >= y &&
				other.x + other.width <= x + width &&
				other.y + other.height <= y + height;
		}
		[[nodiscard]] constexpr bool intersect(const Rect& other) const noexcept
		{
			return !(other.x + other.width <= x ||
				other.x >= x + width ||
				other.y + other.height <= y ||
				other.y >= y + height);
		}
		[[nodiscard]] constexpr Vec2 position() const noexcept
		{
			return { x, y };
		}
		[[nodiscard]] constexpr Vec2 coverage() const noexcept
		{
			return { width, height };
		}
		[[nodiscard]] constexpr Rect offsetOf(Vec2 offset) const noexcept
		{
			return Rect{ x + offset.x, y + offset.y, width, height };
		}
		[[nodiscard]] constexpr Rect extendOf(Vec2 extension) const noexcept
		{
			return Rect{ x , y , width + extension.x, height + extension.y };
		}
		[[nodiscard]] constexpr float area() const noexcept
		{
			return  width * height;
		}
		[[nodiscard]] constexpr Vec2 center() const noexcept
		{
			return  Vec2{ x + width * 0.5f, y + height * 0.5f };
		}
		[[nodiscard]] constexpr bool operator==(const Rect& other) const noexcept
		{
			return  x == other.x && y == other.y && width == other.width && height == other.height;
		}
	};

	namespace colors
	{
		constexpr ColorR8G8B8A8 White = { 255, 255, 255, 255 };
		constexpr ColorR8G8B8A8 Black = { 0, 0, 0, 255 };
		constexpr ColorR8G8B8A8 Blank = { 0, 0, 0, 0 };
	}

	struct Camera2D
	{
		Vec2 offset;
		Vec2 target;
		float rotation;
		float zoom;

		Camera2D() : offset(Vec2{ 0.0f, 0.0f }), target(Vec2{ 0.0f, 0.0f }), rotation(0.0f), zoom(1.0f) {}
		Camera2D(Vec2 offset, Vec2 target, float rotation, float zoom) : offset(offset), target(target), rotation(rotation), zoom(zoom) {}
	};
}