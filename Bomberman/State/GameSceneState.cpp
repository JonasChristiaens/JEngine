#include "GameSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Components/HealthComponent.h"
#include "State/EndSceneState.h"
#include "Scenes/GameplaySceneBuilder.h"

namespace dae
{
	GameSceneState::GameSceneState(SceneStateMachineComponent& owner)
		: SceneState(owner)
	{
	}

	void GameSceneState::OnEnter()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		m_owner.SetActiveScene(scene);
		m_owner.ClearPlayers();

		const auto data = BuildGameplayScene(scene);
		m_owner.RegisterPlayer(data.player1);
		m_owner.RegisterPlayer(data.player2);
	}

	void GameSceneState::OnExit()
	{
		if (auto* scene = m_owner.GetActiveScene())
		{
			scene->RemoveAll();
		}
		m_owner.ClearPlayers();
	}

	void GameSceneState::Update()
	{
		if (AreAllPlayersDead())
		{
			m_owner.GetStateMachine().SetState(std::make_unique<EndSceneState>(m_owner));
		}
	}

	bool GameSceneState::AreAllPlayersDead() const
	{
		const auto& players = m_owner.GetPlayers();
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
