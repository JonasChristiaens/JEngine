#pragma once
#include "State/State.h"

namespace dae
{
	class SceneStateMachineComponent;

	class SceneState : public State
	{
	public:
		explicit SceneState(SceneStateMachineComponent& owner)
			: m_owner(owner)
		{
		}

	protected:
		SceneStateMachineComponent& m_owner;
	};
}
