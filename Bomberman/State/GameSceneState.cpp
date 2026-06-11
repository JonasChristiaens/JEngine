#include "GameSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/DeathAnimatorComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/DetonatorComponent.h"
#include "Components/SkateComponent.h"
#include "EventQueue/EventManager.h"
#include "Input/InputManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "State/EndSceneState.h"
#include "State/TransitionSceneState.h"
#include "Scenes/GameplaySceneBuilder.h"
#include "Level/LevelDataLoader.h"
#include "Resources/ResourceManager.h"
#include "Audio/ServiceLocator.h"
#include "Core/GameTime.h"

namespace
{
	constexpr dae::EventId kLevelCompletedEventId = dae::make_sdbm_hash("LevelCompleted");
	constexpr dae::EventId kAllEnemiesDeadEventId = dae::make_sdbm_hash("AllEnemiesDead");
}

namespace dae
{
	GameSceneState::GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode)
		: SceneState(owner)
	{
		SetGameMode(gameMode);
	}

	GameSceneState::GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode, int levelIndex, const PlayerCarryOver& carryOver)
		: SceneState(owner)
		, m_CurrentLevelIndex(levelIndex)
		, m_CarriedBombCapacity(carryOver.bombCapacity)
		, m_CarriedBombRange(carryOver.bombRange)
		, m_CarriedDetonator(carryOver.hasDetonator)
		, m_CarriedHasSkate(carryOver.hasSkate)
		, m_CarriedHealth(carryOver.health)
		, m_CarriedScore(carryOver.score)
	{
		SetGameMode(gameMode);
	}

	void GameSceneState::OnEnter()
	{
		EventManager::GetInstance().AddObserver(*this);

		auto& scene = SceneManager::GetInstance().CreateScene();
		m_Owner.SetActiveScene(scene);
		m_Owner.ClearPlayers();

		const auto data = BuildGameplayScene(scene, GetGameMode(), m_CurrentLevelIndex, { m_CarriedBombCapacity, m_CarriedBombRange, m_CarriedDetonator, m_CarriedHasSkate, m_CarriedHealth, m_CarriedScore });
		m_Owner.RegisterPlayer(data.player1);
		m_Owner.RegisterPlayer(data.player2);
		m_AlivePlayerCount = (data.player1 ? 1 : 0) + (data.player2 ? 1 : 0);
		m_TotalLevels = static_cast<int>(LevelDataLoader::Load((ResourceManager::GetInstance().GetDataPath() / "levels.bin").string()).size());

		m_StageClearPlayed = false;

		Event playBgmEvent(make_sdbm_hash("PlayAudioEvent"));
		playBgmEvent.nbArgs = 1;
		playBgmEvent.args[0].p = const_cast<char*>("Main_BGM.flac");
		EventManager::GetInstance().BroadcastImmediate(playBgmEvent, m_Owner.GetOwner());
		m_BgmCooldown = 1.0f;
	}

	void GameSceneState::OnExit()
	{
		ServiceLocator::GetSoundService().StopAll();
		if (EventManager::IsAlive())
			EventManager::GetInstance().RemoveObserver(*this);

		InputManager::GetInstance().ClearAllBindings();
		EventManager::GetInstance().ClearQueue();
		ResetGameplayObservers();

		if (auto* scene = m_Owner.GetActiveScene())
		{
			scene->RemoveAll();
		}
		m_Owner.ClearPlayers();
	}

	void GameSceneState::Update()
	{
		if (m_BgmCooldown > 0.0f)
		{
			m_BgmCooldown -= GameTime::GetInstance().GetDeltaTime();
		}
		else if (!ServiceLocator::GetSoundService().IsPlaying())
		{
			Event playBgmEvent(make_sdbm_hash("PlayAudioEvent"));
			playBgmEvent.nbArgs = 1;
			playBgmEvent.args[0].p = const_cast<char*>("Main_BGM.flac");
			EventManager::GetInstance().BroadcastImmediate(playBgmEvent, m_Owner.GetOwner());
			m_BgmCooldown = 1.0f;
		}

		if (m_LevelCompleted)
		{
			m_LevelCompleted = false;
			SavePlayerState();
			++m_CurrentLevelIndex;

			if (m_CurrentLevelIndex >= m_TotalLevels)
			{
				const auto& players = m_Owner.GetPlayers();
				int score1 = 0;
				int score2 = 0;
				bool hasPlayer2 = false;

				if (!players.empty() && players[0])
				{
					auto* sc = players[0]->GetComponent<ScoreComponent>();
					if (sc)
						score1 = sc->GetScore();
				}
				if (players.size() > 1 && players[1])
				{
					hasPlayer2 = true;
					auto* sc = players[1]->GetComponent<ScoreComponent>();
					if (sc)
						score2 = sc->GetScore();
				}

				m_Owner.GetStateMachine().SetState(
					std::make_unique<TransitionSceneState>(
						m_Owner,
						"GAME COMPLETE",
						std::make_unique<EndSceneState>(m_Owner, score1, score2, hasPlayer2)
					)
				);
				return;
			}

			const PlayerCarryOver carryOver{ m_CarriedBombCapacity, m_CarriedBombRange, m_CarriedDetonator, m_CarriedHasSkate, m_CarriedHealth, m_CarriedScore };
			const int level = m_CurrentLevelIndex;
			const GameMode mode = GetGameMode();

			m_Owner.GetStateMachine().SetState(
				std::make_unique<TransitionSceneState>(
					m_Owner,
					"STAGE " + std::to_string(level + 1),
					std::make_unique<GameSceneState>(m_Owner, mode, level, carryOver)
				)
			);
			return;
		}

		if (m_PlayerSurvivedDamage)
		{
			m_PlayerSurvivedDamage = false;

			const PlayerCarryOver carryOver{ m_CarriedBombCapacity, m_CarriedBombRange, m_CarriedDetonator, m_CarriedHasSkate, m_CarriedHealth, m_CarriedScore };
			const GameMode mode = GetGameMode();

			m_Owner.GetStateMachine().SetState(
				std::make_unique<TransitionSceneState>(
					m_Owner,
					"STAGE " + std::to_string(m_CurrentLevelIndex + 1),
					std::make_unique<GameSceneState>(m_Owner, mode, m_CurrentLevelIndex, carryOver)
				)
			);
			return;
		}

		if (m_AlivePlayerCount <= 0)
		{
			const auto& players = m_Owner.GetPlayers();
			int score1 = 0;
			int score2 = 0;
			bool hasPlayer2 = false;

			if (!players.empty() && players[0])
			{
				auto* sc = players[0]->GetComponent<ScoreComponent>();
				if (sc)
					score1 = sc->GetScore();
			}
			if (players.size() > 1 && players[1])
			{
				hasPlayer2 = true;
				auto* sc = players[1]->GetComponent<ScoreComponent>();
				if (sc)
					score2 = sc->GetScore();
			}

			m_Owner.GetStateMachine().SetState(
				std::make_unique<TransitionSceneState>(
					m_Owner,
					"GAME OVER",
					std::make_unique<EndSceneState>(m_Owner, score1, score2, hasPlayer2)
				)
			);
			return;
		}
	}

	void GameSceneState::Notify(GameObject& actor, Event event)
	{
		if (event.id == kLevelCompletedEventId)
		{
			m_LevelCompleted = true;
		}
		else if (event.id == kAllEnemiesDeadEventId)
		{
			if (!m_StageClearPlayed)
			{
				m_StageClearPlayed = true;

				Event playSfxEvent(make_sdbm_hash("PlayAudioEvent"));
				playSfxEvent.nbArgs = 1;
				playSfxEvent.args[0].p = const_cast<char*>("Stage_Clear.flac");
				EventManager::GetInstance().BroadcastImmediate(playSfxEvent, m_Owner.GetOwner());
			}
		}
		else if (event.id == make_sdbm_hash("EntityDied"))
		{
			if (m_RespawnAfterDeathAnim && m_pRespawnPlayer == &actor)
			{
				m_RespawnAfterDeathAnim = false;
				m_pRespawnPlayer = nullptr;
				m_PlayerSurvivedDamage = true;
				return;
			}

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
		else if (event.id == make_sdbm_hash("TookDamageEvent"))
		{
			HandlePlayerSurvivedDamage(actor);
		}
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

		auto* skate = players[0]->GetComponent<SkateComponent>();
		if (skate)
			m_CarriedHasSkate = skate->HasSkate();

		auto* health = players[0]->GetComponent<HealthComponent>();
		if (health)
			m_CarriedHealth = health->GetHealth();

		auto* score = players[0]->GetComponent<ScoreComponent>();
		if (score)
			m_CarriedScore = score->GetScore();
	}

	void GameSceneState::HandlePlayerSurvivedDamage(GameObject& actor)
	{
		const auto& players = m_Owner.GetPlayers();
		bool isRegisteredPlayer = false;
		for (const auto* player : players)
		{
			if (player == &actor)
			{
				isRegisteredPlayer = true;
				break;
			}
		}

		if (!isRegisteredPlayer)
			return;

		auto* health = actor.GetComponent<HealthComponent>();
		if (!health || health->IsDead())
			return;

		SavePlayerState();

		auto* deathAnim = actor.GetComponent<DeathAnimatorComponent>();
		if (deathAnim)
		{
			deathAnim->Play();
			m_RespawnAfterDeathAnim = true;
			m_pRespawnPlayer = &actor;
		}
	}
}
