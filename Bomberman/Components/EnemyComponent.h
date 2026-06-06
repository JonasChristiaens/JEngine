#pragma once
#include "BaseComponent.h"

namespace dae
{
	class StateMachineComponent;

	class EnemyComponent final : public BaseComponent
	{
	public:
		explicit EnemyComponent(GameObject* pOwner);

		void Update() override {}

		StateMachineComponent* GetStateMachineComponent() const { return m_pStateMachine; }

	private:
		StateMachineComponent* m_pStateMachine{};
	};
}
