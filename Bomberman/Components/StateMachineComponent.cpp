#include "StateMachineComponent.h"

dae::StateMachineComponent::StateMachineComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{}

void dae::StateMachineComponent::Update()
{
	m_StateMachine.Update();
}

void dae::StateMachineComponent::Render() const
{
	m_StateMachine.Render();
}
