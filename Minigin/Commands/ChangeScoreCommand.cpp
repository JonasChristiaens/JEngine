#include "ChangeScoreCommand.h"
#include "Components/ScoreComponent.h"

ChangeScoreCommand::ChangeScoreCommand(int amount)
	: m_amount(amount)
{
}

void ChangeScoreCommand::Execute()
{
	if (m_pGameActor)
	{
		if (auto pScore = m_pGameActor->GetComponent<dae::ScoreComponent>())
		{
			pScore->ChangeScore(m_amount);
		}
	}
}