#include "CameraComponent.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Rendering/Renderer.h"
#include <algorithm>

namespace dae
{
	CameraComponent::CameraComponent(GameObject* pOwner, GameObject* pTarget, float windowWidth, float playfieldWidth)
		: BaseComponent(pOwner)
		, m_pTarget(pTarget)
		, m_WindowWidth(windowWidth)
		, m_PlayfieldWidth(playfieldWidth)
	{}

	void CameraComponent::Update()
	{
		if (m_pTarget == nullptr)
			return;

		if (m_pTarget->IsMarkedForDeletion())
		{
			m_pTarget = nullptr;
			return;
		}

		if (m_pSecondTarget != nullptr && m_pSecondTarget->IsMarkedForDeletion())
		{
			m_pSecondTarget = nullptr;
		}

		if (m_pTransform == nullptr)
			m_pTransform = GetOwner()->GetComponent<TransformComponent>();
		if (m_pTransform == nullptr)
			return;

		const auto& targetPosition = m_pTarget->GetLocalPosition();
		const float halfWindowWidth = m_WindowWidth * 0.5f;
		const float minX = 0.0f;
		const float maxX = std::max(minX, m_PlayfieldWidth - m_WindowWidth);

		float desiredCenterX = targetPosition.x;
		if (m_pSecondTarget != nullptr)
		{
			const auto& secondPos = m_pSecondTarget->GetLocalPosition();
			const float minPlayerX = std::min(targetPosition.x, secondPos.x);
			const float maxPlayerX = std::max(targetPosition.x, secondPos.x);

			desiredCenterX = (targetPosition.x + secondPos.x) * 0.5f;

			if (desiredCenterX - halfWindowWidth > minPlayerX)
				desiredCenterX = minPlayerX + halfWindowWidth;
			if (desiredCenterX + halfWindowWidth < maxPlayerX)
				desiredCenterX = maxPlayerX - halfWindowWidth;
		}

		const float desiredX = desiredCenterX - halfWindowWidth;
		const float clampedX = std::clamp(desiredX, minX, maxX);

		m_pTransform->SetLocalPosition(-clampedX, 0.0f, 0.0f);
		Renderer::GetInstance().SetCameraOffset(-clampedX, 0.0f);
	}
}
