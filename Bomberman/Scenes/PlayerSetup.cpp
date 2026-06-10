#include "PlayerSetup.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/PlayerAnimatorComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/DetonatorComponent.h"
#include "Components/SkateComponent.h"
#include "Components/DeathAnimatorComponent.h"
#include "EnemyConfig.h"
#include "Commands/MoveCommand.h"
#include "Commands/SpawnBombCommand.h"
#include "Commands/DetonateBombsCommand.h"
#include "Commands/SkipLevelCommand.h"
#include "Input/InputManager.h"
#include "Input/ControllerButtons.h"
#include "Input/KeyCodes.h"
#include <SDL3/SDL.h>

namespace
{
	constexpr float kPlayerMoveSpeed{ 120.0f };
	constexpr float kPlayerSpriteSize{ 16.0f };
	constexpr float kPlayerSpriteScale{ 3.0f };
	constexpr float kPlayerCollisionSize{ (kPlayerSpriteSize * kPlayerSpriteScale) - 20.0f };
	constexpr float kPlayerPivotY{ 1.0f - (kPlayerCollisionSize * 0.5f) / (kPlayerSpriteSize * kPlayerSpriteScale) };
}

namespace dae
{
	GameObject* CreatePlayer(Scene& scene, const glm::vec3& startPos, const PlayerCarryOver& carryOver)
	{
		auto go = std::make_unique<GameObject>();
		auto* transform = go->AddComponent<TransformComponent>();
		transform->SetLocalPosition(startPos);
		auto* render = go->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetSpriteSheet(16, 16, 6, 2);
		render->SetSprite(4, 0);
		render->SetScale(kPlayerSpriteScale);
		render->SetPivot({ 0.5f, kPlayerPivotY });
		render->SetRenderLayer(4);
		go->AddComponent<SpriteAnimatorComponent>();
		go->AddComponent<PlayerAnimatorComponent>();
		go->AddComponent<HealthComponent>(carryOver.health);
		go->AddComponent<ScoreComponent>(carryOver.score);
		go->AddComponent<BombRangeComponent>(4);
		go->AddComponent<BombCapacityComponent>(carryOver.bombCapacity);
		auto* detonator = go->AddComponent<DetonatorComponent>();
		if (carryOver.hasDetonator)
			detonator->SetHasDetonator(true);
		auto* skate = go->AddComponent<SkateComponent>();
		if (carryOver.hasSkate)
			skate->SetHasSkate(true);
		go->AddComponent<DeathAnimatorComponent>("BombermanSprites_General.png", BuildHorizontalFrames(0.0f, 32.0f, 7, 16.0f, 16.0f), 10.0f, kPlayerSpriteScale, true);
		auto* collider = go->AddComponent<CollisionComponent>(kPlayerCollisionSize, kPlayerCollisionSize);
		collider->SetOffset({ -kPlayerCollisionSize * 0.5f, -kPlayerCollisionSize * 0.5f });

		auto* rangeComp = go->GetComponent<BombRangeComponent>();
		for (int i = 1; i < carryOver.bombRange; ++i)
			rangeComp->IncreaseRange();

		auto* player = go.get();
		scene.Add(std::move(go));
		return player;
	}

	void BindKeyboardMovement(GameObject& player)
	{
		auto& input = InputManager::GetInstance();
		auto bindMove = [&](KeyCode key, const glm::vec3& dir)
		{
			auto cmd = std::make_unique<MoveCommand>(dir, kPlayerMoveSpeed);
			cmd->SetGameActor(&player);
			input.BindKeyboardInput(key, KeyState::Pressed, std::move(cmd));
		};

		bindMove(KeyCode::W, { 0, -1, 0 });
		bindMove(KeyCode::S, { 0, 1, 0 });
		bindMove(KeyCode::A, { -1, 0, 0 });
		bindMove(KeyCode::D, { 1, 0, 0 });

		auto bomb = std::make_unique<SpawnBombCommand>();
		bomb->SetGameActor(&player);
		input.BindKeyboardInput(KeyCode::R, KeyState::Down, std::move(bomb));

		auto detonate = std::make_unique<DetonateBombsCommand>();
		detonate->SetGameActor(&player);
		input.BindKeyboardInput(KeyCode::B, KeyState::Down, std::move(detonate));

		auto skip = std::make_unique<SkipLevelCommand>();
		skip->SetGameActor(&player);
		input.BindKeyboardInput(KeyCode::F1, KeyState::Down, std::move(skip));
	}

	void BindControllerMovement(GameObject& player, unsigned int controllerIndex)
	{
		auto& input = InputManager::GetInstance();
		if (!input.HasController(controllerIndex))
			return;

		auto bindMove = [&](unsigned int button, const glm::vec3& dir)
		{
			auto cmd = std::make_unique<MoveCommand>(dir, kPlayerMoveSpeed);
			cmd->SetGameActor(&player);
			input.BindControllerInput(controllerIndex, button, KeyState::Pressed, std::move(cmd));
		};

		bindMove(ControllerButton::kDpadUp, { 0, -1, 0 });
		bindMove(ControllerButton::kDpadDown, { 0, 1, 0 });
		bindMove(ControllerButton::kDpadLeft, { -1, 0, 0 });
		bindMove(ControllerButton::kDpadRight, { 1, 0, 0 });

		auto bomb = std::make_unique<SpawnBombCommand>();
		bomb->SetGameActor(&player);
		input.BindControllerInput(controllerIndex, ControllerButton::kY, KeyState::Down, std::move(bomb));

		auto detonate = std::make_unique<DetonateBombsCommand>();
		detonate->SetGameActor(&player);
		input.BindControllerInput(controllerIndex, ControllerButton::kB, KeyState::Down, std::move(detonate));
	}

	void BindPlayerControls(GameObject& player, bool keyboard, unsigned int controllerIndex)
	{
		if (keyboard)
			BindKeyboardMovement(player);
		BindControllerMovement(player, controllerIndex);
	}

	unsigned int GetSecondaryControllerIndex()
	{
		auto& input = InputManager::GetInstance();
		return input.HasController(1) ? 1u : 0u;
	}
}
