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

	GameSceneState::GameSceneState(SceneStateMachineComponent& owner, GameMode gameMode, int levelIndex, const PlayerCarryOver& carryOver, const PlayerCarryOver& p2CarryOver)
		: SceneState(owner)
		, m_CurrentLevelIndex(levelIndex)
	{
		m_Carried[0] = carryOver;
		m_Carried[1] = p2CarryOver;
		SetGameMode(gameMode);
	}

	void GameSceneState::OnEnter()
	{
		EventManager::GetInstance().AddObserver(*this);

		auto& scene = SceneManager::GetInstance().CreateScene();
		m_Owner.SetActiveScene(scene);
		m_Owner.ClearPlayers();

		const PlayerCarryOver& carryOver = m_Carried[0];
		const PlayerCarryOver& p2CarryOver = m_Carried[1];
		const auto data = BuildGameplayScene(scene, GetGameMode(), m_CurrentLevelIndex, carryOver, p2CarryOver);
		m_pPlayers[0] = data.player1;
		m_pPlayers[1] = data.player2;

		const int playerCount = (GetGameMode() == GameMode::Versus) ? 1 : 2;
		m_AlivePlayerCount = 0;
		for (int i = 0; i < playerCount; ++i)
		{
			if (m_pPlayers[i])
			{
				m_Owner.RegisterPlayer(m_pPlayers[i]);
				++m_AlivePlayerCount;
			}
		}
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
		m_pPlayers[0] = nullptr;
		m_pPlayers[1] = nullptr;
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
				const int score1 = m_Carried[0].score;
				const int score2 = m_Carried[1].score;
				const bool hasPlayer2 = (GetGameMode() == GameMode::Coop);

				m_Owner.GetStateMachine().SetState(
					std::make_unique<TransitionSceneState>(
						m_Owner,
						"GAME COMPLETE",
						std::make_unique<EndSceneState>(m_Owner, score1, score2, hasPlayer2)
					)
				);
				return;
			}

			const PlayerCarryOver& carryOver = m_Carried[0];
			const PlayerCarryOver& p2CarryOver = m_Carried[1];
			const int level = m_CurrentLevelIndex;
			const GameMode mode = GetGameMode();

			m_Owner.GetStateMachine().SetState(
				std::make_unique<TransitionSceneState>(
					m_Owner,
					"STAGE " + std::to_string(level + 1),
					std::make_unique<GameSceneState>(m_Owner, mode, level, carryOver, p2CarryOver)
				)
			);
			return;
		}

		if (m_PlayerSurvivedDamage)
		{
			m_PlayerSurvivedDamage = false;

			const PlayerCarryOver& carryOver = m_Carried[0];
			const PlayerCarryOver& p2CarryOver = m_Carried[1];
			const GameMode mode = GetGameMode();

			m_Owner.GetStateMachine().SetState(
				std::make_unique<TransitionSceneState>(
					m_Owner,
					"STAGE " + std::to_string(m_CurrentLevelIndex + 1),
					std::make_unique<GameSceneState>(m_Owner, mode, m_CurrentLevelIndex, carryOver, p2CarryOver)
				)
			);
			return;
		}

		if (m_AlivePlayerCount <= 0)
		{
			const int score1 = m_Carried[0].score;
			const int score2 = m_Carried[1].score;
			const bool hasPlayer2 = (GetGameMode() == GameMode::Coop);

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

			for (int i = 0; i < kMaxPlayers; ++i)
			{
				if (&actor == m_pPlayers[i])
				{
					auto* sc = actor.GetComponent<ScoreComponent>();
					if (sc)
						m_Carried[i].score = sc->GetScore();
					m_Carried[i].health = 0;
					m_pPlayers[i] = nullptr;
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
		for (int i = 0; i < kMaxPlayers; ++i)
		{
			if (!m_pPlayers[i])
				continue;

			auto* health = m_pPlayers[i]->GetComponent<HealthComponent>();
			if (!health || health->IsDead())
				continue;

			auto* capacity = m_pPlayers[i]->GetComponent<BombCapacityComponent>();
			if (capacity)
				m_Carried[i].bombCapacity = capacity->GetMaxBombs();

			auto* range = m_pPlayers[i]->GetComponent<BombRangeComponent>();
			if (range)
				m_Carried[i].bombRange = range->GetRange();

			auto* detonator = m_pPlayers[i]->GetComponent<DetonatorComponent>();
			if (detonator)
				m_Carried[i].hasDetonator = detonator->HasDetonator();

			auto* skate = m_pPlayers[i]->GetComponent<SkateComponent>();
			if (skate)
				m_Carried[i].hasSkate = skate->HasSkate();

			m_Carried[i].health = health->GetHealth();

			auto* score = m_pPlayers[i]->GetComponent<ScoreComponent>();
			if (score)
				m_Carried[i].score = score->GetScore();
		}
	}

	void GameSceneState::HandlePlayerSurvivedDamage(GameObject& actor)
	{
		bool isRegistered = false;
		for (int i = 0; i < kMaxPlayers; ++i)
		{
			if (&actor == m_pPlayers[i])
			{
				isRegistered = true;
				break;
			}
		}
		if (!isRegistered)
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
