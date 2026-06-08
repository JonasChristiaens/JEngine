#pragma once
#include "Components/EnemyMovementComponent.h"
#include <SDL3/SDL_rect.h>
#include <vector>

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
		std::vector<SDL_FRect> deathFrames{};
		float deathFps{ 10.0f };
	};

	inline std::vector<SDL_FRect> BuildHorizontalFrames(float startX, float startY, int frameCount, float frameWidth, float frameHeight)
	{
		std::vector<SDL_FRect> frames;
		frames.reserve(frameCount);
		for (int i = 0; i < frameCount; ++i)
		{
			frames.push_back({ startX + i * frameWidth, startY, frameWidth, frameHeight });
		}
		return frames;
	}
}
