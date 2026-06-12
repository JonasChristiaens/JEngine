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
		explicit GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode, int levelIndex, const PlayerCarryOver& carryOver, const PlayerCarryOver& p2CarryOver = {});

		void OnEnter() override;
		void OnExit() override;
		void Update() override;
		void Notify(GameObject& actor, Event event) override;

	private:
		static constexpr int kMaxPlayers{ 2 };

		int m_CurrentLevelIndex{ 0 };
		int m_TotalLevels{ 0 };
		bool m_LevelCompleted{ false };
		PlayerCarryOver m_Carried[kMaxPlayers]{};
		int m_AlivePlayerCount{ 0 };
		bool m_PlayerSurvivedDamage{ false };
		bool m_RespawnAfterDeathAnim{ false };
		GameObject* m_pRespawnPlayer{ nullptr };
		GameObject* m_pPlayers[kMaxPlayers]{ nullptr, nullptr };
		float m_BgmCooldown{ 0.0f };
		bool m_StageClearPlayed{ false };

		void SavePlayerState();
		void HandlePlayerSurvivedDamage(GameObject& player);
	};
}
