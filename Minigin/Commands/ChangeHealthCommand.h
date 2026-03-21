#pragma once
#include "Command.h"
#include "../Components/HealthComponent.h"

class ChangeHealthCommand final : public Command
{
public:
	ChangeHealthCommand(int deltaHealth);
	virtual ~ChangeHealthCommand() = default;

	void Execute() override;

private:
	int m_deltaHealth;
};
