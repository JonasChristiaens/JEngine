#pragma once
#include "SceneState.h"
#include "EventQueue/IObserver.h"
#include "Scenes/GameplaySceneBuilder.h"

namespace dae
{
	class GameSceneState final : public SceneState, public IObserver
	{
	public:
		explicit GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode = GameMode::Solo);
		explicit GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode, int levelIndex, const PlayerCarryOver& carryOver);

		void OnEnter() override;
		void OnExit() override;
		void Update() override;
		void Notify(GameObject& actor, Event event) override;

	private:
		int m_CurrentLevelIndex{ 0 };
		bool m_LevelCompleted{ false };
		int m_CarriedBombCapacity{ 1 };
		int m_CarriedBombRange{ 1 };
		bool m_CarriedDetonator{ false };
		int m_AlivePlayerCount{ 0 };

		void SavePlayerState();
		void ReloadScene();
	};
}
