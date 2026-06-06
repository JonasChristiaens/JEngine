#include "GameplaySceneBuilder.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/PlayerAnimatorComponent.h"
#include "Components/CameraComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/PlayfieldComponent.h"
#include "Factories/EnemyFactory.h"
#include "Commands/MoveCommand.h"
#include "Commands/ChangeHealthCommand.h"
#include "Commands/ChangeScoreCommand.h"
#include "Commands/SpawnBombCommand.h"
#include "Observers/BombEventObserver.h"
#include "Observers/EntityDeathObserver.h"
#include "Audio/AudioEventObserver.h"
#include "Input/InputManager.h"
#include "Input/ControllerButtons.h"
#include "Rendering/Renderer.h"
#include "Level/LevelData.h"
#include "Level/LevelDataLoader.h"
#include "Resources/ResourceManager.h"

namespace
{
	dae::PlayfieldComponent::PlayfieldConfig ToPlayfieldConfig(const dae::LevelData& levelData)
	{
		dae::PlayfieldComponent::PlayfieldConfig config{};
		config.softBlockCount = levelData.softBlockCount;
		config.reservedTiles = levelData.reservedTiles;
		return config;
	}

	std::vector<dae::LevelData> LoadLevels()
	{
		const auto jsonText = dae::ResourceManager::GetInstance().LoadTextFile("levels.json");
		return dae::LevelDataLoader::LoadFromText(jsonText);
	}
}

namespace dae
{
	GameplaySceneData BuildGameplayScene(Scene& scene)
	{
		static std::unique_ptr<BombEventObserver> g_BombObserver{};
		static std::unique_ptr<EntityDeathObserver> g_EntityDeathObserver{};
		static std::unique_ptr<AudioEventObserver> g_AudioObserver{};

		const float playfieldWidth = 496.0f;
		const float playfieldHeight = 208.0f;
		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		const float windowWidth = static_cast<float>(windowSize.x);
		const float windowHeight = static_cast<float>(windowSize.y);
		const float playfieldScale = windowHeight / playfieldHeight;
		const float playfieldScaledWidth = playfieldWidth * playfieldScale;
		const float tileWorldSize = 16.0f * playfieldScale;

		auto cameraRoot = std::make_unique<GameObject>();
		auto cameraTransform = cameraRoot->AddComponent<TransformComponent>();
		cameraTransform->SetLocalPosition(0, 0);

		auto go = std::make_unique<GameObject>();
		go->AddComponent<TransformComponent>();
		auto render = go->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_Playfield.png");
		render->SetSourceRectangle(0, 0, playfieldWidth, playfieldHeight);
		render->SetScale(playfieldScale);
		render->SetRenderLayer(0);
		go->SetParent(cameraRoot.get(), false);
		scene.Add(std::move(go));

		const auto levels = LoadLevels();
		const size_t currentLevelIndex = 0;
		const auto& levelData = levels.at(currentLevelIndex);

		cameraRoot->AddComponent<PlayfieldComponent>(scene, playfieldWidth, playfieldHeight, playfieldScale, ToPlayfieldConfig(levelData));

		const glm::vec3 player1StartPos{ tileWorldSize * 1.5f, tileWorldSize * 2.0f, 0.0f };
		const glm::vec3 player2StartPos{ tileWorldSize * 3.5f, tileWorldSize * 2.0f, 0.0f };
		const std::vector<glm::vec3> playerStartPositions{ player1StartPos, player2StartPos };

		if (levelData.balloomCount > 0)
		{
			const int gridColumns = static_cast<int>(std::floor(playfieldWidth / 16.0f));
			const int gridRows = static_cast<int>(std::floor(playfieldHeight / 16.0f));
			const float balloomSpeed = 110.0f;
			for (int i = 0; i < levelData.balloomCount; ++i)
			{
				EnemyFactory::CreateBalloom(scene, *cameraRoot, gridColumns, gridRows, tileWorldSize, balloomSpeed, playerStartPositions);
			}
		}

		go = std::make_unique<GameObject>();
		auto transform = go->AddComponent<TransformComponent>();
		transform->SetLocalPosition(player1StartPos);
		render = go->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetSpriteSheet(16, 16, 6, 2);
		render->SetSprite(4, 0);
		render->SetScale(3.0f);
		render->SetPivot({ 0.5f, 0.5f });
		render->SetRenderLayer(4);
		go->AddComponent<SpriteAnimatorComponent>();
		go->AddComponent<PlayerAnimatorComponent>();
		go->AddComponent<HealthComponent>(4);
		go->AddComponent<ScoreComponent>(0);
		const float playerColliderSize = tileWorldSize * 0.8f;
		auto player1Collider = go->AddComponent<CollisionComponent>(playerColliderSize, playerColliderSize);
		player1Collider->SetOffset({ -playerColliderSize * 0.5f, -playerColliderSize * 0.5f });
		auto player1 = go.get();
		go->SetParent(cameraRoot.get(), false);
		scene.Add(std::move(go));

		go = std::make_unique<GameObject>();
		transform = go->AddComponent<TransformComponent>();
		transform->SetLocalPosition(player2StartPos);
		render = go->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetSpriteSheet(16, 16, 6, 2);
		render->SetSprite(4, 0);
		render->SetScale(3.0f);
		render->SetPivot({ 0.5f, 0.5f });
		render->SetRenderLayer(4);
		go->AddComponent<SpriteAnimatorComponent>();
		go->AddComponent<PlayerAnimatorComponent>();
		go->AddComponent<HealthComponent>(4);
		go->AddComponent<ScoreComponent>(0);
		auto player2Collider = go->AddComponent<CollisionComponent>(playerColliderSize, playerColliderSize);
		player2Collider->SetOffset({ -playerColliderSize * 0.5f, -playerColliderSize * 0.5f });
		auto player2 = go.get();
		go->SetParent(cameraRoot.get(), false);
		scene.Add(std::move(go));

		cameraRoot->AddComponent<CameraComponent>(player1, windowWidth, playfieldScaledWidth);
		scene.Add(std::move(cameraRoot));

		const float moveSpeed = 150.0f;
		auto& input = InputManager::GetInstance();

		auto moveUpP1 = std::make_unique<MoveCommand>(glm::vec3(0, -1, 0), moveSpeed);
		moveUpP1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_W, KeyState::Pressed, std::move(moveUpP1));

		auto moveDownP1 = std::make_unique<MoveCommand>(glm::vec3(0, 1, 0), moveSpeed);
		moveDownP1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_S, KeyState::Pressed, std::move(moveDownP1));

		auto moveLeftP1 = std::make_unique<MoveCommand>(glm::vec3(-1, 0, 0), moveSpeed);
		moveLeftP1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_A, KeyState::Pressed, std::move(moveLeftP1));

		auto moveRightP1 = std::make_unique<MoveCommand>(glm::vec3(1, 0, 0), moveSpeed);
		moveRightP1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_D, KeyState::Pressed, std::move(moveRightP1));

		auto damageP1 = std::make_unique<ChangeHealthCommand>(-1);
		damageP1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_C, KeyState::Down, std::move(damageP1));

		auto score10P1 = std::make_unique<ChangeScoreCommand>(10);
		score10P1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_Z, KeyState::Down, std::move(score10P1));

		auto score100P1 = std::make_unique<ChangeScoreCommand>(100);
		score100P1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_X, KeyState::Down, std::move(score100P1));

		auto spawnBombP1 = std::make_unique<SpawnBombCommand>();
		spawnBombP1->SetGameActor(player1);
		input.BindKeyboardInput(SDLK_R, KeyState::Down, std::move(spawnBombP1));

		const unsigned int controllerIndex = 0;
		input.AddController(controllerIndex);

		auto moveUpP2 = std::make_unique<MoveCommand>(glm::vec3(0, -1, 0), moveSpeed * 2);
		moveUpP2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::DPadUp, KeyState::Pressed, std::move(moveUpP2));

		auto moveDownP2 = std::make_unique<MoveCommand>(glm::vec3(0, 1, 0), moveSpeed * 2);
		moveDownP2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::DPadDown, KeyState::Pressed, std::move(moveDownP2));

		auto moveLeftP2 = std::make_unique<MoveCommand>(glm::vec3(-1, 0, 0), moveSpeed * 2);
		moveLeftP2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::DPadLeft, KeyState::Pressed, std::move(moveLeftP2));

		auto moveRightP2 = std::make_unique<MoveCommand>(glm::vec3(1, 0, 0), moveSpeed * 2);
		moveRightP2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::DPadRight, KeyState::Pressed, std::move(moveRightP2));

		auto damageP2 = std::make_unique<ChangeHealthCommand>(-1);
		damageP2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::X, KeyState::Down, std::move(damageP2));

		auto score10P2 = std::make_unique<ChangeScoreCommand>(10);
		score10P2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::A, KeyState::Down, std::move(score10P2));

		auto score100P2 = std::make_unique<ChangeScoreCommand>(100);
		score100P2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::B, KeyState::Down, std::move(score100P2));

		auto spawnBombP2 = std::make_unique<SpawnBombCommand>();
		spawnBombP2->SetGameActor(player2);
		input.BindControllerInput(controllerIndex, ControllerButton::Y, KeyState::Down, std::move(spawnBombP2));

		if (!g_BombObserver)
			g_BombObserver = std::make_unique<BombEventObserver>(scene, tileWorldSize);

		if (!g_EntityDeathObserver)
			g_EntityDeathObserver = std::make_unique<EntityDeathObserver>(scene);

		if (!g_AudioObserver)
			g_AudioObserver = std::make_unique<AudioEventObserver>();

		return { player1, player2, tileWorldSize };
	}
}
