#include "DetonateBombsCommand.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"
#include "Components/HealthComponent.h"

namespace dae
{
	void DetonateBombsCommand::Execute()
	{
		if (m_pGameActor == nullptr)
			return;

		auto* health = m_pGameActor->GetComponent<HealthComponent>();
		if (health && health->IsDead())
			return;

		Event remoteDetonateEvent(make_sdbm_hash("RemoteDetonateEvent"));
		remoteDetonateEvent.nbArgs = 0;
		EventManager::GetInstance().BroadcastEvent(remoteDetonateEvent, m_pGameActor);
	}
}
