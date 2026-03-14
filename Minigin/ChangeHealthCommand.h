#pragma once
#include "Command.h"
#include "GameObject.h"
#include "HealthComponent.h"

class ChangeHealthCommand final : public Command
{
public:
	explicit ChangeHealthCommand(int amount);
	virtual ~ChangeHealthCommand() = default;

	void Execute() override;

private:
	int m_amount;
};