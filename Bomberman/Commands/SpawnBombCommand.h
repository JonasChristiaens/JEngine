#pragma once
#include "Command.h"

namespace dae
{
	class SpawnBombCommand final : public Command
	{
	public:
		void Execute() override;
	};
}