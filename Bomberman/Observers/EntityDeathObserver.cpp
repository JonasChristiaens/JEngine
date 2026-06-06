#include "EntityDeathObserver.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"

namespace
{
	constexpr dae::EventId kEntityDiedEventId = dae::make_sdbm_hash("EntityDied");
}

namespace dae
{
	EntityDeathObserver::EntityDeathObserver(Scene& scene)
		: IObserver()
		, m_pScene(&scene)
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

	void EntityDeathObserver::Notify(GameObject& actor, Event event)
	{
		if (event.id != kEntityDiedEventId)
			return;

		if (m_pScene == nullptr)
			return;

		if (!m_pScene->Contains(&actor) || actor.IsMarkedForDeletion())
			return;

		actor.MarkForDeletion();
	}
}
