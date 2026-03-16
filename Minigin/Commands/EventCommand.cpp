#include "EventCommand.h"

EventCommand::EventCommand(dae::Event eventToFire)
	: m_event(eventToFire)
{
}

void EventCommand::Execute()
{
	NotifyObserver(m_event, m_pGameActor);
}