#pragma once
#include "BaseComponent.h"
#include <glm/vec2.hpp>

namespace dae
{
	class TransformComponent;
	class CollisionComponent;
	class GameObject;

	enum class EnemyChaseAxis
	{
		None,
		X,
		Y,
		Both
	};

	class EnemyMovementComponent final : public BaseComponent
	{
	public:
		EnemyMovementComponent(GameObject* pOwner, float moveSpeed, float minDirectionTime, float maxDirectionTime);

		void Update() override;

		void SetChaseTarget(GameObject* pTarget) { m_pChaseTarget = pTarget; }
		void SetChaseAxis(EnemyChaseAxis axis) { m_ChaseAxis = axis; }
		void SetChaseAlignmentThreshold(float threshold) { m_ChaseAlignmentThreshold = threshold; }

	private:
		TransformComponent* m_pTransform{};
		CollisionComponent* m_pCollision{};
		glm::vec2 m_Direction{ 0.0f, 0.0f };
		float m_MoveSpeed{};
		float m_MinDirectionTime{};
		float m_MaxDirectionTime{};
		float m_TimeUntilDirectionChange{};

		GameObject* m_pChaseTarget{};
		EnemyChaseAxis m_ChaseAxis{ EnemyChaseAxis::None };
		float m_ChaseAlignmentThreshold{ 24.0f };

		void RefreshComponents();
		void ChooseNewDirection();
		void PickRandomDirection();
		float RandomRange(float min, float max) const;
	};
}
