#include "EnemyComponent.h"
#include "StateStackComponent.h"
#include "GameObject.h"

namespace dae
{
    EnemyComponent::EnemyComponent(GameObject* pOwner)
        : BaseComponent(pOwner)
    {
        m_pStateStack = pOwner->GetComponent<StateStackComponent>();
        if (!m_pStateStack)
        {
            m_pStateStack = pOwner->AddComponent<StateStackComponent>();
        }
    }
}
