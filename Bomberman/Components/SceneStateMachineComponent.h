#pragma once
#include "BaseComponent.h"
#include "State/StateMachine.h"
#include <vector>

namespace dae
{
	class Scene;
	class GameObject;
	class StateMachineComponent;

	class SceneStateMachineComponent final : public BaseComponent
	{
	public:
		explicit SceneStateMachineComponent(GameObject* pOwner);
		~SceneStateMachineComponent() override;

		void Update() override;
		void Render() const override;

		StateMachine& GetStateMachine();

		void SetActiveScene(Scene& scene);
		Scene* GetActiveScene() const { return m_pActiveScene; }

		void RegisterPlayer(GameObject* player);
		const std::vector<GameObject*>& GetPlayers() const { return m_Players; }

		void ClearPlayers();

	private:
		StateMachineComponent* m_pStateMachineComponent{};
		Scene* m_pActiveScene{};
		std::vector<GameObject*> m_Players{};
	};
}
