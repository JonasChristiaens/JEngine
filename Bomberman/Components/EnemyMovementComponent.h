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
		glm::vec2 m_Direction{ 0.0f, 0.0f };
		float m_MoveSpeed{};
		float m_MinDirectionTime{};
		float m_MaxDirectionTime{};
		float m_TimeUntilDirectionChange{};

		void RefreshComponents();
		void ChooseNewDirection();
		float RandomRange(float min, float max) const;
	};
}
