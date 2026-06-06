#include "GameSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "EventQueue/EventManager.h"
#include "Input/InputManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Components/HealthComponent.h"
#include "State/EndSceneState.h"
#include "Scenes/GameplaySceneBuilder.h"

namespace dae
{
	GameSceneState::GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode)
		: SceneState(owner)
	{
		SetGameMode(gameMode);
	}

	void GameSceneState::OnEnter()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		m_Owner.SetActiveScene(scene);
		m_Owner.ClearPlayers();

		const auto data = BuildGameplayScene(scene, GetGameMode());
		m_Owner.RegisterPlayer(data.player1);
		m_Owner.RegisterPlayer(data.player2);
	}

	void GameSceneState::OnExit()
	{
		InputManager::GetInstance().ClearAllBindings();
		EventManager::GetInstance().ClearQueue();

		if (auto* scene = m_Owner.GetActiveScene())
		{
			scene->RemoveAll();
		}
		m_Owner.ClearPlayers();
	}

	void GameSceneState::Update()
	{
		if (AreAllPlayersDead())
		{
			m_Owner.GetStateMachine().SetState(std::make_unique<EndSceneState>(m_Owner));
		}
	}

	bool GameSceneState::AreAllPlayersDead() const
	{
		const auto& players = m_Owner.GetPlayers();
		if (players.empty())
			return false;

		for (const auto* player : players)
		{
			if (!player)
				continue;

			auto* health = player->GetComponent<HealthComponent>();
			if (health && health->GetHealth() > 0)
				return false;
		}

		return true;
	}
}
