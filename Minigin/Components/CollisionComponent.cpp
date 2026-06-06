#include "CollisionComponent.h"
#include "../Scene/GameObject.h"
#include "TransformComponent.h"
#include <algorithm>
#include "Renderer.h"

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
		const float left1 = worldPosition.x + m_Offset.x;
		const float right1 = left1 + m_Width;
		const float top1 = worldPosition.y + m_Offset.y;
		const float bottom1 = top1 + m_Height;

		for (auto* other : m_AllColliders)
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
			const float left2 = otherPos.x + other->m_Offset.x;
			const float right2 = left2 + other->GetWidth();
			const float top2 = otherPos.y + other->m_Offset.y;
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
		float cameraX = 0.0f, cameraY = 0.0f;
		Renderer::GetInstance().GetCameraOffset(cameraX, cameraY);

		SDL_FRect rect{};
		rect.x = pos.x + m_Offset.x + cameraX;
		rect.y = pos.y + m_Offset.y + cameraY;
		rect.w = m_Width;
		rect.h = m_Height;

		SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderRect(renderer, &rect);
	}

	void CollisionComponent::Update()
	{
		if (!m_OnCollision) return;

		auto transform = GetOwner()->GetComponent<TransformComponent>();
		if (!transform) return;

		const auto pos = transform->GetWorldPosition();
		const float left1 = pos.x + m_Offset.x;
		const float right1 = left1 + m_Width;
		const float top1 = pos.y + m_Offset.y;
		const float bottom1 = top1 + m_Height;

		for (auto* other : m_AllColliders)
		{
			if (other == this) continue;
			if (other->GetOwner()->IsMarkedForDeletion()) continue;

			auto otherTransform = other->GetOwner()->GetComponent<TransformComponent>();
			if (!otherTransform) continue;

			const auto otherPos = otherTransform->GetWorldPosition();
			const float left2 = otherPos.x + other->m_Offset.x;
			const float right2 = left2 + other->GetWidth();
			const float top2 = otherPos.y + other->m_Offset.y;
			const float bottom2 = top2 + other->GetHeight();

			if (left1 < right2 && right1 > left2 && top1 < bottom2 && bottom1 > top2)
			{
				m_OnCollision(other->GetOwner());
			}
		}
	}
}
