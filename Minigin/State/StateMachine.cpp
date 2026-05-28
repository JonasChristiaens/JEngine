#include "StateMachine.h"

void dae::StateMachine::SetState(std::unique_ptr<State> state)
{
	if (!state)
		return;

	if (m_state)
	{
		m_state->OnExit();
	}

	state->OnEnter();
	m_state = std::move(state);
}

void dae::StateMachine::Clear()
{
	if (m_state)
	{
		m_state->OnExit();
		m_state.reset();
	}
}

void dae::StateMachine::HandleInput()
{
	if (m_state)
	{
		m_state->HandleInput();
	}
}

void dae::StateMachine::Update()
{
	if (m_state)
	{
		m_state->Update();
	}
}

void dae::StateMachine::Render() const
{
	if (m_state)
	{
		m_state->Render();
	}
}
