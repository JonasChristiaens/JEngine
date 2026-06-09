#pragma once
#include "SceneState.h"
#include "EventQueue/IObserver.h"

namespace dae
{
	class GameSceneState final : public SceneState, public IObserver
	{
	public:
		explicit GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode = GameMode::Solo);

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

		bool AreAllPlayersDead() const;
		void SavePlayerState();
		void ReloadScene();
	};
}
