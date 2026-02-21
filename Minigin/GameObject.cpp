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

void dae::GameObject::MarkForDeletion()
{
	m_markedForDeletion = true;
}

bool dae::GameObject::IsMarkedForDeletion() const
{
	return m_markedForDeletion;
}

// Hierarchy management
void dae::GameObject::SetParent(GameObject* pParent, bool /*keepWorldTransform*/)
{
	// Check if the new parent is valid
	if (IsChild(pParent) || pParent == this || m_pParent == pParent)
		return;

	// Remove itself as a child from the previous parent
	if (m_pParent) m_pParent->RemoveChild(this);

	// Set the given parent on itself
	m_pParent = pParent;

	// Add itself as a child to the given parent
	if (m_pParent) m_pParent->AddChild(this);

	// Update position relative to new parent
	// TODO: Implement with dirty flag pattern in future step
}

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
