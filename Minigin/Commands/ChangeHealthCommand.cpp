#include "ChangeHealthCommand.h"
#include "Components/HealthComponent.h"

ChangeHealthCommand::ChangeHealthCommand(int amount)
	: m_amount(amount)
{
}

void ChangeHealthCommand::Execute()
{
	if (m_pGameActor)
	{
		if (auto pHealth = m_pGameActor->GetComponent<dae::HealthComponent>())
		{
			pHealth->ChangeHealth(m_amount);
		}
	}
}