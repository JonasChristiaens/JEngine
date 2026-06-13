#pragma once
#include <climits>
#include <vector>
#include <glm/vec3.hpp>
#include "Config/EnemyConfig.h"

namespace dae
{
	class GameObject;
	class Scene;

	namespace EnemyFactory
	{
		GameObject* CreateEnemy(Scene& scene, GameObject& parent, int gridColumns, int gridRows, float tileWorldSize, float moveSpeed, const EnemyConfig& config, const std::vector<GameObject*>& chaseTargets = {}, const std::vector<glm::vec3>& reservedWorldPositions = {}, bool useAiMovement = true, int maxColumn = INT_MAX);
	}
}
