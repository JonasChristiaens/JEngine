#include "GameObject.h"
#include "Components/BaseComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include <algorithm>

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update()
{
	if (m_MarkedForDeletion)
		return;

	for (auto& component : m_Components)
	{
		component->Update();
	}

	// Iterate by index to avoid copying the child vector every frame
	const size_t childCount = m_Children.size();
	for (size_t i = 0; i < childCount && i < m_Children.size(); ++i)
	{
		if (m_Children[i])
		{
			m_Children[i]->Update();
		}
	}
}

void dae::GameObject::Render() const
{
	if (m_MarkedForDeletion)
		return;

	for (const auto& component : m_Components)
	{
		component->Render();
	}

	if (m_ChildrenVersion != m_LastSortedVersion)
	{
		m_SortedChildren = m_Children;
		std::stable_sort(m_SortedChildren.begin(), m_SortedChildren.end(),
			[](const GameObject* a, const GameObject* b)
			{
				const auto* ra = a->GetComponent<RenderComponent>();
				const auto* rb = b->GetComponent<RenderComponent>();
				const int la = ra ? ra->GetRenderLayer() : 0;
				const int lb = rb ? rb->GetRenderLayer() : 0;
				return la < lb;
			});
		m_LastSortedVersion = m_ChildrenVersion;
	}

	for (const auto* pChild : m_SortedChildren)
	{
		if (pChild)
		{
			pChild->Render();
		}
	}
}

void dae::GameObject::MarkForDeletion()
{
	m_MarkedForDeletion = true;

	if (m_pParent)
	{
		m_pParent->RemoveChild(this);
		m_pParent = nullptr;
	}

	// Mark children for deletion as well
	for (auto* pChild : m_Children)
	{
		pChild->MarkForDeletion();
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

	m_Children.push_back(pChild);
	++m_ChildrenVersion;
}

void dae::GameObject::RemoveChild(GameObject* pChild)
{
	if (pChild == nullptr || !IsChild(pChild))
		return;

	m_Children.erase(std::find(m_Children.begin(), m_Children.end(), pChild));
	++m_ChildrenVersion;
}

bool dae::GameObject::IsChild(GameObject* pChild) const
{
	if (pChild == nullptr)
		return false;

	if (std::find(m_Children.begin(), m_Children.end(), pChild) != m_Children.end())
		return true;

	for (const auto* child : m_Children)
	{
		if (child->IsChild(pChild))
			return true;
	}

	return false;
}

void dae::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	if (auto* pTransform = GetComponent<TransformComponent>())
	{
		pTransform->SetLocalPosition(pos);
	}
}

glm::vec3 dae::GameObject::GetLocalPosition() const
{
	if (auto* pTransform = GetComponent<TransformComponent>())
	{
		return pTransform->GetLocalPosition();
	}

	return { 0.f, 0.f, 0.f };
}

glm::vec3 dae::GameObject::GetWorldPosition() const
{
	if (auto* pTransform = GetComponent<TransformComponent>())
	{
		return pTransform->GetWorldPosition();
	}

	return { 0.f, 0.f, 0.f };
}

void dae::GameObject::SetPositionDirty()
{
	if (auto* pTransform = GetComponent<TransformComponent>())
	{
		pTransform->SetPositionDirty();
	}
}
