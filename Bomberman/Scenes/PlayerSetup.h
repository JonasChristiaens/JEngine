#pragma once
#include "GameplaySceneBuilder.h"
#include <glm/vec3.hpp>
#include <vector>

namespace dae
{
	class Scene;
	class GameObject;

	GameObject* CreatePlayer(Scene& scene, const glm::vec3& startPos, const PlayerCarryOver& carryOver = {});
	void BindKeyboardMovement(GameObject& player);
	void BindControllerMovement(GameObject& player, unsigned int controllerIndex);
	void BindPlayerControls(GameObject& player, bool keyboard, unsigned int controllerIndex);
	void AssignPlayerInputs(const std::vector<GameObject*>& players);
	void BindEnemyMovementControls(GameObject& enemy, unsigned int controllerIndex);
}
