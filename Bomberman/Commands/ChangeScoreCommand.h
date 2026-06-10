#pragma once
#include "Command.h"

namespace dae
{
	class ChangeScoreCommand final : public Command
	{
	public:
		explicit ChangeScoreCommand(int deltaScore);
		virtual ~ChangeScoreCommand() = default;

		void Execute() override;

	private:
		int m_DeltaScore;
	};
}
