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
#include "Components/StateStackComponent.h"
#include "State/BalloomIdleState.h"
#include <random>
#include <glm/vec3.hpp>

namespace
{
    constexpr float kBalloomScale = 3.0f;
    constexpr float kBalloomColliderScale = 0.8f;
    constexpr float kBalloomMinDirectionTime = 2.0f;
    constexpr float kBalloomMaxDirectionTime = 4.0f;
    constexpr float kBalloomSpriteSheetX = 0.0f;
    constexpr float kBalloomSpriteSheetY = 241.0f;
    constexpr float kBalloomSpriteWidth = 16.0f;
    constexpr float kBalloomSpriteHeight = 16.0f;
    constexpr float kBalloomSpriteSheetWidth = 96.0f;
}

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
        const float left1 = spawnPos.x - colliderSize * 0.5f;
        const float right1 = left1 + colliderSize;
        const float top1 = spawnPos.y - colliderSize * 0.5f;
        const float bottom1 = top1 + colliderSize;

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

            const auto otherPos = transform->GetWorldPosition();
            const float left2 = otherPos.x + collider->GetOffset().x;
            const float right2 = left2 + collider->GetWidth();
            const float top2 = otherPos.y + collider->GetOffset().y;
            const float bottom2 = top2 + collider->GetHeight();

            if (left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2)
            {
                return true;
            }
        }

        return false;
    }

    // Convert world pos to tile
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

        // store reserved tiles
        std::vector<std::pair<int, int>> reservedTiles{};
        reservedTiles.reserve(reservedWorldPositions.size());
        for (const auto& pos : reservedWorldPositions)
            reservedTiles.push_back(WorldToTile(pos, tileWorldSize));

        constexpr int maxAttempts = 128;
        for (int attempt = 0; attempt < maxAttempts; ++attempt)
        {
            const int column = columnDist(GetRng());
            const int row = rowDist(GetRng());

            // check for overlap
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
    GameObject* CreateBalloom(Scene& scene, GameObject& parent, int gridColumns, int gridRows, float tileWorldSize, float moveSpeed,
        const std::vector<glm::vec3>& reservedWorldPositions)
    {
        auto enemy = std::make_unique<GameObject>();
        auto* transform = enemy->AddComponent<TransformComponent>();
        const float colliderSize = tileWorldSize * kBalloomColliderScale;
        transform->SetLocalPosition(SelectSpawnPosition(parent, gridColumns, gridRows, tileWorldSize, colliderSize, reservedWorldPositions));

        auto* render = enemy->AddComponent<RenderComponent>();
        render->SetTexture("Textures/BombermanSprites_General.png");
        render->SetSourceRectangle(kBalloomSpriteSheetX, kBalloomSpriteSheetY, kBalloomSpriteWidth, kBalloomSpriteHeight);
        render->SetScale(kBalloomScale);
        render->SetPivot({ 0.5f, 0.5f });
        render->SetRenderLayer(4);

        enemy->AddComponent<SpriteAnimatorComponent>();
        auto* enemyComp = enemy->AddComponent<EnemyComponent>();
        auto* stateStack = enemyComp->GetStateStackComponent();

        enemy->AddComponent<EnemyMovementComponent>(moveSpeed, kBalloomMinDirectionTime, kBalloomMaxDirectionTime);
        enemy->AddComponent<HealthComponent>(1);

        auto* collider = enemy->AddComponent<CollisionComponent>(colliderSize, colliderSize);
        collider->SetOffset({ -colliderSize * 0.5f, -colliderSize * 0.5f });

        if (stateStack)
        {
            stateStack->GetStateStack().Push(std::make_unique<BalloomIdleState>(enemy.get()));
        }

        enemy->SetParent(&parent, false);
        auto* enemyPtr = enemy.get();
        scene.Add(std::move(enemy));
        return enemyPtr;
    }
}