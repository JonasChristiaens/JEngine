#pragma once
#include "Command.h"

namespace dae
{
	class DetonateBombsCommand final : public Command
	{
	public:
		void Execute() override;
	};
}
