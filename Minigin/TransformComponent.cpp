#include "TransformComponent.h"
#include "GameObject.h"

dae::TransformComponent::TransformComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::TransformComponent::SetLocalPosition(float x, float y, float z)
{
	m_localPosition.x = x;
	m_localPosition.y = y;
	m_localPosition.z = z;
	SetPositionDirty();
}

void dae::TransformComponent::SetLocalPosition(const glm::vec3& position)
{
	m_localPosition = position;
	SetPositionDirty();
}

const glm::vec3& dae::TransformComponent::GetWorldPosition() const
{
	if (m_positionIsDirty)
		UpdateWorldPosition();

	return m_worldPosition;
}

void dae::TransformComponent::SetPositionDirty()
{
	m_positionIsDirty = true;

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
	if (m_positionIsDirty)
	{
		GameObject* pParent = GetOwner()->GetParent();
		if (pParent == nullptr)
		{
			m_worldPosition = m_localPosition;
		}
		else
		{
			if (auto* pParentTransform = pParent->GetComponent<TransformComponent>())
			{
				m_worldPosition = pParentTransform->GetWorldPosition() + m_localPosition;
			}
			else
			{
				m_worldPosition = m_localPosition;
			}
		}
	}
	m_positionIsDirty = false;
}