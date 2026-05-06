#include "EnemyMovementComponent.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "GameTime.h"
#include <glm/glm.hpp>
#include <random>
#include <array>

namespace
{
    std::mt19937& GetRng()
    {
        static std::random_device device{};
        static std::mt19937 rng{ device() };
        return rng;
    }
}

namespace dae
{
    EnemyMovementComponent::EnemyMovementComponent(GameObject* pOwner, float moveSpeed, float minDirectionTime, float maxDirectionTime)
        : BaseComponent(pOwner)
        , m_moveSpeed(moveSpeed)
        , m_minDirectionTime(minDirectionTime)
        , m_maxDirectionTime(maxDirectionTime)
        , m_timeUntilDirectionChange(RandomRange(minDirectionTime, maxDirectionTime))
    {
        ChooseNewDirection();
    }

    void EnemyMovementComponent::Update()
    {
        RefreshComponents();
        if (!m_pTransform)
            return;

        float deltaTime = GameTime::GetInstance().GetDeltaTime();
        glm::vec2 delta = m_direction * m_moveSpeed * deltaTime;
        glm::vec3 currentPos = m_pTransform->GetLocalPosition();
        glm::vec3 nextPos = currentPos + glm::vec3(delta.x, delta.y, 0.0f);
        glm::vec3 nextWorld = m_pTransform->GetWorldPosition() + glm::vec3(delta.x, delta.y, 0.0f);

        bool canMove = true;
        if (m_pCollision)
        {
            canMove = !m_pCollision->WouldCollide(nextWorld);
        }

        if (canMove)
        {
            m_pTransform->SetLocalPosition(nextPos);
        }
        else
        {
            ChooseNewDirection();
        }

        m_timeUntilDirectionChange -= deltaTime;
        if (m_timeUntilDirectionChange <= 0.0f)
        {
            ChooseNewDirection();
        }
    }

    void EnemyMovementComponent::RefreshComponents()
    {
        if (!m_pTransform)
            m_pTransform = GetOwner()->GetComponent<TransformComponent>();
        if (!m_pCollision)
            m_pCollision = GetOwner()->GetComponent<CollisionComponent>();
    }

    void EnemyMovementComponent::ChooseNewDirection()
    {
        static const std::array<glm::vec2, 4> directions{
            glm::vec2{ 1.0f, 0.0f },
            glm::vec2{ -1.0f, 0.0f },
            glm::vec2{ 0.0f, 1.0f },
            glm::vec2{ 0.0f, -1.0f }
        };

        std::uniform_int_distribution<int> directionDist(0, static_cast<int>(directions.size()) - 1);
        m_direction = directions[directionDist(GetRng())];
        m_timeUntilDirectionChange = RandomRange(m_minDirectionTime, m_maxDirectionTime);
    }

    float EnemyMovementComponent::RandomRange(float min, float max) const
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(GetRng());
    }
}
