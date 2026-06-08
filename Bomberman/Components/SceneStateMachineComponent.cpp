#include "SceneStateMachineComponent.h"
#include "StateMachineComponent.h"
#include "Scene/Scene.h"
#include "State/StateMachine.h"

namespace dae
{
	SceneStateMachineComponent::SceneStateMachineComponent(GameObject* pOwner)
		: BaseComponent(pOwner)
	{
		m_pStateMachineComponent = pOwner->GetComponent<StateMachineComponent>();
		if (!m_pStateMachineComponent)
		{
			m_pStateMachineComponent = pOwner->AddComponent<StateMachineComponent>();
		}
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
		m_pStateMachineComponent->GetStateMachine().HandleInput();
		m_pStateMachineComponent->Update();
	}

	void SceneStateMachineComponent::Render() const
	{
		m_pStateMachineComponent->Render();
	}

	StateMachine& SceneStateMachineComponent::GetStateMachine()
	{
		return m_pStateMachineComponent->GetStateMachine();
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
			m_Players.push_back(player);
		}
	}

	void SceneStateMachineComponent::ClearPlayers()
	{
		m_Players.clear();
	}
}
