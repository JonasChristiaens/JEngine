#include "SpawnBombCommand.h"
#include "../EventQueue/EventManager.h"
#include "../GameObject.h"
#include "../Components/TransformComponent.h"

void SpawnBombCommand::Execute()
{
    if (m_pGameActor == nullptr)
        return;

    auto* pTransform = m_pGameActor->GetComponent<dae::TransformComponent>();
    if (pTransform == nullptr)
        return;

    const auto& pos = pTransform->GetWorldPosition();

    dae::Event placeBombEvent(dae::make_sdbm_hash("PlaceBombEvent"));
    placeBombEvent.nbArgs = 2;
    placeBombEvent.args[0].f = pos.x;
    placeBombEvent.args[1].f = pos.y;

    dae::EventManager::GetInstance().BroadcastEvent(placeBombEvent, m_pGameActor);
}