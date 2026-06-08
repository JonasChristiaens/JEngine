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
		, m_MoveSpeed(moveSpeed)
		, m_MinDirectionTime(minDirectionTime)
		, m_MaxDirectionTime(maxDirectionTime)
		, m_TimeUntilDirectionChange(RandomRange(minDirectionTime, maxDirectionTime))
	{
		ChooseNewDirection();
	}

	void EnemyMovementComponent::Update()
	{
		if (!m_Enabled)
			return;

		RefreshComponents();
		if (!m_pTransform)
			return;

		const float deltaTime = GameTime::GetInstance().GetDeltaTime();
		glm::vec2 delta = m_Direction * m_MoveSpeed * deltaTime;
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

		m_TimeUntilDirectionChange -= deltaTime;
		if (m_TimeUntilDirectionChange <= 0.0f)
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
		if (m_pChaseTarget && m_ChaseAxis != EnemyChaseAxis::None)
		{
			auto* targetTransform = m_pChaseTarget->GetComponent<TransformComponent>();
			if (m_pTransform && targetTransform)
			{
				const glm::vec3 targetPos = targetTransform->GetWorldPosition();
				const glm::vec3 myPos = m_pTransform->GetWorldPosition();

				const bool alignedX = std::abs(myPos.x - targetPos.x) <= m_ChaseAlignmentThreshold;
				const bool alignedY = std::abs(myPos.y - targetPos.y) <= m_ChaseAlignmentThreshold;

				if (m_ChaseAxis == EnemyChaseAxis::Y && alignedX)
				{
					m_Direction = glm::vec2(0.0f, (targetPos.y > myPos.y) ? 1.0f : -1.0f);
				}
				else if (m_ChaseAxis == EnemyChaseAxis::X && alignedY)
				{
					m_Direction = glm::vec2((targetPos.x > myPos.x) ? 1.0f : -1.0f, 0.0f);
				}
				else if (m_ChaseAxis == EnemyChaseAxis::Both && (alignedX || alignedY))
				{
					if (alignedX && alignedY)
					{
						std::uniform_int_distribution<int> axisDist(0, 1);
						if (axisDist(GetRng()) == 0)
						{
							m_Direction = glm::vec2(0.0f, (targetPos.y > myPos.y) ? 1.0f : -1.0f);
						}
						else
						{
							m_Direction = glm::vec2((targetPos.x > myPos.x) ? 1.0f : -1.0f, 0.0f);
						}
					}
					else if (alignedX)
					{
						m_Direction = glm::vec2(0.0f, (targetPos.y > myPos.y) ? 1.0f : -1.0f);
					}
					else
					{
						m_Direction = glm::vec2((targetPos.x > myPos.x) ? 1.0f : -1.0f, 0.0f);
					}
				}
				else
				{
					PickRandomDirection();
				}
			}
			else
			{
				PickRandomDirection();
			}
		}
		else
		{
			PickRandomDirection();
		}

		m_TimeUntilDirectionChange = RandomRange(m_MinDirectionTime, m_MaxDirectionTime);
	}

	void EnemyMovementComponent::PickRandomDirection()
	{
		static const std::array<glm::vec2, 4> directions{
			glm::vec2{ 1.0f, 0.0f },
			glm::vec2{ -1.0f, 0.0f },
			glm::vec2{ 0.0f, 1.0f },
			glm::vec2{ 0.0f, -1.0f }
		};

		std::uniform_int_distribution<int> directionDist(0, static_cast<int>(directions.size()) - 1);
		m_Direction = directions[directionDist(GetRng())];
	}

	float EnemyMovementComponent::RandomRange(float min, float max) const
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(GetRng());
	}
}
