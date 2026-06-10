#include "KeyboardInput.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <vector>

namespace
{
	SDL_Scancode KeyCodeToScancode(dae::KeyCode key)
	{
		switch (key)
		{
		case dae::KeyCode::W:      return SDL_SCANCODE_W;
		case dae::KeyCode::A:      return SDL_SCANCODE_A;
		case dae::KeyCode::S:      return SDL_SCANCODE_S;
		case dae::KeyCode::D:      return SDL_SCANCODE_D;
		case dae::KeyCode::R:      return SDL_SCANCODE_R;
		case dae::KeyCode::B:      return SDL_SCANCODE_B;
		case dae::KeyCode::Up:     return SDL_SCANCODE_UP;
		case dae::KeyCode::Down:   return SDL_SCANCODE_DOWN;
		case dae::KeyCode::Return: return SDL_SCANCODE_RETURN;
		case dae::KeyCode::F1:     return SDL_SCANCODE_F1;
		default:                   return SDL_SCANCODE_UNKNOWN;
		}
	}
}

namespace dae
{
	class KeyboardInput::KeyboardInputImpl
	{
	public:
		void Update()
		{
			int numKeys = 0;
			const bool* state = SDL_GetKeyboardState(&numKeys);
			if (!state || numKeys <= 0)
				return;

			m_previousState = m_currentState;
			if (m_previousState.empty())
			{
				m_previousState.assign(state, state + numKeys);
			}
			m_currentState.assign(state, state + numKeys);
		}

		bool IsDownThisFrame(KeyCode key) const
		{
			const SDL_Scancode sc = KeyCodeToScancode(key);
			if (sc == SDL_SCANCODE_UNKNOWN || sc >= static_cast<int>(m_currentState.size()))
				return false;
			return m_currentState[sc] && !m_previousState[sc];
		}

		bool IsUpThisFrame(KeyCode key) const
		{
			const SDL_Scancode sc = KeyCodeToScancode(key);
			if (sc == SDL_SCANCODE_UNKNOWN || sc >= static_cast<int>(m_currentState.size()))
				return false;
			return !m_currentState[sc] && m_previousState[sc];
		}

		bool IsPressed(KeyCode key) const
		{
			const SDL_Scancode sc = KeyCodeToScancode(key);
			if (sc == SDL_SCANCODE_UNKNOWN || sc >= static_cast<int>(m_currentState.size()))
				return false;
			return m_currentState[sc];
		}

	private:
		std::vector<bool> m_currentState;
		std::vector<bool> m_previousState;
	};

	KeyboardInput::KeyboardInput()
		: m_pImpl(std::make_unique<KeyboardInputImpl>())
	{
	}

	KeyboardInput::~KeyboardInput() = default;

	KeyboardInput::KeyboardInput(KeyboardInput&&) noexcept = default;
	KeyboardInput& KeyboardInput::operator=(KeyboardInput&&) noexcept = default;

	void KeyboardInput::Update()
	{
		m_pImpl->Update();
	}

	bool KeyboardInput::IsDownThisFrame(KeyCode key) const
	{
		return m_pImpl->IsDownThisFrame(key);
	}

	bool KeyboardInput::IsUpThisFrame(KeyCode key) const
	{
		return m_pImpl->IsUpThisFrame(key);
	}

	bool KeyboardInput::IsPressed(KeyCode key) const
	{
		return m_pImpl->IsPressed(key);
	}
}
