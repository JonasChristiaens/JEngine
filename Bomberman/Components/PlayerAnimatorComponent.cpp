#include "PlayerAnimatorComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteAnimatorComponent.h"
#include "Rendering/Renderer.h"
#include <SDL3/SDL.h>
#include <cmath>

dae::PlayerAnimatorComponent::PlayerAnimatorComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::PlayerAnimatorComponent::Update()
{
	if (m_pAnimator == nullptr)
		m_pAnimator = GetOwner()->GetComponent<SpriteAnimatorComponent>();
	if (m_pTransform == nullptr)
		m_pTransform = GetOwner()->GetComponent<TransformComponent>();

	if (m_pAnimator == nullptr || m_pTransform == nullptr)
		return;

	glm::vec3 currentPos = m_pTransform->GetLocalPosition();

	if (!m_isInitialized)
	{
		m_lastPosition = currentPos;
		m_isInitialized = true;
	}

	glm::vec3 delta = currentPos - m_lastPosition;
	m_lastPosition = currentPos;

	// Calculate if we moved this frame
	if (std::abs(delta.x) > 0.01f || std::abs(delta.y) > 0.01f)
	{
		if (std::abs(delta.y) > 0.01f)
		{
			if (delta.y > 0)
				m_pAnimator->SetAnimation(3, 0, 3, 3, 10.0f, true); // Walk Down
			else
				m_pAnimator->SetAnimation(3, 1, 3, 3, 10.0f, true); // Walk Up
		}
		else
		{
			if (delta.x < 0)
				m_pAnimator->SetAnimation(0, 0, 3, 3, 10.0f, true); // Walk Left
			else
				m_pAnimator->SetAnimation(0, 1, 3, 3, 10.0f, true); // Walk Right
		}
	}
	else
	{
		// Idle frame
		m_pAnimator->SetAnimation(4, 0, 1, 1, 10.0f, true);
	}
}

void dae::PlayerAnimatorComponent::Render() const
{
	if (m_pTransform == nullptr)
		return;

	const auto& pos = m_pTransform->GetWorldPosition();
	SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
	SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255);
	SDL_FRect rect{};
	constexpr float markerSize = 6.0f;
	rect.x = pos.x - markerSize * 0.5f;
	rect.y = pos.y - markerSize * 0.5f;
	rect.w = markerSize;
	rect.h = markerSize;
	SDL_RenderFillRect(renderer, &rect);
}
