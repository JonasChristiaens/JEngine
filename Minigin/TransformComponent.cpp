#include "TransformComponent.h"

dae::TransformComponent::TransformComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::TransformComponent::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void dae::TransformComponent::SetPosition(const glm::vec3& position)
{
	m_position = position;
}