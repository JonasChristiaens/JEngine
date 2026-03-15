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
	NotifyObserver(Event::ScoreChange, GetOwner());
}
