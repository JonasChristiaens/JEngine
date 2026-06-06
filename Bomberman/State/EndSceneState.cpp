#include "EndSceneState.h"
#include "Components/SceneStateMachineComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

namespace dae
{
	EndSceneState::EndSceneState(SceneStateMachineComponent& owner)
		: SceneState(owner)
	{
	}

	void EndSceneState::OnEnter()
	{
		auto& scene = SceneManager::GetInstance().CreateScene();
		m_Owner.SetActiveScene(scene);
	}

	void EndSceneState::OnExit()
	{
		if (auto* scene = m_Owner.GetActiveScene())
		{
			scene->RemoveAll();
		}
	}
}
