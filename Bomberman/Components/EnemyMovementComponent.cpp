#include "EnemyMovementComponent.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Core/GameTime.h"
#include <glm/glm.hpp>
#include <random>
#include <array>
#include <utility>

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

		if (m_pChaseTarget && m_ChaseAxis != EnemyChaseAxis::None)
		{
			auto* targetTransform = m_pChaseTarget->GetComponent<TransformComponent>();
			if (targetTransform)
			{
				const glm::vec3 targetPos = targetTransform->GetWorldPosition();
				const glm::vec3 myPos = m_pTransform->GetWorldPosition();
				const float tileSize = m_ChaseAlignmentThreshold;
				const float innerHalf = tileSize * 0.45f;

				const int myCellCol = static_cast<int>(myPos.x / tileSize);
				const int myCellRow = static_cast<int>(myPos.y / tileSize);
				const float myCellCenterX = (static_cast<float>(myCellCol) + 0.5f) * tileSize;
				const float myCellCenterY = (static_cast<float>(myCellRow) + 0.5f) * tileSize;

				glm::vec2 chaseDir{};

				const bool checkY = m_ChaseAxis == EnemyChaseAxis::Y || m_ChaseAxis == EnemyChaseAxis::Both;
				const bool checkX = m_ChaseAxis == EnemyChaseAxis::X || m_ChaseAxis == EnemyChaseAxis::Both;

				bool canChaseY = false;
				bool canChaseX = false;

				float colliderLeft = myPos.x;
				float colliderRight = myPos.x;
				float colliderTop = myPos.y;
				float colliderBottom = myPos.y;
				if (m_pCollision)
				{
					colliderLeft = myPos.x + m_pCollision->GetOffset().x;
					colliderRight = colliderLeft + m_pCollision->GetWidth();
					colliderTop = myPos.y + m_pCollision->GetOffset().y;
					colliderBottom = colliderTop + m_pCollision->GetHeight();
				}

				if (checkY)
				{
					const float columnLeft = myCellCenterX - innerHalf;
					const float columnRight = myCellCenterX + innerHalf;
					const bool enemyInColumn = colliderLeft >= columnLeft && colliderRight <= columnRight;
					const bool targetInColumn = std::abs(targetPos.x - myCellCenterX) <= innerHalf;

					if (enemyInColumn && targetInColumn && IsLineOfSightClear(myPos, targetPos, tileSize, false))
						canChaseY = true;
				}
				if (checkX)
				{
					const float rowTop = myCellCenterY - innerHalf;
					const float rowBottom = myCellCenterY + innerHalf;
					const bool enemyInRow = colliderTop >= rowTop && colliderBottom <= rowBottom;
					const bool targetInRow = std::abs(targetPos.y - myCellCenterY) <= innerHalf;

					if (enemyInRow && targetInRow && IsLineOfSightClear(myPos, targetPos, tileSize, true))
						canChaseX = true;
				}

				if (canChaseX && canChaseY)
				{
					std::uniform_int_distribution<int> axisDist(0, 1);
					if (axisDist(GetRng()) == 0)
						chaseDir = { 0.0f, (targetPos.y > myPos.y) ? 1.0f : -1.0f };
					else
						chaseDir = { (targetPos.x > myPos.x) ? 1.0f : -1.0f, 0.0f };
				}
				else if (canChaseY)
				{
					chaseDir = { 0.0f, (targetPos.y > myPos.y) ? 1.0f : -1.0f };
				}
				else if (canChaseX)
				{
					chaseDir = { (targetPos.x > myPos.x) ? 1.0f : -1.0f, 0.0f };
				}

				const bool isChasing = chaseDir.x != 0.0f || chaseDir.y != 0.0f;

				if (m_WasChasing && !isChasing)
				{
					PickRandomDirection();
					m_TimeUntilDirectionChange = RandomRange(m_MinDirectionTime, m_MaxDirectionTime);
				}
				m_WasChasing = isChasing;

				if (isChasing)
				{
					m_Direction = chaseDir;
					glm::vec2 delta = m_Direction * m_MoveSpeed * deltaTime;
					glm::vec3 nextWorld = m_pTransform->GetWorldPosition() + glm::vec3(delta.x, delta.y, 0.0f);

					if (!m_pCollision || !m_pCollision->WouldCollide(nextWorld))
					{
						glm::vec3 currentLocal = m_pTransform->GetLocalPosition();
						m_pTransform->SetLocalPosition(currentLocal + glm::vec3(delta.x, delta.y, 0.0f));
					}
					else
					{
						PickRandomDirection();
						m_TimeUntilDirectionChange = RandomRange(m_MinDirectionTime, m_MaxDirectionTime);
						m_WasChasing = false;
					}
					return;
				}
			}
		}
		else
		{
			m_WasChasing = false;
		}

		glm::vec2 delta = m_Direction * m_MoveSpeed * deltaTime;
		glm::vec3 nextLocal = m_pTransform->GetLocalPosition() + glm::vec3(delta.x, delta.y, 0.0f);
		glm::vec3 nextWorld = m_pTransform->GetWorldPosition() + glm::vec3(delta.x, delta.y, 0.0f);

		if (!m_pCollision || !m_pCollision->WouldCollide(nextWorld))
		{
			m_pTransform->SetLocalPosition(nextLocal);
		}
		else
		{
			OnBlocked();
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
		PickRandomDirection();
		m_TimeUntilDirectionChange = RandomRange(m_MinDirectionTime, m_MaxDirectionTime);
	}

	void EnemyMovementComponent::OnBlocked()
	{
		PickRandomDirection();
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

	bool EnemyMovementComponent::IsLineOfSightClear(const glm::vec3& from, const glm::vec3& to, float tileSize, bool horizontal) const
	{
		GameObject* pParent = GetOwner()->GetParent();
		if (!pParent)
			return true;

		int fromCell{ 0 }, toCell{ 0 }, fixedAxis{ 0 };
		if (horizontal)
		{
			fromCell = static_cast<int>(from.x / tileSize);
			toCell = static_cast<int>(to.x / tileSize);
			fixedAxis = static_cast<int>(from.y / tileSize);
		}
		else
		{
			fromCell = static_cast<int>(from.y / tileSize);
			toCell = static_cast<int>(to.y / tileSize);
			fixedAxis = static_cast<int>(from.x / tileSize);
		}

		if (fromCell > toCell)
			std::swap(fromCell, toCell);

		for (int c = fromCell + 1; c < toCell; ++c)
		{
			for (const auto* child : pParent->GetChildren())
			{
				if (!child || child->IsMarkedForDeletion())
					continue;

				auto* collider = child->GetComponent<CollisionComponent>();
				if (!collider || collider->IsTrigger())
					continue;

				auto* transform = child->GetComponent<TransformComponent>();
				if (!transform)
					continue;

				const auto& pos = transform->GetWorldPosition();
				int childCell, childFixed;
				if (horizontal)
				{
					childCell = static_cast<int>(pos.x / tileSize);
					childFixed = static_cast<int>(pos.y / tileSize);
				}
				else
				{
					childCell = static_cast<int>(pos.y / tileSize);
					childFixed = static_cast<int>(pos.x / tileSize);
				}

				if (childCell == c && childFixed == fixedAxis)
					return false;
			}
		}

		return true;
	}
}
