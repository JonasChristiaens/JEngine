#pragma once
#include "EventQueue/IObserver.h"

namespace dae
{
    class Scene;

    class BombEventObserver final : public IObserver
    {
    public:
        explicit BombEventObserver(Scene& scene);
        ~BombEventObserver() override;

        void Notify(const GameObject& pGameActor, Event event) override;

    private:
        Scene* m_pScene{};
        int m_NextBombId{ 0 };
    };
}