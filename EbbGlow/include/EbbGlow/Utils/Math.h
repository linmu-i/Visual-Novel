#pragma once

#include <cmath>
#include <algorithm>
#include <numbers>
#include <concepts>

namespace ebbglow
{
	template<std::floating_point Float>
	[[nodiscard]] constexpr Float DegToRad(Float deg) noexcept
	{
		constexpr Float factor = std::numbers::pi_v<Float> / static_cast<Float>(180);
		return deg * factor;
	}

	template<std::floating_point Float>
	[[nodiscard]] constexpr Float RadToDeg(Float rad) noexcept
	{
		constexpr Float factor = static_cast<Float>(180) / std::numbers::pi_v<Float>;
		return rad * factor;
	}
}