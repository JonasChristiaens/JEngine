#pragma once
#include "Command.h"

namespace dae
{
	class ToggleMuteCommand final : public Command
	{
	public:
		void Execute() override;

	private:
		inline static bool m_Muted{ false };
	};
}
