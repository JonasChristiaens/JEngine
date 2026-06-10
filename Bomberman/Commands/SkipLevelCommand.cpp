#include "SkipLevelCommand.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"
#include "Components/HealthComponent.h"

namespace dae
{
	void SkipLevelCommand::Execute()
	{
		if (m_pGameActor == nullptr)
			return;

		auto* health = m_pGameActor->GetComponent<HealthComponent>();
		if (health && health->IsDead())
			return;

		Event levelCompleteEvent(make_sdbm_hash("LevelCompleted"));
		levelCompleteEvent.nbArgs = 0;
		EventManager::GetInstance().BroadcastEvent(levelCompleteEvent, m_pGameActor);
	}
}
