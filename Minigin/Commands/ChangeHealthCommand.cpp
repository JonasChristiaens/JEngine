#include "ChangeHealthCommand.h"
#include "../EventQueue/EventManager.h"
#include "../GameObject.h"

ChangeHealthCommand::ChangeHealthCommand(int deltaHealth)
	: m_deltaHealth(deltaHealth)
{
}

void ChangeHealthCommand::Execute()
{
	if (m_pGameActor != nullptr)
	{
		dae::Event healthEvent(dae::make_sdbm_hash("ChangeHealthEvent"));
		healthEvent.nbArgs = 1;
		healthEvent.args[0].i = m_deltaHealth;
		dae::EventManager::GetInstance().BroadcastEvent(healthEvent, m_pGameActor);
	}
}
