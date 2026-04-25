#include "ScoreComponent.h"
#include "../EventQueue/EventManager.h"
#include "../GameObject.h"

dae::ScoreComponent::ScoreComponent(GameObject* pOwner, int health)
	: BaseComponent(pOwner)
	, m_CurrentScore(health)
{
	EventManager::GetInstance().AddObserver(*this);
}

dae::ScoreComponent::~ScoreComponent()
{
	EventManager::GetInstance().RemoveObserver(*this);
}

void dae::ScoreComponent::ChangeCurrentScore(int amount)
{
	m_CurrentScore += amount;
	NotifyObservers(Event(make_sdbm_hash("ScoreChanged")), GetOwner());
}

void dae::ScoreComponent::Notify(const GameObject& pGameActor, Event event)
{
	if (&pGameActor == GetOwner() && event.id == make_sdbm_hash("ChangeScoreEvent") && event.nbArgs > 0)
	{
		ChangeCurrentScore(event.args[0].i);
	}
}
