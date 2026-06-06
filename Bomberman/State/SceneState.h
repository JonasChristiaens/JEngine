#pragma once
#include "State/State.h"
#include "GameMode.h"

namespace dae
{
	class SceneStateMachineComponent;

	class SceneState : public State
	{
	public:
		explicit SceneState(SceneStateMachineComponent& owner)
			: m_Owner(owner)
		{
		}

		GameMode GetGameMode() const { return m_GameMode; }
		void SetGameMode(GameMode gameMode) { m_GameMode = gameMode; }

	protected:
		SceneStateMachineComponent& m_Owner;
		GameMode m_GameMode{ GameMode::Solo };
	};
}
