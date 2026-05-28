#pragma once
#include "BaseComponent.h"
#include "State/StateMachine.h"

namespace dae
{
	class StateMachineComponent final : public BaseComponent
	{
	public:
		StateMachineComponent(GameObject* pOwner);

		void Update() override;
		void Render() const override;

		StateMachine& GetStateMachine() { return m_stateMachine; }

	private:
		StateMachine m_stateMachine{};
	};
}
