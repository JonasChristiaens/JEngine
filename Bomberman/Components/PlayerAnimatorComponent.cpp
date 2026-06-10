#include "PlayerAnimatorComponent.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/HealthComponent.h"
#include "SpriteAnimatorComponent.h"
#include "Rendering/Renderer.h"
#include "EventQueue/EventManager.h"
#include "Core/GameTime.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace
{
	constexpr dae::EventId kPlayAudioEventId = dae::make_sdbm_hash("PlayAudioEvent");
	constexpr const char* kFootstepHorizontalSfxPath = "step_horizontal.wav";
	constexpr const char* kFootstepVerticalSfxPath = "step_vertical.wav";
	constexpr float kFootstepIntervalSeconds = 0.25f;
}

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

	auto* health = GetOwner()->GetComponent<HealthComponent>();
	if (health && health->IsDead())
		return;

	glm::vec3 currentPos = m_pTransform->GetLocalPosition();

	if (!m_IsInitialized)
	{
		m_LastPosition = currentPos;
		m_IsInitialized = true;
	}

	glm::vec3 delta = currentPos - m_LastPosition;
	m_LastPosition = currentPos;
	const bool movedHorizontally = std::abs(delta.x) > 0.01f;
	const bool movedVertically = std::abs(delta.y) > 0.01f;
	const bool movedThisFrame = movedHorizontally || movedVertically;
	if (movedThisFrame)
	{
		m_StepElapsed += dae::GameTime::GetInstance().GetDeltaTime();
		if (m_StepElapsed >= kFootstepIntervalSeconds)
		{
			const char* sfxPath = movedVertically ? kFootstepVerticalSfxPath : kFootstepHorizontalSfxPath;
			dae::Event playAudioEvent(kPlayAudioEventId);
			playAudioEvent.nbArgs = 1;
			playAudioEvent.args[0].p = const_cast<char*>(sfxPath);
			dae::EventManager::GetInstance().BroadcastEvent(playAudioEvent, GetOwner());
			m_StepElapsed = 0.0f;
		}
	}
	else
	{
		m_StepElapsed = 0.0f;
	}

	// Calculate if we moved this frame
	if (movedThisFrame)
	{
		if (movedVertically)
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
	float cameraX = 0.0f, cameraY = 0.0f;
	Renderer::GetInstance().GetCameraOffset(cameraX, cameraY);

	SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
	SDL_SetRenderDrawColor(renderer, 0, 120, 255, 255);
	SDL_FRect rect{};
	constexpr float markerSize = 6.0f;
	rect.x = pos.x - markerSize * 0.5f + cameraX;
	rect.y = pos.y - markerSize * 0.5f + cameraY;
	rect.w = markerSize;
	rect.h = markerSize;
	SDL_RenderFillRect(renderer, &rect);
}
