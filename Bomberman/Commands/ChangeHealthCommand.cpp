#include "ChangeHealthCommand.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"

namespace dae
{
	ChangeHealthCommand::ChangeHealthCommand(int deltaHealth)
		: m_DeltaHealth(deltaHealth)
	{
	}

	void ChangeHealthCommand::Execute()
	{
		if (m_pGameActor != nullptr)
		{
			dae::Event healthEvent(dae::make_sdbm_hash("ChangeHealthEvent"));
			healthEvent.nbArgs = 1;
			healthEvent.args[0].i = m_DeltaHealth;
			dae::EventManager::GetInstance().BroadcastEvent(healthEvent, m_pGameActor);
		}
	}
}
