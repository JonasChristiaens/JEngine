#include "CollisionComponent.h"
#include "CollisionGrid.h"
#include "../Scene/GameObject.h"
#include "TransformComponent.h"
#include "RectBounds.h"
#include <algorithm>

namespace dae
{
	CollisionComponent::CollisionComponent(GameObject* pOwner, float width, float height, bool isTrigger)
		: BaseComponent(pOwner)
		, m_Width(width)
		, m_Height(height)
		, m_IsTrigger(isTrigger)
	{}

	CollisionComponent::~CollisionComponent()
	{
		CollisionGrid::Unregister(this);
	}

	void CollisionComponent::SetOffset(const glm::vec2& offset)
	{
		m_Offset = offset;
	}

	bool CollisionComponent::WouldCollide(const glm::vec3& worldPosition) const
	{
		if (!m_Registered)
		{
			CollisionGrid::Register(this);
			m_Registered = true;
			if (auto* pTx = GetOwner()->GetComponent<TransformComponent>())
				m_LastRegisteredPos = pTx->GetWorldPosition();
		}

		const RectBounds self = RectBounds::FromOffset(worldPosition.x, worldPosition.y, m_Width, m_Height, m_Offset.x, m_Offset.y);

		std::vector<CollisionComponent*> candidates{};
		candidates.reserve(16);
		CollisionGrid::Query({ worldPosition.x + m_Offset.x, worldPosition.y + m_Offset.y, 0.0f }, m_Width, m_Height, candidates);

		for (auto* other : candidates)
		{
			if (other == this)
				continue;
			if (other->GetOwner()->IsMarkedForDeletion())
				continue;
			if (other->IsTrigger())
				continue;

			auto* otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
			if (!otherTransform)
				continue;

			const auto& otherPos = otherTransform->GetWorldPosition();
			const RectBounds otherBox = RectBounds::FromOffset(otherPos.x, otherPos.y, other->GetWidth(), other->GetHeight(), other->m_Offset.x, other->m_Offset.y);

			if (self.Overlaps(otherBox))
			{
				return true;
			}
		}

		return false;
	}

	void CollisionComponent::Update()
	{
		if (!m_Registered)
		{
			CollisionGrid::Register(this);
			m_Registered = true;
			if (auto* pTx = GetOwner()->GetComponent<TransformComponent>())
				m_LastRegisteredPos = pTx->GetWorldPosition();
		}
		else
		{
			if (auto* pTx = GetOwner()->GetComponent<TransformComponent>())
			{
				const auto& currentPos = pTx->GetWorldPosition();
				if (currentPos != m_LastRegisteredPos)
				{
					CollisionGrid::Unregister(this);
					CollisionGrid::Register(this);
					m_LastRegisteredPos = currentPos;
				}
			}
		}

		if (!m_OnCollision) return;

		if (m_pTransform == nullptr)
			m_pTransform = GetOwner()->GetComponent<TransformComponent>();
		if (!m_pTransform) return;

		const auto pos = m_pTransform->GetWorldPosition();
		const RectBounds self = RectBounds::FromOffset(pos.x, pos.y, m_Width, m_Height, m_Offset.x, m_Offset.y);

		std::vector<CollisionComponent*> candidates{};
		candidates.reserve(16);
		CollisionGrid::Query({ pos.x + m_Offset.x, pos.y + m_Offset.y, 0.0f }, m_Width, m_Height, candidates);

		for (auto* other : candidates)
		{
			if (other == this) continue;
			if (other->GetOwner()->IsMarkedForDeletion()) continue;

			auto* otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
			if (!otherTransform) continue;

			const auto& otherPos = otherTransform->GetWorldPosition();
			const RectBounds otherBox = RectBounds::FromOffset(otherPos.x, otherPos.y, other->GetWidth(), other->GetHeight(), other->m_Offset.x, other->m_Offset.y);

			if (self.Overlaps(otherBox))
			{
				m_OnCollision(other->GetOwner());
			}
		}
	}
}
