#pragma once
#include <memory>
#include "State.h"

namespace dae
{
	class StateMachine
	{
	public:
		StateMachine() = default;

		StateMachine(const StateMachine& other) = delete;
		StateMachine(StateMachine&& other) = delete;
		StateMachine& operator=(const StateMachine& other) = delete;
		StateMachine& operator=(StateMachine&& other) = delete;

		void SetState(std::unique_ptr<State> state);
		void Clear();

		void HandleInput();
		void Update();
		void Render() const;

		bool IsEmpty() const { return !m_State; }

	private:
		std::unique_ptr<State> m_State{};
	};
}
