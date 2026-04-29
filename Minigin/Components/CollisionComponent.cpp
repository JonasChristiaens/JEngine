#include "CollisionComponent.h"
#include "../Scene/GameObject.h"
#include "TransformComponent.h"

namespace dae
{
	std::vector<CollisionComponent*> CollisionComponent::m_allColliders{};

	CollisionComponent::CollisionComponent(GameObject* pOwner, float width, float height)
		: BaseComponent(pOwner), m_width(width), m_height(height)
	{
		m_allColliders.push_back(this);
	}

	CollisionComponent::~CollisionComponent()
	{
		m_allColliders.erase(std::remove(m_allColliders.begin(), m_allColliders.end(), this), m_allColliders.end());
	}

	void CollisionComponent::Update()
	{
		if (!m_onCollision) return;

		auto transform = GetOwner()->GetComponent<TransformComponent>();
		if (!transform) return;

		const auto pos = transform->GetWorldPosition();
		const float left1 = pos.x;
		const float right1 = pos.x + m_width;
		const float top1 = pos.y;
		const float bottom1 = pos.y + m_height;

		for (auto* other : m_allColliders)
		{
			if (other == this) continue;
			if (other->GetOwner()->IsMarkedForDeletion()) continue;

			auto otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
			if (!otherTransform) continue;

			const auto otherPos = otherTransform->GetWorldPosition();
			const float left2 = otherPos.x;
			const float right2 = otherPos.x + other->GetWidth();
			const float top2 = otherPos.y;
			const float bottom2 = otherPos.y + other->GetHeight();

			if (left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2)
			{
				m_onCollision(other->GetOwner());
			}
		}
	}
}
