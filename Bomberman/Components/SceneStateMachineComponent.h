#pragma once
#include "BaseComponent.h"
#include "State/StateMachine.h"
#include <memory>
#include <vector>

namespace dae
{
	class Scene;
	class GameObject;

	class SceneStateMachineComponent final : public BaseComponent
	{
	public:
		explicit SceneStateMachineComponent(GameObject* pOwner);
		~SceneStateMachineComponent() override;

		void Update() override;
		void Render() const override;

		StateMachine& GetStateMachine() { return m_stateMachine; }

		void SetActiveScene(Scene& scene);
		Scene* GetActiveScene() const { return m_pActiveScene; }

		void RegisterPlayer(GameObject* player);
		const std::vector<GameObject*>& GetPlayers() const { return m_players; }

		void ClearPlayers();

	private:
		StateMachine m_stateMachine{};
		Scene* m_pActiveScene{};
		std::vector<GameObject*> m_players{};
	};
}
