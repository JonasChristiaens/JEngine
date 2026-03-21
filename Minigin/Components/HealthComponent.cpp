#include "HealthComponent.h"

dae::HealthComponent::HealthComponent(GameObject* pOwner, int health)
	: BaseComponent(pOwner)
	, m_CurrentHealth(health)
{
}

void dae::HealthComponent::ChangeCurrentHealth(int amount)
{
	m_CurrentHealth += amount;
	NotifyObservers(Event(make_sdbm_hash("HealthChanged")), GetOwner());
}