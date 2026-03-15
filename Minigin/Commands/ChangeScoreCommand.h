#pragma once
#include "Command.h"
#include "GameObject.h"

class ChangeScoreCommand final : public Command
{
public:
	explicit ChangeScoreCommand(int amount);
	virtual ~ChangeScoreCommand() = default;

	void Execute() override;

private:
	int m_amount;
};