#include "EntityDeathObserver.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"

namespace
{
	constexpr dae::EventId kEntityDiedEventId = dae::make_sdbm_hash("EntityDied");
}

namespace dae
{
	EntityDeathObserver::EntityDeathObserver(Scene& scene)
		: m_pScene(&scene)
	{
		EventManager::GetInstance().AddObserver(*this);
	}

	EntityDeathObserver::~EntityDeathObserver()
	{
		if (EventManager::IsAlive())
		{
			EventManager::GetInstance().RemoveObserver(*this);
		}
	}

	void EntityDeathObserver::Notify(const GameObject& pGameActor, Event event)
	{
		if (event.id != kEntityDiedEventId)
			return;

		auto* actor = const_cast<GameObject*>(&pGameActor);
		if (!actor || actor->IsMarkedForDeletion())
			return;

		actor->MarkForDeletion();
	}
}
