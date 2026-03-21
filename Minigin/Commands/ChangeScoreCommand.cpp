#include "ChangeScoreCommand.h"
#include "../GameObject.h"

ChangeScoreCommand::ChangeScoreCommand(int deltaScore)
	: m_deltaScore(deltaScore)
{
}

void ChangeScoreCommand::Execute()
{
	if (m_pGameActor != nullptr)
	{
		auto scoreComp = m_pGameActor->GetComponent<dae::ScoreComponent>();
		if (scoreComp != nullptr)
		{
			scoreComp->ChangeCurrentScore(m_deltaScore);
		}
	}
}
