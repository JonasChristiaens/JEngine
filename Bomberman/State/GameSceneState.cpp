#include "GameSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Components/HealthComponent.h"
#include "Components/DeathAnimatorComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/DetonatorComponent.h"
#include "EventQueue/EventManager.h"
#include "Input/InputManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "State/EndSceneState.h"
#include "Scenes/GameplaySceneBuilder.h"

namespace
{
	constexpr dae::EventId kLevelCompletedEventId = dae::make_sdbm_hash("LevelCompleted");
}

namespace dae
{
	GameSceneState::GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode)
		: SceneState(owner)
	{
		SetGameMode(gameMode);
	}

	void GameSceneState::OnEnter()
	{
		EventManager::GetInstance().AddObserver(*this);

		auto& scene = SceneManager::GetInstance().CreateScene();
		m_Owner.SetActiveScene(scene);
		m_Owner.ClearPlayers();

		const auto data = BuildGameplayScene(scene, GetGameMode(), m_CurrentLevelIndex, { m_CarriedBombCapacity, m_CarriedBombRange, m_CarriedDetonator });
		m_Owner.RegisterPlayer(data.player1);
		m_Owner.RegisterPlayer(data.player2);
		m_AlivePlayerCount = (data.player1 ? 1 : 0) + (data.player2 ? 1 : 0);
	}

	void GameSceneState::OnExit()
	{
		if (EventManager::IsAlive())
			EventManager::GetInstance().RemoveObserver(*this);

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
		if (m_LevelCompleted)
		{
			m_LevelCompleted = false;
			SavePlayerState();
			++m_CurrentLevelIndex;
			ReloadScene();
			return;
		}

		if (m_AlivePlayerCount <= 0)
		{
			m_Owner.GetStateMachine().SetState(std::make_unique<EndSceneState>(m_Owner));
		}
	}

	void GameSceneState::Notify(GameObject& actor, Event event)
	{
		if (event.id == kLevelCompletedEventId)
		{
			m_LevelCompleted = true;
		}
		else if (event.id == make_sdbm_hash("EntityDied"))
		{
			const auto& players = m_Owner.GetPlayers();
			for (const auto* player : players)
			{
				if (player == &actor)
				{
					--m_AlivePlayerCount;
					break;
				}
			}
		}
	}

	void GameSceneState::ReloadScene()
	{
		OnExit();
		ResetGameplayObservers();
		OnEnter();
	}

	void GameSceneState::SavePlayerState()
	{
		const auto& players = m_Owner.GetPlayers();
		if (players.empty() || !players[0])
			return;

		auto* capacity = players[0]->GetComponent<BombCapacityComponent>();
		if (capacity)
			m_CarriedBombCapacity = capacity->GetMaxBombs();

		auto* range = players[0]->GetComponent<BombRangeComponent>();
		if (range)
			m_CarriedBombRange = range->GetRange();

		auto* detonator = players[0]->GetComponent<DetonatorComponent>();
		if (detonator)
			m_CarriedDetonator = detonator->HasDetonator();
	}
}
