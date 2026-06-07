#pragma once
#include "Command.h"

namespace dae
{
	class ChangeHealthCommand final : public Command
	{
	public:
		ChangeHealthCommand(int deltaHealth);
		virtual ~ChangeHealthCommand() = default;

		void Execute() override;

	private:
		int m_DeltaHealth;
	};
}
