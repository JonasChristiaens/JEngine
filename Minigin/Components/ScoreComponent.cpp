#include "ScoreComponent.h"
#include "GameObject.h"

dae::ScoreComponent::ScoreComponent(GameObject* pOwner, int startScore)
	: BaseComponent(pOwner)
	, m_score(startScore)
{
}

void dae::ScoreComponent::ChangeScore(int amount)
{
	m_score += amount;
	
	if (amount == 100)
	{
		NotifyObserver(Event::PlayerScoreLargeChanged, GetOwner());
	}
	else if (amount == 10)
	{
		NotifyObserver(Event::PlayerScoreSmallChanged, GetOwner());
	}
}
