#include "TransformComponent.h"
#include "GameObject.h"

dae::TransformComponent::TransformComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{}

void dae::TransformComponent::SetLocalPosition(float x, float y, float z)
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = z;
	SetPositionDirty();
}

void dae::TransformComponent::SetLocalPosition(const glm::vec3& position)
{
	m_LocalPosition = position;
	SetPositionDirty();
}

void dae::TransformComponent::Update()
{
	UpdateWorldPosition();
}

const glm::vec3& dae::TransformComponent::GetWorldPosition() const
{
	if (m_PositionIsDirty)
		UpdateWorldPosition();

	return m_WorldPosition;
}

void dae::TransformComponent::SetPositionDirty()
{
	m_PositionIsDirty = true;

	// Mark all children as dirty
	const auto& children = GetOwner()->GetChildren();
	for (auto* pChild : children)
	{
		if (auto* pTransform = pChild->GetComponent<TransformComponent>())
		{
			pTransform->SetPositionDirty();
		}
	}
}

void dae::TransformComponent::UpdateWorldPosition() const
{
	if (m_PositionIsDirty)
	{
		GameObject* pParent = GetOwner()->GetParent();
		if (pParent == nullptr)
		{
			m_WorldPosition = m_LocalPosition;
		}
		else
		{
			if (auto* pParentTransform = pParent->GetComponent<TransformComponent>())
			{
				m_WorldPosition = pParentTransform->GetWorldPosition() + m_LocalPosition;
			}
			else
			{
				m_WorldPosition = m_LocalPosition;
			}
		}
	}

	m_PositionIsDirty = false;
}