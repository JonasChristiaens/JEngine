#include "ChangeScoreCommand.h"
#include "../EventQueue/EventManager.h"
#include "../GameObject.h"

ChangeScoreCommand::ChangeScoreCommand(int deltaScore)
	: m_deltaScore(deltaScore)
{
}

void ChangeScoreCommand::Execute()
{
	if (m_pGameActor != nullptr)
	{
		dae::Event scoreEvent(dae::make_sdbm_hash("ChangeScoreEvent"));
		scoreEvent.nbArgs = 1;
		scoreEvent.args[0].i = m_deltaScore;
		dae::EventManager::GetInstance().BroadcastEvent(scoreEvent, m_pGameActor);
	}
}
