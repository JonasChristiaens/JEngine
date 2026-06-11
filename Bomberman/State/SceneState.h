#pragma once
#include "State/State.h"
#include "Config/GameMode.h"

namespace dae
{
	class SceneStateMachineComponent;

	class SceneState : public State
	{
	public:
		explicit SceneState(SceneStateMachineComponent& owner)
			: m_Owner(owner)
		{}

		GameMode GetGameMode() const noexcept { return m_GameMode; }
		void SetGameMode(GameMode gameMode) noexcept { m_GameMode = gameMode; }

	protected:
		SceneStateMachineComponent& m_Owner;
		GameMode m_GameMode{ GameMode::Solo };
	};
}
