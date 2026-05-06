#include "StateStackComponent.h"

 dae::StateStackComponent::StateStackComponent(GameObject* pOwner)
    : BaseComponent(pOwner)
{
}

void dae::StateStackComponent::Update()
{
    m_stateStack.Update();
}

void dae::StateStackComponent::Render() const
{
    m_stateStack.Render();
}
