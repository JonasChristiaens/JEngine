#include "RotateComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GameTime.h"
#include <cmath>
#include <numbers>

dae::RotateComponent::RotateComponent(GameObject* pOwner, float radius, float speed)
	: BaseComponent(pOwner)
	, m_Radius(radius)
	, m_Speed(speed)
{
	m_pTransform = GetOwner()->GetComponent<TransformComponent>();
}

void dae::RotateComponent::Update()
{
	const float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

	if (!m_pTransform)
		return;

	m_Angle += m_Speed * deltaTime;

	// Keep angle in [0, 2*PI] range
	constexpr float TWO_PI = 2.0f * std::numbers::pi_v<float>;
	if (m_Angle > TWO_PI)
		m_Angle -= TWO_PI;
	else if (m_Angle < 0.0f)
		m_Angle += TWO_PI;

	// Calculate circular motion in local space (relative to parent)
	float x = m_Radius * std::cos(m_Angle);
	float y = m_Radius * std::sin(m_Angle);

	m_pTransform->SetLocalPosition(x, y);
}