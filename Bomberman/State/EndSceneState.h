#pragma once
#include "SceneState.h"

namespace dae
{
	class EndSceneState final : public SceneState
	{
	public:
		explicit EndSceneState(SceneStateMachineComponent& owner);

		void OnEnter() override;
		void OnExit() override;
	};
}
