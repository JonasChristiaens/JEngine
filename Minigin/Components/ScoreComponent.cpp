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

	if (m_CurrentScore >= 500 && !m_WonTriggered)
	{
		m_WonTriggered = true;
		EventManager::GetInstance().BroadcastEvent(Event(make_sdbm_hash("PlayerWon")), GetOwner());
	}
}

void dae::ScoreComponent::Notify(const GameObject& pGameActor, Event event)
{
	if (&pGameActor == GetOwner())
	{
		if (event.id == make_sdbm_hash("ChangeScoreEvent"))
		{
			if (event.nbArgs > 0)
			{
				ChangeCurrentScore(event.args[0].i);
			}
		}
	}

	if (event.id == make_sdbm_hash("ResetAchievements"))
	{
		m_WonTriggered = false;
	}
}
