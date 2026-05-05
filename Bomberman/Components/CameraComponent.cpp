#include "CameraComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <algorithm>

namespace dae
{
	CameraComponent::CameraComponent(GameObject* pOwner, GameObject* pTarget, float windowWidth, float playfieldWidth)
		: BaseComponent(pOwner)
		, m_pTarget(pTarget)
		, m_windowWidth(windowWidth)
		, m_playfieldWidth(playfieldWidth)
	{}

	void CameraComponent::Update()
	{
		if (m_pTarget == nullptr)
			return;

		auto* cameraTransform = GetOwner()->GetComponent<TransformComponent>();
		if (cameraTransform == nullptr)
			return;

		const auto& targetPosition = m_pTarget->GetLocalPosition();
		const float halfWindowWidth = m_windowWidth * 0.5f;
		const float minX = 0.0f;
		const float maxX = std::max(minX, m_playfieldWidth - m_windowWidth);
		const float desiredX = targetPosition.x - halfWindowWidth;
		const float clampedX = std::clamp(desiredX, minX, maxX);

		cameraTransform->SetLocalPosition(-clampedX, 0.0f, 0.0f);
	}
}
