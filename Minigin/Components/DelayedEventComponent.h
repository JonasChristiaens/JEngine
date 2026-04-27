#pragma once
#include "BaseComponent.h"
#include "../EventQueue/Event.h"

namespace dae
{
    class DelayedEventComponent final : public BaseComponent
    {
    public:
        DelayedEventComponent(GameObject* pOwner, const Event& eventToSend, float delaySeconds);

        void Update() override;
        void Render() const override {}

    private:
        Event m_EventToSend;
        float m_DelaySeconds{};
        float m_Elapsed{};
        bool m_HasSent{};
    };
}