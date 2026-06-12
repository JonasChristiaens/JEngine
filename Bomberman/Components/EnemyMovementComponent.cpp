#include "EnemyMovementComponent.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/CollisionGrid.h"
#include "Components/PlayfieldComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/EnemyComponent.h"
#include "Components/HealthComponent.h"
#include "Core/GameTime.h"
#include "EventQueue/EventManager.h"
#include <glm/glm.hpp>
#include <random>
#include <array>
#include <utility>
#include <algorithm>

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
		if (m_pCollision && !m_FilterSet)
		{
			m_pCollision->SetCollisionFilter([](GameObject* other)
				{
					return other->HasComponent<EnemyComponent>();
				});
			m_FilterSet = true;
		}
		if (!m_pTransform)
			return;

		const float deltaTime = GameTime::GetInstance().GetDeltaTime();

		auto* chaseTarget = FindClosestChaseTarget();
		if (chaseTarget && m_ChaseAxis != EnemyChaseAxis::None)
		{
			auto* targetTransform = chaseTarget->GetComponent<TransformComponent>();
			if (targetTransform)
			{
				const glm::vec3 targetLocal = targetTransform->GetLocalPosition();
				const glm::vec3 myLocal = m_pTransform->GetLocalPosition();
				const float tileSize = m_ChaseAlignmentThreshold;

				const int myCellCol = static_cast<int>(myLocal.x / tileSize);
				const int myCellRow = static_cast<int>(myLocal.y / tileSize);
				const int targetCellCol = static_cast<int>(targetLocal.x / tileSize);
				const int targetCellRow = static_cast<int>(targetLocal.y / tileSize);

				const float cellLeft = static_cast<float>(myCellCol) * tileSize;
				const float cellTop = static_cast<float>(myCellRow) * tileSize;
				const float cellRight = cellLeft + tileSize;
				const float cellBottom = cellTop + tileSize;
				const glm::vec3 myCellCenter(
					cellLeft + tileSize * 0.5f,
					cellTop + tileSize * 0.5f,
					0.0f
				);

				float myColliderLeft = myLocal.x;
				float myColliderRight = myLocal.x;
				float myColliderTop = myLocal.y;
				float myColliderBottom = myLocal.y;
				if (m_pCollision)
				{
					myColliderLeft = myLocal.x + m_pCollision->GetOffset().x;
					myColliderRight = myColliderLeft + m_pCollision->GetWidth();
					myColliderTop = myLocal.y + m_pCollision->GetOffset().y;
					myColliderBottom = myColliderTop + m_pCollision->GetHeight();
				}
				const bool enemyInCell = myColliderLeft >= cellLeft && myColliderRight <= cellRight
					&& myColliderTop >= cellTop && myColliderBottom <= cellBottom;

				glm::vec2 chaseDir{};

				const bool checkY = m_ChaseAxis == EnemyChaseAxis::Y || m_ChaseAxis == EnemyChaseAxis::Both;
				const bool checkX = m_ChaseAxis == EnemyChaseAxis::X || m_ChaseAxis == EnemyChaseAxis::Both;

				bool canChaseY = false;
				bool canChaseX = false;

				if (checkY)
				{
					const bool sameCol = targetCellCol == myCellCol;
					const bool losClear = IsLineOfSightClear(myCellCenter, targetLocal, tileSize, false);
					if (sameCol && losClear)
					{
						if (enemyInCell)
							canChaseY = true;
						else if (m_WasChasing)
							canChaseY = true;
					}
				}
				if (checkX)
				{
					const bool sameRow = targetCellRow == myCellRow;
					const bool losClear = IsLineOfSightClear(myCellCenter, targetLocal, tileSize, true);
					if (sameRow && losClear)
					{
						if (enemyInCell)
							canChaseX = true;
						else if (m_WasChasing)
							canChaseX = true;
					}
				}

				if (canChaseX && canChaseY)
				{
					std::uniform_int_distribution<int> axisDist(0, 1);
					if (axisDist(GetRng()) == 0)
						chaseDir = { 0.0f, (targetLocal.y > myLocal.y) ? 1.0f : -1.0f };
					else
						chaseDir = { (targetLocal.x > myLocal.x) ? 1.0f : -1.0f, 0.0f };
				}
				else if (canChaseY)
				{
					chaseDir = { 0.0f, (targetLocal.y > myLocal.y) ? 1.0f : -1.0f };
				}
				else if (canChaseX)
				{
					chaseDir = { (targetLocal.x > myLocal.x) ? 1.0f : -1.0f, 0.0f };
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
						OnBlocked(nextWorld);
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
			OnBlocked(nextWorld);
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

	void EnemyMovementComponent::OnBlocked(const glm::vec3& attemptedWorldPos)
	{
		if (m_pCollision)
		{
			std::vector<CollisionComponent*> candidates{};
			candidates.reserve(8);
			CollisionGrid::Query({ attemptedWorldPos.x + m_pCollision->GetOffset().x, attemptedWorldPos.y + m_pCollision->GetOffset().y, 0.0f },
				m_pCollision->GetWidth(), m_pCollision->GetHeight(), candidates);

			for (auto* other : candidates)
			{
				if (other == m_pCollision)
					continue;
				if (other->IsTrigger())
					continue;
				auto* otherOwner = other->GetOwner();
				if (!otherOwner || otherOwner->IsMarkedForDeletion())
					continue;
				if (!otherOwner->HasComponent<BombRangeComponent>())
					continue;

				auto* otherTx = otherOwner->GetComponent<TransformComponent>();
				if (!otherTx)
					continue;

				if (CollisionComponent::RectOverlaps(
					attemptedWorldPos.x + m_pCollision->GetOffset().x,
					attemptedWorldPos.y + m_pCollision->GetOffset().y,
					m_pCollision->GetWidth(), m_pCollision->GetHeight(),
					otherTx->GetWorldPosition().x + other->GetOffset().x,
					otherTx->GetWorldPosition().y + other->GetOffset().y,
					other->GetWidth(), other->GetHeight()))
				{
					Event damageEvent(make_sdbm_hash("ChangeHealthEvent"));
					damageEvent.nbArgs = 1;
					damageEvent.args[0].i = -1;
					EventManager::GetInstance().BroadcastEvent(damageEvent, otherOwner);
					break;
				}
			}
		}

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

		const glm::vec2 oldDir = m_Direction;
		std::uniform_int_distribution<int> directionDist(0, static_cast<int>(directions.size()) - 1);
		m_Direction = directions[directionDist(GetRng())];

		const bool axisChanged = (oldDir.x != 0.0f) != (m_Direction.x != 0.0f);
		if (axisChanged && m_pTransform && m_ChaseAlignmentThreshold > 0.0f)
		{
			const float tileSize = m_ChaseAlignmentThreshold;
			glm::vec3 pos = m_pTransform->GetLocalPosition();

			if (m_Direction.x != 0.0f)
			{
				const int row = static_cast<int>(pos.y / tileSize);
				pos.y = (static_cast<float>(row) + 0.5f) * tileSize;
			}
			else
			{
				const int col = static_cast<int>(pos.x / tileSize);
				pos.x = (static_cast<float>(col) + 0.5f) * tileSize;
			}

			m_pTransform->SetLocalPosition(pos);
		}
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

		auto* playfield = pParent->GetComponent<PlayfieldComponent>();
		if (!playfield)
			return true;

		const auto& occupied = playfield->GetOccupiedTiles();
		const int gridRows = static_cast<int>(occupied.size());
		if (gridRows == 0) return true;
		const int gridCols = static_cast<int>(occupied[0].size());

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
			const int col = horizontal ? c : fixedAxis;
			const int row = horizontal ? fixedAxis : c;

			if (row < 0 || row >= gridRows || col < 0 || col >= gridCols)
				continue;

			const bool isBorder = row == 0 || col == 0 || row == gridRows - 1 || col == gridCols - 1;
			const bool isPillar = (row % 2 == 0) && (col % 2 == 0);
			if (isBorder || isPillar || occupied[row][col])
				return false;
		}

		return true;
	}

	void EnemyMovementComponent::AddChaseTarget(GameObject* pTarget)
	{
		if (pTarget)
			m_ChaseTargets.push_back(pTarget);
	}

	GameObject* EnemyMovementComponent::FindClosestChaseTarget() const
	{
		for (auto it = m_ChaseTargets.begin(); it != m_ChaseTargets.end();)
		{
			auto* target = *it;
			if (!target || target->IsMarkedForDeletion())
			{
				it = const_cast<std::vector<GameObject*>&>(m_ChaseTargets).erase(it);
				continue;
			}
			auto* health = target->GetComponent<HealthComponent>();
			if (health && health->IsDead())
			{
				it = const_cast<std::vector<GameObject*>&>(m_ChaseTargets).erase(it);
				continue;
			}
			++it;
		}

		if (m_ChaseTargets.empty())
			return nullptr;

		const glm::vec3 myLocal = m_pTransform->GetLocalPosition();
		GameObject* closest = nullptr;
		float closestDistSq = std::numeric_limits<float>::max();

		for (auto* target : m_ChaseTargets)
		{
			auto* tx = target->GetComponent<TransformComponent>();
			if (!tx)
				continue;

			const glm::vec3 targetLocal = tx->GetLocalPosition();
			const float dx = targetLocal.x - myLocal.x;
			const float dy = targetLocal.y - myLocal.y;
			const float distSq = dx * dx + dy * dy;

			if (distSq < closestDistSq)
			{
				closestDistSq = distSq;
				closest = target;
			}
		}

		return closest;
	}
}
