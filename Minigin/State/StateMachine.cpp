#include "StateMachine.h"

void dae::StateMachine::SetState(std::unique_ptr<State> state)
{
	if (!state)
		return;

	if (m_State)
	{
		m_State->OnExit();
	}

	state->OnEnter();
	m_State = std::move(state);
}

void dae::StateMachine::Clear()
{
	if (m_State)
	{
		m_State->OnExit();
		m_State.reset();
	}
}

void dae::StateMachine::HandleInput()
{
	if (m_State)
	{
		m_State->HandleInput();
	}
}

void dae::StateMachine::Update()
{
	if (m_State)
	{
		m_State->Update();
	}
}

void dae::StateMachine::Render() const
{
	if (m_State)
	{
		m_State->Render();
	}
}
