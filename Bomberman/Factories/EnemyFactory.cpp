#include "EnemyFactory.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/EnemyComponent.h"
#include "Components/EnemyMovementComponent.h"
#include "Components/StateMachineComponent.h"
#include "Components/DeathAnimatorComponent.h"
#include "State/EnemyIdleState.h"
#include "Components/RectBounds.h"
#include <random>
#include <glm/vec3.hpp>

namespace
{
	std::mt19937& GetRng()
	{
		static std::random_device device{};
		static std::mt19937 rng{ device() };
		return rng;
	}

	bool IsBlockedAtPosition(const dae::GameObject& parent, const glm::vec3& spawnPos, float colliderSize)
	{
		const dae::RectBounds spawnBox = dae::RectBounds::FromCenterSize(spawnPos.x, spawnPos.y, colliderSize, colliderSize);

		for (const auto* child : parent.GetChildren())
		{
			if (!child || child->IsMarkedForDeletion())
				continue;

			auto* collider = child->GetComponent<dae::CollisionComponent>();
			if (!collider || collider->IsTrigger())
				continue;

			auto* transform = child->GetComponent<dae::TransformComponent>();
			if (!transform)
				continue;

			const auto otherPos = transform->GetLocalPosition();
			const dae::RectBounds otherBox = dae::RectBounds::FromOffset(otherPos.x, otherPos.y,
				collider->GetWidth(), collider->GetHeight(),
				collider->GetOffset().x, collider->GetOffset().y);

			if (spawnBox.Overlaps(otherBox))
			{
				return true;
			}
		}

		return false;
	}

	std::pair<int, int> WorldToTile(const glm::vec3& worldPos, float tileWorldSize)
	{
		return {
			static_cast<int>(worldPos.x / tileWorldSize),
			static_cast<int>(worldPos.y / tileWorldSize)
		};
	}

	glm::vec3 SelectSpawnPosition(const dae::GameObject& parent, int gridColumns, int gridRows,
		float tileWorldSize, float colliderSize,
		const std::vector<glm::vec3>& reservedWorldPositions)
	{
		std::uniform_int_distribution<int> columnDist(0, gridColumns - 1);
		std::uniform_int_distribution<int> rowDist(0, gridRows - 1);
		std::uniform_real_distribution<float> fallbackOffset(0.0f, tileWorldSize * 0.5f);

		std::vector<std::pair<int, int>> reservedTiles{};
		reservedTiles.reserve(reservedWorldPositions.size());
		for (const auto& pos : reservedWorldPositions)
			reservedTiles.push_back(WorldToTile(pos, tileWorldSize));

		constexpr int maxAttempts{ 128 };
		for (int attempt = 0; attempt < maxAttempts; ++attempt)
		{
			const int column = columnDist(GetRng());
			const int row = rowDist(GetRng());

			bool isReserved = false;
			for (const auto& [reservedCol, reservedRow] : reservedTiles)
			{
				if (column == reservedCol && row == reservedRow)
				{
					isReserved = true;
					break;
				}
			}
			if (isReserved)
				continue;

			const float spawnX = (static_cast<float>(column) + 0.5f) * tileWorldSize;
			const float spawnY = (static_cast<float>(row) + 0.5f) * tileWorldSize;
			const glm::vec3 spawnPos{ spawnX, spawnY, 0.0f };

			if (!IsBlockedAtPosition(parent, spawnPos, colliderSize))
				return spawnPos;
		}

		return { tileWorldSize + fallbackOffset(GetRng()), tileWorldSize + fallbackOffset(GetRng()), 0.0f };
	}
}

namespace dae::EnemyFactory
{
	GameObject* CreateEnemy(Scene& scene, GameObject& parent, int gridColumns, int gridRows, float tileWorldSize, float moveSpeed,
		const EnemyConfig& config, GameObject* pChaseTarget, const std::vector<glm::vec3>& reservedWorldPositions, bool useAiMovement)
	{
		auto enemy = std::make_unique<GameObject>();
		auto* transform = enemy->AddComponent<TransformComponent>();
		const float colliderSize = tileWorldSize * config.colliderScale;
		transform->SetLocalPosition(SelectSpawnPosition(parent, gridColumns, gridRows, tileWorldSize, colliderSize, reservedWorldPositions));

		auto* render = enemy->AddComponent<RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetSourceRectangle(config.spriteX, config.spriteY, config.spriteWidth, config.spriteHeight);
		render->SetScale(config.scale);
		render->SetPivot({ 0.5f, 0.5f });
		render->SetRenderLayer(4);

		enemy->AddComponent<SpriteAnimatorComponent>();
		auto* stateMachine = enemy->AddComponent<StateMachineComponent>();
		enemy->AddComponent<EnemyComponent>(config.points);

		if (useAiMovement)
		{
			auto* movement = enemy->AddComponent<EnemyMovementComponent>(moveSpeed, config.minDirectionTime, config.maxDirectionTime);
			if (pChaseTarget && config.chaseAxis != EnemyChaseAxis::None)
			{
				movement->SetChaseTarget(pChaseTarget);
				movement->SetChaseAxis(config.chaseAxis);
				movement->SetChaseAlignmentThreshold(tileWorldSize);
			}
		}
		enemy->AddComponent<HealthComponent>(1);

		auto* collider = enemy->AddComponent<CollisionComponent>(colliderSize, colliderSize);
		collider->SetOffset({ -colliderSize * 0.5f, -colliderSize * 0.5f });

		stateMachine->GetStateMachine().SetState(std::make_unique<EnemyIdleState>(enemy.get(), config.spriteX, config.spriteY, config.spriteWidth, config.spriteHeight, config.scale));

		if (!config.deathFrames.empty())
		{
			enemy->AddComponent<DeathAnimatorComponent>("BombermanSprites_General.png", config.deathFrames, config.deathFps, config.scale);
		}

		enemy->SetParent(&parent, false);
		auto* enemyPtr = enemy.get();
		scene.Add(std::move(enemy));
		return enemyPtr;
	}
}
