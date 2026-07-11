#pragma once

#include <cstdint>
#include <cmath>
#include <concepts>
#include <numbers>
#include <algorithm>

namespace ebbglow
{
	struct Vec2
	{
		float x;
		float y;

		constexpr Vec2() noexcept : x(0.0f), y(0.0f) {}
		constexpr Vec2(float x, float y) noexcept : x(x), y(y) {}

		template<std::convertible_to<float> T0, std::convertible_to<float> T1>
		constexpr Vec2(T0 x, T1 y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}

		constexpr Vec2& operator+=(Vec2 other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		[[nodiscard]] constexpr Vec2 operator+(Vec2 other) const noexcept
		{
			return Vec2(x + other.x, y + other.y);
		}

		constexpr Vec2& operator-=(Vec2 other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		[[nodiscard]] constexpr Vec2 operator-(Vec2 other) const noexcept
		{
			return Vec2(x - other.x, y - other.y);
		}

		[[nodiscard]] constexpr Vec2 operator-() const noexcept
		{
			return Vec2(-x, -y);
		}

		[[nodiscard]] constexpr Vec2 operator*(float scalar) const noexcept
		{
			return Vec2(x * scalar, y * scalar);
		}
		constexpr Vec2& operator*=(float scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		[[nodiscard]] constexpr Vec2 operator/(float scalar) const noexcept
		{
			return Vec2(x / scalar, y / scalar);
		}
		constexpr Vec2& operator/=(float scalar) noexcept
		{
			x /= scalar;
			y /= scalar;
			return *this;
		}

		[[nodiscard]] constexpr bool operator==(const Vec2& other) const noexcept = default;
		[[nodiscard]] constexpr bool operator!=(const Vec2& other) const noexcept = default;

		[[nodiscard]] constexpr float dot(Vec2 other) const noexcept
		{
			return x * other.x + y * other.y;
		}

		[[nodiscard]] constexpr float cross(Vec2 other) const noexcept
		{
			return x * other.y - y * other.x;
		}

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

		[[nodiscard]] Vec2 rotated(float rad) const noexcept
		{
			float cosTheta = std::cos(rad);
			float sinTheta = std::sin(rad);
			return Vec2(x * cosTheta - y * sinTheta, x * sinTheta + y * cosTheta);
		}
		[[nodiscard]] Vec2 rotatedAround(Vec2 pivot, float rad) const noexcept
		{
			Vec2 direction = *this - pivot;
			return pivot + direction.rotated(rad);
		}

		[[nodiscard]] constexpr Vec2 lerp(Vec2 other, float t) const noexcept
		{
			return Vec2(x + (other.x - x) * t, y + (other.y - y) * t);
		}

		[[nodiscard]] float rad() const noexcept
		{
			return std::atan2(y, x);
		}

		[[nodiscard]] float deg() const noexcept
		{
			constexpr float factor = 180.0f / std::numbers::pi_v<float>;
			return rad() * factor;
		}

		[[nodiscard]] float angleTo(Vec2 other) const noexcept
		{
			float dotProd = dot(other);
			float lenProd = length() * other.length();
			if (lenProd < 1e-8f) return 0.0f;
			float cosTheta = dotProd / lenProd;
			cosTheta = std::clamp(cosTheta, -1.0f, 1.0f);
			return std::acos(cosTheta);
		}
		[[nodiscard]] float signedAngleTo(Vec2 other) const noexcept
		{
			return std::atan2(cross(other), dot(other));
		}

		[[nodiscard]] Vec2 reflect(Vec2 normal) const noexcept
		{
			if (normal.lengthSqr() < 1e-8f) return *this;
			normal = normal.normalized();
			return *this - normal * (2.0f * dot(normal));
		}

		[[nodiscard]] Vec2 projectOnto(Vec2 other) const noexcept
		{
			float otherLenSqr = other.lengthSqr();
			if (otherLenSqr < 1e-8f) return Vec2(0.0f, 0.0f);
			float projectionScale = dot(other) / otherLenSqr;
			return other * projectionScale;
		}

		static constexpr Vec2 Zero() noexcept { return Vec2{ 0.0f, 0.0f }; }
		static constexpr Vec2 XUnit() noexcept { return Vec2{ 1.0f, 0.0f }; }
		static constexpr Vec2 YUnit() noexcept { return Vec2{ 0.0f, 1.0f }; }
		static Vec2 Polar(float length, float rad) noexcept { return Vec2{ length * std::cos(rad), length * std::sin(rad) }; }

	};

	[[nodiscard]] inline constexpr Vec2 operator*(float a, Vec2 b) noexcept
	{
		return b * a;
	}

	struct ColorR8G8B8A8
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		constexpr ColorR8G8B8A8() : r(0), g(0), b(0), a(0) {}
		constexpr ColorR8G8B8A8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
		constexpr ColorR8G8B8A8(uint32_t color) : r((color >> 24) & 0xff), g((color >> 16) & 0xff), b((color >> 8) & 0xff), a(color & 0xff) {}
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

		template<std::convertible_to<float> T0, std::convertible_to<float> T1, std::convertible_to<float> T2, std::convertible_to<float> T3>
		constexpr Rect(T0 x, T1 y, T2 width, T3 height) : x(static_cast<float>(x)), y(static_cast<float>(y)), width(static_cast<float>(width)), height(static_cast<float>(height)) {}

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
		[[nodiscard]] constexpr Rect intersection(const Rect& other) const noexcept
		{
			if (!intersect(other)) return {};
			return Rect
			{
				std::max(x, other.x),
				std::max(y, other.y),
				std::min(x + width, other.x + other.width) - std::max(x, other.x),
				std::min(y + height, other.y + other.height) - std::max(y, other.y)
			};
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
		[[nodiscard]] constexpr Rect scaleOf(float scale) const noexcept
		{
			return Rect{ x , y , width * scale, height * scale };
		}
		[[nodiscard]] constexpr Rect scaleAround(Vec2 pivot, float scale) const noexcept
		{
			Vec2 newPos = pivot + (position() - pivot) * scale;
			return Rect{ newPos.x, newPos.y, width * scale, height * scale };
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