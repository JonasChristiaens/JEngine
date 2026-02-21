#include "MoveComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <cmath>

dae::MoveComponent::MoveComponent(GameObject* pOwner, float radius, float speed)
	: BaseComponent(pOwner)
	, m_radius(radius)
	, m_speed(speed)
{
	m_pTransform = GetOwner()->GetComponent<TransformComponent>();
}

void dae::MoveComponent::Update(float deltaTime)
{
	if (!m_pTransform)
		return;

	m_angle += m_speed * deltaTime;

	// Keep angle in [0, 2*PI] range
	const float TWO_PI = 6.28318530718f;
	if (m_angle > TWO_PI)
		m_angle -= TWO_PI;

	// Calculate circular motion in local space
	float x = m_radius * cosf(m_angle) + m_centerX;
	float y = m_radius * sinf(m_angle) + m_centerY;

	m_pTransform->SetLocalPosition(x, y);
}