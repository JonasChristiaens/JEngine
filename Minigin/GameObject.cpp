#include "GameObject.h"
#include "BaseComponent.h"

dae::GameObject::~GameObject()
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(this);
	}
}

void dae::GameObject::Update(float deltaTime)
{
	for (auto& component : m_components)
	{
		component->Update(deltaTime);
	}

	// Update children
	for (auto* pChild : m_children)
	{
		pChild->Update(deltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (const auto& component : m_components)
	{
		component->Render();
	}

	// Render children
	for (const auto* pChild : m_children)
	{
		pChild->Render();
	}
}

void dae::GameObject::SetParent(GameObject* pParent, bool keepWorldPosition)
{
	// Check if the new parent is valid
	if (IsChild(pParent) || pParent == this || m_pParent == pParent)
		return;

	// Update position relative to new parent
	if (pParent == nullptr)
		SetLocalPosition(GetWorldPosition());
	else
	{
		if (keepWorldPosition)
			SetLocalPosition(GetWorldPosition() - pParent->GetWorldPosition());

		SetPositionDirty();
	}

	// Remove itself as a child from the previous parent
	if (m_pParent) m_pParent->RemoveChild(this);

	// Set the given parent on itself
	m_pParent = pParent;

	// Add itself as a child to the given parent
	if (m_pParent) m_pParent->AddChild(this);
}

// ----------------
// HELPER FUNCTIONS
// ----------------
void dae::GameObject::AddChild(GameObject* pChild)
{
	if (pChild == this || m_pParent == pChild)
		return;

	m_children.push_back(pChild);
}

void dae::GameObject::RemoveChild(GameObject* pChild)
{
	if (pChild == nullptr || !IsChild(pChild))
		return;

	m_children.erase(std::find(m_children.begin(), m_children.end(), pChild));
}

bool dae::GameObject::IsChild(GameObject* pChild) const
{
	if (pChild == nullptr)
		return false;

	return std::find(m_children.begin(), m_children.end(), pChild) != m_children.end();
}

void dae::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	m_localPosition = pos;
	SetPositionDirty();
}

const glm::vec3& dae::GameObject::GetWorldPosition() const
{
	if (m_positionIsDirty)
		UpdateWorldPosition();

	return m_worldPosition;
}

void dae::GameObject::SetPositionDirty()
{
	m_positionIsDirty = true;

	// Mark all children as dirty
	for (auto* pChild : m_children)
	{
		pChild->SetPositionDirty();
	}
}

void dae::GameObject::UpdateWorldPosition() const
{
	if (m_positionIsDirty)
	{
		if (m_pParent == nullptr)
			m_worldPosition = m_localPosition;
		else
			m_worldPosition = m_pParent->GetWorldPosition() + m_localPosition;
	}
	m_positionIsDirty = false;
}
