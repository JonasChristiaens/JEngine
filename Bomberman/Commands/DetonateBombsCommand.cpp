#include "DetonateBombsCommand.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"

namespace dae
{
	void DetonateBombsCommand::Execute()
	{
		if (m_pGameActor == nullptr)
			return;

		Event remoteDetonateEvent(make_sdbm_hash("RemoteDetonateEvent"));
		remoteDetonateEvent.nbArgs = 0;
		EventManager::GetInstance().BroadcastEvent(remoteDetonateEvent, m_pGameActor);
	}
}
