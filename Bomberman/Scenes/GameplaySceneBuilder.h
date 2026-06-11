#pragma once
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Config/GameMode.h"

namespace dae
{
	struct GameplaySceneData
	{
		GameObject* player1{};
		GameObject* player2{};
		float tileWorldSize{};
	};

	struct PlayerCarryOver
	{
		int bombCapacity{ 1 };
		int bombRange{ 1 };
		bool hasDetonator{ false };
		bool hasSkate{ false };
		int health{ 4 };
		int score{ 0 };
	};

	GameplaySceneData BuildGameplayScene(Scene& scene, GameMode gameMode, int levelIndex = 0, const PlayerCarryOver& carryOver = {});
	void ResetGameplayObservers();
}
