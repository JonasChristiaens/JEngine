#include "StateMachineComponent.h"

 dae::StateMachineComponent::StateMachineComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::StateMachineComponent::Update()
{
	m_stateMachine.Update();
}

void dae::StateMachineComponent::Render() const
{
	m_stateMachine.Render();
}
