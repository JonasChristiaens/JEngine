#pragma once
#include "GameplaySceneBuilder.h"
#include <glm/vec3.hpp>
#include <utility>

namespace dae
{
	class Scene;
	class GameObject;
	struct EnemyConfig;

	struct SpawnParams
	{
		Scene& scene;
		GameObject& worldRoot;
		float tileWorldSize;
		const PlayerCarryOver& carryOver;
		const PlayerCarryOver& p2CarryOver;
		const glm::vec3& player1Pos;
		const glm::vec3& player2Pos;
		int balloomCount;
		int onealCount;
		int dollCount;
		int minvoCount;
	};

	GameObject* SpawnEnemy(Scene& scene, GameObject& parent, float tileWorldSize, const EnemyConfig& config, float moveSpeed,
		GameObject* pChaseTarget, const glm::vec3& reservedWorldPosition, bool useAiMovement);

	std::pair<GameObject*, GameObject*> SetupSoloMode(const SpawnParams& p);
	std::pair<GameObject*, GameObject*> SetupCoopMode(const SpawnParams& p);
	std::pair<GameObject*, GameObject*> SetupVersusMode(const SpawnParams& p);
}
