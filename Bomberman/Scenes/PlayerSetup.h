#pragma once
#include "GameplaySceneBuilder.h"
#include <glm/vec3.hpp>

namespace dae
{
	class Scene;
	class GameObject;

	GameObject* CreatePlayer(Scene& scene, const glm::vec3& startPos, const PlayerCarryOver& carryOver = {});
	void BindKeyboardMovement(GameObject& player);
	void BindControllerMovement(GameObject& player, unsigned int controllerIndex);
	void BindPlayerControls(GameObject& player, bool keyboard, unsigned int controllerIndex);
	unsigned int GetSecondaryControllerIndex();
}
