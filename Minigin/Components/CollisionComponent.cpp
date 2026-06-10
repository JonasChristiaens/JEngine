#include "CollisionComponent.h"
#include "../Scene/GameObject.h"
#include "TransformComponent.h"
#include "RectBounds.h"
#include <algorithm>

namespace dae
{
	std::vector<CollisionComponent*> CollisionComponent::m_AllColliders{};

	CollisionComponent::CollisionComponent(GameObject* pOwner, float width, float height, bool isTrigger)
		: BaseComponent(pOwner)
		, m_Width(width)
		, m_Height(height)
		, m_IsTrigger(isTrigger)
	{
		m_AllColliders.push_back(this);
	}

	CollisionComponent::~CollisionComponent()
	{
		m_AllColliders.erase(std::remove(m_AllColliders.begin(), m_AllColliders.end(), this), m_AllColliders.end());
	}

	void CollisionComponent::SetOffset(const glm::vec2& offset)
	{
		m_Offset = offset;
	}

	bool CollisionComponent::WouldCollide(const glm::vec3& worldPosition) const
	{
		const RectBounds self = RectBounds::FromOffset(worldPosition.x, worldPosition.y, m_Width, m_Height, m_Offset.x, m_Offset.y);

		for (auto* other : m_AllColliders)
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
		if (!m_OnCollision) return;

		if (m_pTransform == nullptr)
			m_pTransform = GetOwner()->GetComponent<TransformComponent>();
		if (!m_pTransform) return;

		const auto pos = m_pTransform->GetWorldPosition();
		const RectBounds self = RectBounds::FromOffset(pos.x, pos.y, m_Width, m_Height, m_Offset.x, m_Offset.y);

		for (auto* other : m_AllColliders)
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
