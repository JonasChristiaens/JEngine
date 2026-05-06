#include "StateStack.h"

void dae::StateStack::Push(std::unique_ptr<State> state)
{
    if (!state)
        return;

    if (!m_states.empty())
    {
        m_states.back()->OnSuspend();
    }

    state->OnEnter();
    m_states.emplace_back(std::move(state));
}

void dae::StateStack::Pop()
{
    if (m_states.empty())
        return;

    m_states.back()->OnExit();
    m_states.pop_back();

    if (!m_states.empty())
    {
        m_states.back()->OnResume();
    }
}

void dae::StateStack::Clear()
{
    while (!m_states.empty())
    {
        m_states.back()->OnExit();
        m_states.pop_back();
    }
}

void dae::StateStack::HandleInput()
{
    for (auto& state : m_states)
    {
        if (state)
        {
            state->HandleInput();
        }
    }
}

void dae::StateStack::Update()
{
    for (auto& state : m_states)
    {
        if (state)
        {
            state->Update();
        }
    }
}

void dae::StateStack::Render() const
{
    for (const auto& state : m_states)
    {
        if (state)
        {
            state->Render();
        }
    }
}
