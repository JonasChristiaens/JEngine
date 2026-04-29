#pragma once
#if USE_STEAMWORKS

#include "Command.h"

class ResetAchievementsCommand final : public Command
{
public:
	ResetAchievementsCommand() = default;
	virtual ~ResetAchievementsCommand() = default;

	void Execute() override;
};

#endif // USE_STEAMWORKS
