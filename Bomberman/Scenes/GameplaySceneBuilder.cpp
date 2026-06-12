#include "GameplaySceneBuilder.h"
#include "PlayerSetup.h"
#include "ModeSetup.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/CameraComponent.h"
#include "Components/HudComponent.h"
#include "Components/PlayfieldComponent.h"
#include "Components/CollisionGrid.h"
#include "Observers/BombEventObserver.h"
#include "Observers/EntityDeathObserver.h"
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

	dae::PlayfieldComponent::PlayfieldConfig ToPlayfieldConfig(const dae::LevelData& levelData)
	{
		dae::PlayfieldComponent::PlayfieldConfig config{};
		config.softBlockCount = levelData.softBlockCount;
		config.reservedTiles = levelData.reservedTiles;
		config.pickupType = levelData.pickupType;

		config.reservedTiles.insert(config.reservedTiles.end(), {
			{3, 2}, {4, 1}, {4, 2}, {5, 1}, {5, 2}, {5, 3}, {3, 3}, {4, 3}
		});
		return config;
	}

	std::vector<dae::LevelData> LoadLevels()
	{
		const auto path = (dae::ResourceManager::GetInstance().GetDataPath() / "levels.bin").string();
		return dae::LevelDataLoader::Load(path);
	}

	std::unique_ptr<dae::BombEventObserver> g_BombObserver{};
	std::unique_ptr<dae::EntityDeathObserver> g_EntityDeathObserver{};
}

namespace dae
{
	GameplaySceneData BuildGameplayScene(Scene& scene, GameMode gameMode, int levelIndex, const PlayerCarryOver& carryOver, const PlayerCarryOver& p2CarryOver)
	{
		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		const float windowWidth = static_cast<float>(windowSize.x);
		const float windowHeight = static_cast<float>(windowSize.y);
		const float hudHeight = windowHeight * kHudHeightRatio;
		const float playfieldAreaHeight = windowHeight - hudHeight;
		const float playfieldScale = playfieldAreaHeight / kPlayfieldHeight;
		const float playfieldScaledWidth = kPlayfieldWidth * playfieldScale;
		const float tileWorldSize = 16.0f * playfieldScale;

		CollisionGrid::Initialize(0.0f, hudHeight, tileWorldSize, static_cast<int>(kPlayfieldWidth / 16.0f), static_cast<int>(kPlayfieldHeight / 16.0f));

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
		worldRootPtr->AddComponent<PlayfieldComponent>(scene, kPlayfieldWidth, kPlayfieldHeight, playfieldScale, ToPlayfieldConfig(levels.at(static_cast<size_t>(idx))));

		const glm::vec3 player1Pos{ tileWorldSize * 1.5f, tileWorldSize * 2.5f, 0.0f };
		const glm::vec3 player2Pos{ tileWorldSize * 3.5f, tileWorldSize * 2.5f, 0.0f };
		const size_t levelIdx = static_cast<size_t>(idx);

		const SpawnParams spawnParams{
			scene, *worldRootPtr, tileWorldSize, carryOver, p2CarryOver,
			player1Pos, player2Pos,
			std::max(0, levels.at(levelIdx).balloomCount),
			std::max(0, levels.at(levelIdx).onealCount),
			std::max(0, levels.at(levelIdx).dollCount),
			std::max(0, levels.at(levelIdx).minvoCount)
		};

		std::pair<GameObject*, GameObject*> players;
		switch (gameMode)
		{
		case GameMode::Solo:
			players = SetupSoloMode(spawnParams);
			break;
		case GameMode::Coop:
			players = SetupCoopMode(spawnParams);
			break;
		case GameMode::Versus:
			players = SetupVersusMode(spawnParams);
			break;
		}
		GameObject* player1 = players.first;
		GameObject* player2 = players.second;

		auto* camera = cameraRoot->AddComponent<CameraComponent>(player1, windowWidth, playfieldScaledWidth);
		if (player2)
			camera->SetSecondTarget(player2);
		scene.Add(std::move(cameraRoot));

		if (!g_BombObserver)
			g_BombObserver = std::make_unique<BombEventObserver>(scene, tileWorldSize);
		if (!g_EntityDeathObserver)
			g_EntityDeathObserver = std::make_unique<EntityDeathObserver>(scene);

		std::vector<GameObject*> hudPlayers{};
		std::vector<int> hudLives{};
		std::vector<int> hudScores{};
		if (gameMode == GameMode::Coop)
		{
			hudPlayers.push_back(player1);
			hudLives.push_back(carryOver.health);
			hudScores.push_back(carryOver.score);
			hudPlayers.push_back(player2);
			hudLives.push_back(p2CarryOver.health);
			hudScores.push_back(p2CarryOver.score);
		}
		else
		{
			if (player1)
			{
				hudPlayers.push_back(player1);
				hudLives.push_back(carryOver.health);
				hudScores.push_back(carryOver.score);
			}
		}
		hudRoot->AddComponent<HudComponent>(windowWidth, hudHeight, hudPlayers, hudLives, hudScores);

		return { player1, player2, tileWorldSize };
	}

	void ResetGameplayObservers()
	{
		g_BombObserver.reset();
		g_EntityDeathObserver.reset();
	}
}
