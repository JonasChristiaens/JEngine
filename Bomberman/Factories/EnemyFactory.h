#pragma once
#include <vector>
#include <glm/vec3.hpp>
#include "Config/EnemyConfig.h"

namespace dae
{
	class GameObject;
	class Scene;

	namespace EnemyFactory
	{
		GameObject* CreateEnemy(Scene& scene, GameObject& parent, int gridColumns, int gridRows, float tileWorldSize, float moveSpeed, const EnemyConfig& config, GameObject* pChaseTarget = nullptr, const std::vector<glm::vec3>& reservedWorldPositions = {}, bool useAiMovement = true);
	}
}
