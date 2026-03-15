#include "HealthComponent.h"
#include "GameObject.h"

dae::HealthComponent::HealthComponent(GameObject* pOwner, int lives)
	: BaseComponent(pOwner)
	, m_lives(lives)
{
}

void dae::HealthComponent::ChangeHealth(int amount)
{
	m_lives += amount;
	NotifyObserver(Event::HealthChange, GetOwner());
}