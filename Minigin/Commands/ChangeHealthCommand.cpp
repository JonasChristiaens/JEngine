#include "ChangeHealthCommand.h"
#include "../GameObject.h"

ChangeHealthCommand::ChangeHealthCommand(int deltaHealth)
	: m_deltaHealth(deltaHealth)
{
}

void ChangeHealthCommand::Execute()
{
	if (m_pGameActor != nullptr)
	{
		auto healthComp = m_pGameActor->GetComponent<dae::HealthComponent>();
		if (healthComp != nullptr)
		{
			healthComp->ChangeCurrentHealth(m_deltaHealth);
		}
	}
}
