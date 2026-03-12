#include "MoveComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "GameTime.h"
#include <cmath>
#include <numbers>

dae::MoveComponent::MoveComponent(GameObject* pOwner, float radius, float speed)
	: BaseComponent(pOwner)
	, m_radius(radius)
	, m_speed(speed)
{
	m_pTransform = GetOwner()->GetComponent<TransformComponent>();
}

void dae::MoveComponent::Update()
{
	float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

	if (!m_pTransform)
		return;

	m_angle += m_speed * deltaTime;

	// Keep angle in [0, 2*PI] range
	constexpr float TWO_PI = 2.0f * std::numbers::pi_v<float>;
	if (m_angle > TWO_PI)
		m_angle -= TWO_PI;
	else if (m_angle < 0.0f)
		m_angle += TWO_PI;

	// Calculate circular motion in local space (relative to parent)
	float x = m_radius * std::cos(m_angle);
	float y = m_radius * std::sin(m_angle);

	m_pTransform->SetLocalPosition(x, y);
}