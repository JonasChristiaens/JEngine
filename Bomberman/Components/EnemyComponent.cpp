#include "EnemyComponent.h"
#include "StateMachineComponent.h"
#include "GameObject.h"

namespace dae
{
	EnemyComponent::EnemyComponent(GameObject* pOwner)
		: BaseComponent(pOwner)
	{
		m_pStateMachine = pOwner->GetComponent<StateMachineComponent>();
		if (!m_pStateMachine)
		{
			m_pStateMachine = pOwner->AddComponent<StateMachineComponent>();
		}
	}
}
