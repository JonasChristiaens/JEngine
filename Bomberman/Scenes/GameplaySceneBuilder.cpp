#include "GameplaySceneBuilder.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/PlayerAnimatorComponent.h"
#include "Components/CameraComponent.h"
#include "Components/HudComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/PlayfieldComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/DetonatorComponent.h"
#include "Components/DeathAnimatorComponent.h"
#include "EnemyConfig.h"
#include "Factories/EnemyFactory.h"
#include "Commands/MoveCommand.h"
#include "Commands/SpawnBombCommand.h"
#include "Commands/DetonateBombsCommand.h"
#include "Observers/BombEventObserver.h"
#include "Observers/EntityDeathObserver.h"
#include "Audio/AudioEventObserver.h"
#include "Input/InputManager.h"
#include "Input/ControllerButtons.h"
#include "Input/KeyCodes.h"
#include "Rendering/Renderer.h"
#include "Level/LevelData.h"
#include "Level/LevelDataLoader.h"
#include "Resources/ResourceManager.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>

namespace
{
	constexpr float kPlayfieldWidth{ 496.0f };
	constexpr float kPlayfieldHeight{ 208.0f };
	constexpr float kHudHeightRatio{ 0.12f };
	constexpr float kBalloomSpeed{ 90.0f };
	constexpr float kOnealSpeed{ 120.0f };
	constexpr float kDollSpeed{ 120.0f };
	constexpr float kMinvoSpeed{ 180.0f };
	constexpr float kPlayerMoveSpeed{ 120.0f };
	constexpr float kPlayerSpriteSize{ 16.0f };
	constexpr float kPlayerSpriteScale{ 3.0f };
	constexpr float kPlayerCollisionSize{ (kPlayerSpriteSize * kPlayerSpriteScale) - 20.0f };
	constexpr float kPlayerPivotY{ 1.0f - (kPlayerCollisionSize * 0.5f) / (kPlayerSpriteSize * kPlayerSpriteScale) };

	const dae::EnemyConfig kBalloomConfig = []()
	{
		dae::EnemyConfig config{
			0.0f, 241.0f, 16.0f, 16.0f,
			3.0f, 0.8f,
			2.0f, 4.0f,
			dae::EnemyChaseAxis::None,
			100
		};
		config.deathFrames = dae::BuildHorizontalFrames(96.0f, 240.0f, 1, 16.0f, 16.0f);
		auto orangeDeath = dae::BuildHorizontalFrames(112.0f, 240.0f, 4, 16.0f, 16.0f);
		config.deathFrames.insert(config.deathFrames.end(), orangeDeath.begin(), orangeDeath.end());
		config.deathFps = 10.0f;
		return config;
	}();

	const dae::EnemyConfig kOnealConfig = []()
	{
		dae::EnemyConfig config{
			0.0f, 257.0f, 16.0f, 16.0f,
			3.0f, 0.8f,
			0.8f, 3.2f,
			dae::EnemyChaseAxis::Y,
			200
		};
		config.deathFrames = dae::BuildHorizontalFrames(96.0f, 256.0f, 1, 16.0f, 16.0f);
		auto blueDeath = dae::BuildHorizontalFrames(112.0f, 288.0f, 4, 16.0f, 16.0f);
		config.deathFrames.insert(config.deathFrames.end(), blueDeath.begin(), blueDeath.end());
		config.deathFps = 10.0f;
		return config;
	}();

	const dae::EnemyConfig kDollConfig = []()
	{
		dae::EnemyConfig config{
			0.0f, 273.0f, 16.0f, 16.0f,
			3.0f, 0.8f,
			0.8f, 3.2f,
			dae::EnemyChaseAxis::X,
			400
		};
		config.deathFrames = dae::BuildHorizontalFrames(96.0f, 272.0f, 1, 16.0f, 16.0f);
		auto deathRest = dae::BuildHorizontalFrames(112.0f, 272.0f, 4, 16.0f, 16.0f);
		config.deathFrames.insert(config.deathFrames.end(), deathRest.begin(), deathRest.end());
		config.deathFps = 10.0f;
		return config;
	}();

	const dae::EnemyConfig kMinvoConfig = []()
	{
		dae::EnemyConfig config{
			0.0f, 289.0f, 16.0f, 16.0f,
			3.0f, 0.8f,
			0.5f, 2.0f,
			dae::EnemyChaseAxis::Both,
			800
		};
		config.deathFrames = dae::BuildHorizontalFrames(96.0f, 288.0f, 1, 16.0f, 16.0f);
		auto orangeDeath2 = dae::BuildHorizontalFrames(112.0f, 240.0f, 4, 16.0f, 16.0f);
		config.deathFrames.insert(config.deathFrames.end(), orangeDeath2.begin(), orangeDeath2.end());
		config.deathFps = 10.0f;
		return config;
	}();

	dae::PlayfieldComponent::PlayfieldConfig ToPlayfieldConfig(const dae::LevelData& levelData)
	{
		dae::PlayfieldComponent::PlayfieldConfig config{};
		config.softBlockCount = levelData.softBlockCount;
		config.reservedTiles = levelData.reservedTiles;
		config.pickupType = levelData.pickupType;
		return config;
	}

	std::vector<dae::LevelData> LoadLevels()
	{
		const auto jsonText = dae::ResourceManager::GetInstance().LoadTextFile("levels.json");
		return dae::LevelDataLoader::LoadFromText(jsonText);
	}

	dae::GameObject* CreatePlayer(dae::Scene& scene, const glm::vec3& startPos, const dae::PlayerCarryOver& carryOver = {})
	{
		auto go = std::make_unique<dae::GameObject>();
		auto* transform = go->AddComponent<dae::TransformComponent>();
		transform->SetLocalPosition(startPos);
		auto* render = go->AddComponent<dae::RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetSpriteSheet(16, 16, 6, 2);
		render->SetSprite(4, 0);
		render->SetScale(kPlayerSpriteScale);
		render->SetPivot({ 0.5f, kPlayerPivotY });
		render->SetRenderLayer(4);
		go->AddComponent<dae::SpriteAnimatorComponent>();
		go->AddComponent<dae::PlayerAnimatorComponent>();
		go->AddComponent<dae::HealthComponent>(4);
		go->AddComponent<dae::ScoreComponent>(0);
		go->AddComponent<dae::BombRangeComponent>(4);
		go->AddComponent<dae::BombCapacityComponent>(carryOver.bombCapacity);
		auto* detonator = go->AddComponent<dae::DetonatorComponent>();
		if (carryOver.hasDetonator)
			detonator->SetHasDetonator(true);
		go->AddComponent<dae::DeathAnimatorComponent>("BombermanSprites_General.png", dae::BuildHorizontalFrames(0.0f, 32.0f, 7, 16.0f, 16.0f), 10.0f, kPlayerSpriteScale, true);
		auto* collider = go->AddComponent<dae::CollisionComponent>(kPlayerCollisionSize, kPlayerCollisionSize);
		collider->SetOffset({ -kPlayerCollisionSize * 0.5f, -kPlayerCollisionSize * 0.5f });

		auto* rangeComp = go->GetComponent<dae::BombRangeComponent>();
		for (int i = 1; i < carryOver.bombRange; ++i)
			rangeComp->IncreaseRange();

		auto* player = go.get();
		scene.Add(std::move(go));
		return player;
	}

	void BindKeyboardMovement(dae::GameObject& player)
	{
		auto& input = dae::InputManager::GetInstance();
		auto bindMove = [&](dae::KeyCode key, const glm::vec3& dir)
		{
			auto cmd = std::make_unique<dae::MoveCommand>(dir, kPlayerMoveSpeed);
			cmd->SetGameActor(&player);
			input.BindKeyboardInput(key, dae::KeyState::Pressed, std::move(cmd));
		};

		bindMove(dae::KeyCode::W, { 0, -1, 0 });
		bindMove(dae::KeyCode::S, { 0, 1, 0 });
		bindMove(dae::KeyCode::A, { -1, 0, 0 });
		bindMove(dae::KeyCode::D, { 1, 0, 0 });

		auto bomb = std::make_unique<dae::SpawnBombCommand>();
		bomb->SetGameActor(&player);
		input.BindKeyboardInput(dae::KeyCode::R, dae::KeyState::Down, std::move(bomb));

		auto detonate = std::make_unique<dae::DetonateBombsCommand>();
		detonate->SetGameActor(&player);
		input.BindKeyboardInput(dae::KeyCode::B, dae::KeyState::Down, std::move(detonate));
	}

	void BindControllerMovement(dae::GameObject& player, unsigned int controllerIndex)
	{
		auto& input = dae::InputManager::GetInstance();
		if (!input.HasController(controllerIndex))
			return;

		auto bindMove = [&](unsigned int button, const glm::vec3& dir)
		{
			auto cmd = std::make_unique<dae::MoveCommand>(dir, kPlayerMoveSpeed * 2.0f);
			cmd->SetGameActor(&player);
			input.BindControllerInput(controllerIndex, button, dae::KeyState::Pressed, std::move(cmd));
		};

		bindMove(ControllerButton::kDpadUp, { 0, -1, 0 });
		bindMove(ControllerButton::kDpadDown, { 0, 1, 0 });
		bindMove(ControllerButton::kDpadLeft, { -1, 0, 0 });
		bindMove(ControllerButton::kDpadRight, { 1, 0, 0 });

		auto bomb = std::make_unique<dae::SpawnBombCommand>();
		bomb->SetGameActor(&player);
		input.BindControllerInput(controllerIndex, ControllerButton::kY, dae::KeyState::Down, std::move(bomb));

		auto detonate = std::make_unique<dae::DetonateBombsCommand>();
		detonate->SetGameActor(&player);
		input.BindControllerInput(controllerIndex, ControllerButton::kB, dae::KeyState::Down, std::move(detonate));
	}

	void BindPlayerControls(dae::GameObject& player, bool keyboard, unsigned int controllerIndex)
	{
		if (keyboard)
			BindKeyboardMovement(player);
		BindControllerMovement(player, controllerIndex);
	}

	unsigned int GetSecondaryControllerIndex()
	{
		auto& input = dae::InputManager::GetInstance();
		return input.HasController(1) ? 1u : 0u;
	}

	dae::GameObject* SpawnEnemy(dae::Scene& scene, dae::GameObject& parent, float tileWorldSize, const dae::EnemyConfig& config, float moveSpeed,
		dae::GameObject* pChaseTarget, const glm::vec3& reservedWorldPosition, bool useAiMovement)
	{
		const int gridColumns = static_cast<int>(kPlayfieldWidth / 16.0f);
		const int gridRows = static_cast<int>(kPlayfieldHeight / 16.0f);
		auto* enemy = dae::EnemyFactory::CreateEnemy(scene, parent, gridColumns, gridRows, tileWorldSize, moveSpeed, config, pChaseTarget, { reservedWorldPosition }, useAiMovement);
		if (enemy)
		{
			if (auto* playfield = parent.GetComponent<dae::PlayfieldComponent>())
				playfield->RegisterEnemySpawned();
		}
		return enemy;
	}

	std::unique_ptr<dae::BombEventObserver> g_BombObserver{};
	std::unique_ptr<dae::EntityDeathObserver> g_EntityDeathObserver{};
	std::unique_ptr<dae::AudioEventObserver> g_AudioObserver{};
}

namespace dae
{
	GameplaySceneData BuildGameplayScene(Scene& scene, GameMode gameMode, int levelIndex, const PlayerCarryOver& carryOver)
	{		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		const float windowWidth = static_cast<float>(windowSize.x);
		const float windowHeight = static_cast<float>(windowSize.y);
		const float hudHeight = windowHeight * kHudHeightRatio;
		const float playfieldAreaHeight = windowHeight - hudHeight;
		const float playfieldScale = playfieldAreaHeight / kPlayfieldHeight;
		const float playfieldScaledWidth = kPlayfieldWidth * playfieldScale;
		const float tileWorldSize = 16.0f * playfieldScale;

		auto hudObj = std::make_unique<GameObject>();
		hudObj->AddComponent<TransformComponent>();
		GameObject* hudRoot = hudObj.get();
		scene.Add(std::move(hudObj));

		auto cameraRoot = std::make_unique<GameObject>();
		cameraRoot->AddComponent<TransformComponent>();
		cameraRoot->GetComponent<TransformComponent>()->SetLocalPosition(0.0f, hudHeight, 0.0f);

		auto worldRoot = std::make_unique<GameObject>();
		worldRoot->AddComponent<TransformComponent>();
		worldRoot->GetComponent<TransformComponent>()->SetLocalPosition(0.0f, hudHeight, 0.0f);
		auto* worldRootPtr = worldRoot.get();
		scene.Add(std::move(worldRoot));

		auto playfield = std::make_unique<GameObject>();
		playfield->AddComponent<TransformComponent>();
		auto* render = playfield->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_Playfield.png");
		render->SetSourceRectangle(0, 0, kPlayfieldWidth, kPlayfieldHeight);
		render->SetScale(playfieldScale);
		render->SetRenderLayer(0);
		playfield->SetParent(worldRootPtr, false);
		scene.Add(std::move(playfield));

		const auto levels = LoadLevels();
		const int idx = levelIndex < static_cast<int>(levels.size()) ? levelIndex : 0;
		worldRootPtr->AddComponent<PlayfieldComponent>(scene, kPlayfieldWidth, kPlayfieldHeight, playfieldScale, ToPlayfieldConfig(levels.at(idx)));

		const glm::vec3 player1Pos{ tileWorldSize * 1.5f, tileWorldSize * 2.5f, 0.0f };
		const glm::vec3 player2Pos{ tileWorldSize * 3.5f, tileWorldSize * 2.5f, 0.0f };
		const int balloomCount = std::max(0, levels.at(idx).balloomCount);
		const int onealCount = std::max(0, levels.at(idx).onealCount);
		const int dollCount = std::max(0, levels.at(idx).dollCount);
		const int minvoCount = std::max(0, levels.at(idx).minvoCount);

		GameObject* player1 = nullptr;
		GameObject* player2 = nullptr;

		switch (gameMode)
		{
		case GameMode::Solo:
			player1 = CreatePlayer(scene, player1Pos, carryOver);
			player1->SetParent(worldRootPtr, false);
			BindPlayerControls(*player1, true, 0);
			for (int i = 0; i < balloomCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kBalloomConfig, kBalloomSpeed, nullptr, player1Pos, true);
			}
			for (int i = 0; i < onealCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kOnealConfig, kOnealSpeed, player1, player1Pos, true);
			}
			for (int i = 0; i < dollCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kDollConfig, kDollSpeed, player1, player1Pos, true);
			}
			for (int i = 0; i < minvoCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kMinvoConfig, kMinvoSpeed, player1, player1Pos, true);
			}
			break;
		case GameMode::Coop:
			player1 = CreatePlayer(scene, player1Pos, carryOver);
			player2 = CreatePlayer(scene, player2Pos);
			player1->SetParent(worldRootPtr, false);
			player2->SetParent(worldRootPtr, false);
			BindPlayerControls(*player1, true, 0);
			BindPlayerControls(*player2, false, GetSecondaryControllerIndex());
			for (int i = 0; i < balloomCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kBalloomConfig, kBalloomSpeed, nullptr, (i == 0) ? player1Pos : player2Pos, true);
			}
			for (int i = 0; i < onealCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kOnealConfig, kOnealSpeed, (i == 0) ? player1 : player2, (i == 0) ? player1Pos : player2Pos, true);
			}
			for (int i = 0; i < dollCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kDollConfig, kDollSpeed, (i == 0) ? player1 : player2, (i == 0) ? player1Pos : player2Pos, true);
			}
			for (int i = 0; i < minvoCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kMinvoConfig, kMinvoSpeed, (i == 0) ? player1 : player2, (i == 0) ? player1Pos : player2Pos, true);
			}
			break;
		case GameMode::Versus:
			player1 = CreatePlayer(scene, player1Pos, carryOver);
			player1->SetParent(worldRootPtr, false);
			player2 = SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kBalloomConfig, kBalloomSpeed, nullptr, player1Pos, false);
			BindPlayerControls(*player1, true, 0);
			BindControllerMovement(*player2, GetSecondaryControllerIndex());
			for (int i = 0; i < std::max(0, balloomCount - 1); ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kBalloomConfig, kBalloomSpeed, nullptr, player2Pos, true);
			}
			for (int i = 0; i < onealCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kOnealConfig, kOnealSpeed, player1, player2Pos, true);
			}
			for (int i = 0; i < dollCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kDollConfig, kDollSpeed, player1, player2Pos, true);
			}
			for (int i = 0; i < minvoCount; ++i)
			{
				SpawnEnemy(scene, *worldRootPtr, tileWorldSize, kMinvoConfig, kMinvoSpeed, player1, player2Pos, true);
			}
			break;
		}

		cameraRoot->AddComponent<CameraComponent>(player1, windowWidth, playfieldScaledWidth);
		scene.Add(std::move(cameraRoot));

		if (!g_BombObserver)
			g_BombObserver = std::make_unique<BombEventObserver>(scene, tileWorldSize);
		if (!g_EntityDeathObserver)
			g_EntityDeathObserver = std::make_unique<EntityDeathObserver>(scene);
		if (!g_AudioObserver)
			g_AudioObserver = std::make_unique<AudioEventObserver>();

		std::vector<GameObject*> hudPlayers{};
		if (player1) hudPlayers.push_back(player1);
		if (player2) hudPlayers.push_back(player2);
		hudRoot->AddComponent<HudComponent>(windowWidth, hudHeight, hudPlayers);

		return { player1, player2, tileWorldSize };
	}

	void ResetGameplayObservers()
	{
		g_BombObserver.reset();
		g_EntityDeathObserver.reset();
		g_AudioObserver.reset();
	}
}
