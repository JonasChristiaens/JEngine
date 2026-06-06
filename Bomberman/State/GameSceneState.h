#pragma once
#include "SceneState.h"

namespace dae
{
	class GameSceneState final : public SceneState
	{
	public:
		explicit GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode = GameMode::Solo);

		void OnEnter() override;
		void OnExit() override;
		void Update() override;

	private:
		bool AreAllPlayersDead() const;
	};
}
