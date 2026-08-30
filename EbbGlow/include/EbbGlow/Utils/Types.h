#pragma once

#include <cstdint>
#include <cmath>
#include <concepts>
#include <numbers>
#include <algorithm>

namespace ebbglow
{
	template<std::integral T>
	struct Vec2I;

	template<std::floating_point T>
	struct Vec2F
	{
		T x;
		T y;

		constexpr Vec2F() noexcept : x(0), y(0) {}
		constexpr Vec2F(T x, T y) noexcept : x(x), y(y) {}

		template<std::convertible_to<T> T0, std::convertible_to<T> T1>
		constexpr Vec2F(T0 x, T1 y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

		template<std::integral U>
		constexpr Vec2F(Vec2I<U> vec2I);

		template<std::floating_point U>
		constexpr Vec2F(Vec2F<U> other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

		constexpr Vec2F& operator+=(Vec2F other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		[[nodiscard]] constexpr Vec2F operator+(Vec2F other) const noexcept
		{
			return Vec2F(x + other.x, y + other.y);
		}

		constexpr Vec2F& operator-=(Vec2F other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		[[nodiscard]] constexpr Vec2F operator-(Vec2F other) const noexcept
		{
			return Vec2F(x - other.x, y - other.y);
		}

		[[nodiscard]] constexpr Vec2F operator-() const noexcept
		{
			return Vec2F(-x, -y);
		}

		[[nodiscard]] constexpr Vec2F operator*(T scalar) const noexcept
		{
			return Vec2F(x * scalar, y * scalar);
		}
		constexpr Vec2F& operator*=(T scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		[[nodiscard]] constexpr Vec2F operator/(T scalar) const noexcept
		{
			return Vec2F(x / scalar, y / scalar);
		}
		constexpr Vec2F& operator/=(T scalar) noexcept
		{
			x /= scalar;
			y /= scalar;
			return *this;
		}

		[[nodiscard]] constexpr bool operator==(const Vec2F& other) const noexcept = default;
		[[nodiscard]] constexpr bool operator!=(const Vec2F& other) const noexcept = default;

		[[nodiscard]] constexpr T dot(Vec2F other) const noexcept
		{
			return x * other.x + y * other.y;
		}

		[[nodiscard]] constexpr T cross(Vec2F other) const noexcept
		{
			return x * other.y - y * other.x;
		}

		[[nodiscard]] T length() const noexcept
		{
			return std::sqrt(x * x + y * y);
		}

		[[nodiscard]] constexpr T lengthSqr() const noexcept
		{
			return x * x + y * y;
		}

		[[nodiscard]] Vec2F normalized() const noexcept
		{
			T len = length();
			if (len < T(1e-8)) return Vec2F(0, 0);
			return Vec2F(x / len, y / len);
		}

		[[nodiscard]] Vec2F rotated(T rad) const noexcept
		{
			T cosTheta = std::cos(rad);
			T sinTheta = std::sin(rad);
			return Vec2F(x * cosTheta - y * sinTheta, x * sinTheta + y * cosTheta);
		}
		[[nodiscard]] Vec2F rotatedAround(Vec2F pivot, T rad) const noexcept
		{
			Vec2F direction = *this - pivot;
			return pivot + direction.rotated(rad);
		}

		[[nodiscard]] constexpr Vec2F lerp(Vec2F other, T t) const noexcept
		{
			return Vec2F(x + (other.x - x) * t, y + (other.y - y) * t);
		}

		[[nodiscard]] T rad() const noexcept
		{
			return std::atan2(y, x);
		}

		[[nodiscard]] T deg() const noexcept
		{
			constexpr T factor = T(180) / std::numbers::pi_v<T>;
			return rad() * factor;
		}

		[[nodiscard]] T angleTo(Vec2F other) const noexcept
		{
			T dotProd = dot(other);
			T lenProd = length() * other.length();
			if (lenProd < T(1e-8)) return T(0);
			T cosTheta = dotProd / lenProd;
			cosTheta = std::clamp(cosTheta, T(-1), T(1));
			return std::acos(cosTheta);
		}
		[[nodiscard]] T signedAngleTo(Vec2F other) const noexcept
		{
			return std::atan2(cross(other), dot(other));
		}

		[[nodiscard]] Vec2F reflect(Vec2F normal) const noexcept
		{
			if (normal.lengthSqr() < T(1e-8)) return *this;
			normal = normal.normalized();
			return *this - normal * (T(2) * dot(normal));
		}

		[[nodiscard]] constexpr Vec2F projectOnto(Vec2F other) const noexcept
		{
			T otherLenSqr = other.lengthSqr();
			if (otherLenSqr < T(1e-8)) return Vec2F(0, 0);
			T projectionScale = dot(other) / otherLenSqr;
			return other * projectionScale;
		}

		static constexpr Vec2F Zero() noexcept { return Vec2F{ 0, 0 }; }
		static constexpr Vec2F XUnit() noexcept { return Vec2F{ 1, 0 }; }
		static constexpr Vec2F YUnit() noexcept { return Vec2F{ 0, 1 }; }
		static Vec2F Polar(T length, T rad) noexcept { return Vec2F{ length * std::cos(rad), length * std::sin(rad) }; }

	};

	template<std::floating_point T, std::convertible_to<T> U>
	[[nodiscard]] inline constexpr Vec2F<T> operator*(U a, Vec2F<T> b) noexcept
	{
		return b * static_cast<T>(a);
	}

	using Vec2 = Vec2F<float>;

	template<std::integral T>
	struct Vec2I
	{
		T x;
		T y;

		constexpr Vec2I() noexcept : x(0), y(0) {}
		constexpr Vec2I(T x, T y) noexcept : x(x), y(y) {}

		template<std::convertible_to<T> T0, std::convertible_to<T> T1>
		constexpr Vec2I(T0 x, T1 y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

		template<std::floating_point U>
		constexpr Vec2I(Vec2F<U> vec2F) noexcept : x(static_cast<T>(vec2F.x)), y(static_cast<T>(vec2F.y)) {}

		template<std::integral U>
		constexpr Vec2I(Vec2I<U> other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

		[[nodiscard]] constexpr bool operator==(const Vec2I& other) const noexcept = default;
		[[nodiscard]] constexpr bool operator!=(const Vec2I& other) const noexcept = default;

		template<std::floating_point U>
		[[nodiscard]] Vec2F<U> toVec2F() const noexcept
		{
			return Vec2F<U>(static_cast<U>(x), static_cast<U>(y));
		}

		template<std::floating_point U>
		[[nodiscard]] explicit operator Vec2F<U>() const noexcept
		{
			return toVec2F<U>();
		}

		[[nodiscard]] constexpr Vec2I operator+(Vec2I other) const noexcept
		{
			return Vec2I(x + other.x, y + other.y);
		}
		[[nodiscard]] constexpr Vec2I operator-(Vec2I other) const noexcept
		{
			return Vec2I(x - other.x, y - other.y);
		}
		[[nodiscard]] constexpr Vec2I operator*(T scalar) const noexcept
		{
			return Vec2I(x * scalar, y * scalar);
		}
		[[nodiscard]] constexpr Vec2I operator/(T scalar) const noexcept
		{
			return Vec2I(x / scalar, y / scalar);
		}
		[[nodiscard]] constexpr Vec2I operator-() const noexcept
		{
			return Vec2I(-x, -y);
		}

		constexpr Vec2I& operator+=(Vec2I other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}
		constexpr Vec2I& operator-=(Vec2I other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
		constexpr Vec2I& operator*=(T scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}
		constexpr Vec2I& operator/=(T scalar) noexcept
		{
			x /= scalar;
			y /= scalar;
			return *this;
		}

		constexpr Vec2I& operator++() noexcept
		{
			++x;
			++y;
			return *this;
		}

		constexpr Vec2I& operator--() noexcept
		{
			--x;
			--y;
			return *this;
		}

		constexpr Vec2I operator++(int) noexcept
		{
			Vec2I temp = *this;
			++(*this);
			return temp;
		}

		constexpr Vec2I operator--(int) noexcept
		{
			Vec2I temp = *this;
			--(*this);
			return temp;
		}

		constexpr Vec2I operator%(T scalar) const noexcept
		{
			return Vec2I(x % scalar, y % scalar);
		}

		constexpr Vec2I& operator%=(T scalar) noexcept
		{
			x %= scalar;
			y %= scalar;
			return *this;
		}

		template<std::floating_point U>
		[[nodiscard]] constexpr U lengthSqr() const noexcept
		{
			return static_cast<U>(x) * static_cast<U>(x) + static_cast<U>(y) * static_cast<U>(y);
		}
		template<std::floating_point U>
		[[nodiscard]] U length() const noexcept
		{
			return std::sqrt(lengthSqr<U>());
		}

		template<std::floating_point U>
		[[nodiscard]] U distantTo(Vec2I other) const noexcept
		{
			return (*this - other).length<U>();
		}

		template<std::floating_point U>
		[[nodiscard]] Vec2F<U> normalized() const noexcept
		{
			return Vec2F<U>(static_cast<U>(x), static_cast<U>(y)).normalized();
		}

		static constexpr Vec2I Zero() noexcept { return Vec2I{ 0, 0 }; }
		static constexpr Vec2I XUnit() noexcept { return Vec2I{ 1, 0 }; }
		static constexpr Vec2I YUnit() noexcept { return Vec2I{ 0, 1 }; }
	};

	template<std::floating_point T>
	template<std::integral U>
	constexpr Vec2F<T>::Vec2F(Vec2I<U> vec2I)
	{
		x = static_cast<T>(vec2I.x);
		y = static_cast<T>(vec2I.y);
	}

}

namespace std
{
	template<std::floating_point T>
	struct hash<ebbglow::Vec2F<T>>
	{
		size_t operator()(const ebbglow::Vec2F<T>& vec) const noexcept
		{
			size_t h1 = std::hash<T>{}(vec.x);
			size_t h2 = std::hash<T>{}(vec.y);
			return h1 ^ (h2 << 1);
		}
	};
	template<std::integral T>
	struct hash<ebbglow::Vec2I<T>>
	{
		size_t operator()(const ebbglow::Vec2I<T>& vec) const noexcept
		{
			size_t h1 = std::hash<T>{}(vec.x);
			size_t h2 = std::hash<T>{}(vec.y);
			return h1 ^ (h2 << 1);
		}
	};
}

namespace ebbglow
{

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