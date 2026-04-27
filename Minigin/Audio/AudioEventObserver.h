#pragma once
#include "EventQueue/IObserver.h"

namespace dae
{
    class AudioEventObserver final : public IObserver
    {
    public:
        AudioEventObserver();
        ~AudioEventObserver() override;

        void Notify(const GameObject& pGameActor, Event event) override;
    };
}
