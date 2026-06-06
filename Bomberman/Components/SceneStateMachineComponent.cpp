#include "SceneStateMachineComponent.h"
#include "Scene/Scene.h"

namespace dae
{
	SceneStateMachineComponent::SceneStateMachineComponent(GameObject* pOwner)
		: BaseComponent(pOwner)
	{
	}

	SceneStateMachineComponent::~SceneStateMachineComponent()
	{
		if (m_pActiveScene)
		{
			m_pActiveScene->RemoveAll();
		}
	}

	void SceneStateMachineComponent::Update()
	{
		m_stateMachine.HandleInput();
		m_stateMachine.Update();
	}

	void SceneStateMachineComponent::Render() const
	{
		m_stateMachine.Render();
	}

	void SceneStateMachineComponent::SetActiveScene(Scene& scene)
	{
		if (m_pActiveScene)
		{
			m_pActiveScene->RemoveAll();
		}
		m_pActiveScene = &scene;
	}

	void SceneStateMachineComponent::RegisterPlayer(GameObject* player)
	{
		if (player)
		{
			m_players.push_back(player);
		}
	}

	void SceneStateMachineComponent::ClearPlayers()
	{
		m_players.clear();
	}
}
