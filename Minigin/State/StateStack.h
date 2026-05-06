#pragma once
#include <memory>
#include <vector>
#include "State.h"

namespace dae
{
    class StateStack
    {
    public:
        StateStack() = default;

        StateStack(const StateStack& other) = delete;
        StateStack(StateStack&& other) = delete;
        StateStack& operator=(const StateStack& other) = delete;
        StateStack& operator=(StateStack&& other) = delete;

        void Push(std::unique_ptr<State> state);
        void Pop();
        void Clear();

        void HandleInput();
        void Update();
        void Render() const;

        bool IsEmpty() const { return m_states.empty(); }

    private:
        std::vector<std::unique_ptr<State>> m_states{};
    };
}
