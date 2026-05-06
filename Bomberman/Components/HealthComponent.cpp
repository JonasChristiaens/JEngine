#include "HealthComponent.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"

dae::HealthComponent::HealthComponent(GameObject* pOwner, int health)
	: BaseComponent(pOwner)
	, m_CurrentHealth(health)
{
	EventManager::GetInstance().AddObserver(*this);
}

dae::HealthComponent::~HealthComponent()
{
	EventManager::GetInstance().RemoveObserver(*this);
}

void dae::HealthComponent::ChangeCurrentHealth(int amount)
{
	m_CurrentHealth += amount;
	NotifyObservers(Event(make_sdbm_hash("HealthChanged")), GetOwner());

	if (m_CurrentHealth <= 0)
	{
      Event diedEvent(make_sdbm_hash("EntityDied"));
		EventManager::GetInstance().BroadcastEvent(diedEvent, GetOwner());
	}

	if (amount < 0)
	{
		Event tookDamageEvent(make_sdbm_hash("TookDamageEvent"));
		tookDamageEvent.nbArgs = 1;
		tookDamageEvent.args[0].i = -amount;
		EventManager::GetInstance().BroadcastEvent(tookDamageEvent, GetOwner());
	}
}

void dae::HealthComponent::Notify(const GameObject& pGameActor, Event event)
{
	if (&pGameActor == GetOwner())
	{
		if (event.id == make_sdbm_hash("ChangeHealthEvent"))
		{
			if (event.nbArgs > 0)
			{
				ChangeCurrentHealth(event.args[0].i);
			}
		}
	}
}