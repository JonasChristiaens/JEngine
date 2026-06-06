#pragma once
#include "Scene/Scene.h"
#include "Scene/GameObject.h"

namespace dae
{
	struct GameplaySceneData
	{
		GameObject* player1{};
		GameObject* player2{};
		float tileWorldSize{};
	};

	GameplaySceneData BuildGameplayScene(Scene& scene);
}
