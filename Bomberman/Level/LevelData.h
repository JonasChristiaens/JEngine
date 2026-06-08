#pragma once

#include <glm/vec2.hpp>
#include <vector>

namespace dae
{
	enum class PickupType
	{
		None,
		Bomb,
		Flames,
		Skate,
		RemoteControl,
	};

	struct LevelData
	{
		int softBlockCount{ 0 };
		std::vector<glm::ivec2> reservedTiles{};
		int balloomCount{ 0 };
		int onealCount{ 0 };
		int dollCount{ 0 };
		int minvoCount{ 0 };
		PickupType pickupType{ PickupType::None };
	};
}
