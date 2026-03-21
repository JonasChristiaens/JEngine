#include "ScoreComponent.h"

dae::ScoreComponent::ScoreComponent(GameObject* pOwner, int health)
	: BaseComponent(pOwner)
	, m_CurrentScore(health)
{
}

void dae::ScoreComponent::ChangeCurrentScore(int amount)
{
	m_CurrentScore += amount;
	NotifyObservers(Event(make_sdbm_hash("ScoreChanged")), GetOwner());
}
