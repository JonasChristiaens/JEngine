#include "CollisionComponent.h"
#include "../Scene/GameObject.h"
#include "TransformComponent.h"
#include <algorithm>
#include "Renderer.h"

namespace dae
{
	std::vector<CollisionComponent*> CollisionComponent::m_allColliders{};

	CollisionComponent::CollisionComponent(GameObject* pOwner, float width, float height, bool isTrigger)
		: BaseComponent(pOwner)
		, m_width(width)
		, m_height(height)
		, m_isTrigger(isTrigger)
	{
		m_allColliders.push_back(this);
	}

	CollisionComponent::~CollisionComponent()
	{
		m_allColliders.erase(std::remove(m_allColliders.begin(), m_allColliders.end(), this), m_allColliders.end());
	}

	void CollisionComponent::SetOffset(const glm::vec2& offset)
	{
		m_offset = offset;
	}

	bool CollisionComponent::WouldCollide(const glm::vec3& worldPosition) const
	{
		const float left1 = worldPosition.x + m_offset.x;
		const float right1 = left1 + m_width;
		const float top1 = worldPosition.y + m_offset.y;
		const float bottom1 = top1 + m_height;

		for (auto* other : m_allColliders)
		{
			if (other == this)
				continue;
			if (other->GetOwner()->IsMarkedForDeletion())
				continue;
			if (other->IsTrigger())
				continue;

			auto otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
			if (!otherTransform)
				continue;

			const auto otherPos = otherTransform->GetWorldPosition();
			const float left2 = otherPos.x + other->m_offset.x;
			const float right2 = left2 + other->GetWidth();
			const float top2 = otherPos.y + other->m_offset.y;
			const float bottom2 = top2 + other->GetHeight();

			if (left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2)
			{
				return true;
			}
		}

		return false;
	}

	void CollisionComponent::Render() const
	{
		auto transform = GetOwner()->GetComponent<TransformComponent>();
		if (!transform)
			return;

		const auto pos = transform->GetWorldPosition();
		SDL_FRect rect{};
		rect.x = pos.x + m_offset.x;
		rect.y = pos.y + m_offset.y;
		rect.w = m_width;
		rect.h = m_height;

		SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderRect(renderer, &rect);
	}

	void CollisionComponent::Update()
	{
		if (!m_onCollision) return;

		auto transform = GetOwner()->GetComponent<TransformComponent>();
		if (!transform) return;

		const auto pos = transform->GetWorldPosition();
		const float left1 = pos.x + m_offset.x;
		const float right1 = left1 + m_width;
		const float top1 = pos.y + m_offset.y;
		const float bottom1 = top1 + m_height;

		for (auto* other : m_allColliders)
		{
			if (other == this) continue;
			if (other->GetOwner()->IsMarkedForDeletion()) continue;

			auto otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
			if (!otherTransform) continue;

			const auto otherPos = otherTransform->GetWorldPosition();
			const float left2 = otherPos.x + other->m_offset.x;
			const float right2 = left2 + other->GetWidth();
			const float top2 = otherPos.y + other->m_offset.y;
			const float bottom2 = top2 + other->GetHeight();

			if (left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2)
			{
				m_onCollision(other->GetOwner());
			}
		}
	}
}
