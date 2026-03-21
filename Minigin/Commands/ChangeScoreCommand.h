#pragma once
#include "Command.h"
#include "../Components/ScoreComponent.h"

class ChangeScoreCommand final : public Command
{
public:
	ChangeScoreCommand(int deltaScore);
	virtual ~ChangeScoreCommand() = default;

	void Execute() override;

private:
	int m_deltaScore;
};
