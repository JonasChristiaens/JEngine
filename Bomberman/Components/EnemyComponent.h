#pragma once
#include "BaseComponent.h"

namespace dae
{
    class StateStackComponent;

    class EnemyComponent final : public BaseComponent
    {
    public:
        explicit EnemyComponent(GameObject* pOwner);

        void Update() override {}

        StateStackComponent* GetStateStackComponent() const { return m_pStateStack; }

    private:
        StateStackComponent* m_pStateStack{};
    };
}
