#pragma once
#include "BaseComponent.h"
#include "State/StateStack.h"

namespace dae
{
    class StateStackComponent final : public BaseComponent
    {
    public:
        StateStackComponent(GameObject* pOwner);

        void Update() override;
        void Render() const override;

        StateStack& GetStateStack() { return m_stateStack; }

    private:
        StateStack m_stateStack{};
    };
}
