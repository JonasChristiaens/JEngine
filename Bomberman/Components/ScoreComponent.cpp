#include "ScoreComponent.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"

dae::ScoreComponent::ScoreComponent(GameObject* pOwner, int score)
	: BaseComponent(pOwner)
	, m_CurrentScore(score)
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

void dae::ScoreComponent::Notify(GameObject& actor, Event event)
{
	if (&actor == GetOwner() && event.id == make_sdbm_hash("ChangeScoreEvent") && event.nbArgs > 0)
	{
		ChangeCurrentScore(event.args[0].i);
	}
}
