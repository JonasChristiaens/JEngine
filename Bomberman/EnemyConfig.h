#pragma once
#include "Components/EnemyMovementComponent.h"

namespace dae
{
	struct EnemyConfig
	{
		float spriteX{ 0.0f };
		float spriteY{ 241.0f };
		float spriteWidth{ 16.0f };
		float spriteHeight{ 16.0f };
		float scale{ 3.0f };
		float colliderScale{ 0.8f };
		float minDirectionTime{ 2.0f };
		float maxDirectionTime{ 4.0f };
		EnemyChaseAxis chaseAxis{ EnemyChaseAxis::None };
		int points{ 100 };
	};
}
