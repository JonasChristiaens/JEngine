#include "GameObject.h"
#include "BaseComponent.h"

void dae::GameObject::Update(float deltaTime)
{
	for (auto& component : m_components)
	{
		component->Update(deltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (const auto& component : m_components)
	{
		component->Render();
	}
}
