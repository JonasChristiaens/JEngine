#include "HealthComponent.h"

namespace dae
{
	HealthComponent::HealthComponent(GameObject* pOwner, int lives)
		: BaseComponent(pOwner)
		, m_lives(lives)
	{
	}

	void HealthComponent::ChangeHealth(int amount)
	{
		m_lives += amount;
	}
}