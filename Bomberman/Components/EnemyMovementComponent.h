#pragma once
#include "BaseComponent.h"
#include <glm/vec2.hpp>

namespace dae
{
    class TransformComponent;
    class CollisionComponent;

    class EnemyMovementComponent final : public BaseComponent
    {
    public:
        EnemyMovementComponent(GameObject* pOwner, float moveSpeed, float minDirectionTime, float maxDirectionTime);

        void Update() override;

    private:
        TransformComponent* m_pTransform{};
        CollisionComponent* m_pCollision{};
        glm::vec2 m_direction{ 0.0f, 0.0f };
        float m_moveSpeed{};
        float m_minDirectionTime{};
        float m_maxDirectionTime{};
        float m_timeUntilDirectionChange{};

        void RefreshComponents();
        void ChooseNewDirection();
        float RandomRange(float min, float max) const;
    };
}
