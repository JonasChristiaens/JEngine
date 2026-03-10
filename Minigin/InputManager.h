#pragma once
#include "Singleton.h"

#include "ControllerInput.h"
#include "Command.h"

#include <memory>
#include <map>

namespace dae
{
	enum class KeyState
	{
		Down,
		Up,
		Pressed
	};

	struct ControllerKey
	{
		DWORD controllerIndex;
		unsigned int button;
		KeyState keyState;

		bool operator<(const ControllerKey& other) const
		{
			if (controllerIndex != other.controllerIndex)
				return controllerIndex < other.controllerIndex;
			if (button != other.button)
				return button < other.button;
			return keyState < other.keyState;
		}
	};

	struct KeyboardKey
	{
		SDL_Keycode key;
		KeyState keyState;

		bool operator<(const KeyboardKey& other) const
		{
			if (key != other.key)
				return key < other.key;
			return keyState < other.keyState;
		}
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

		bool BindControllerInput(DWORD controllerIndex, unsigned int button, KeyState keyState, std::unique_ptr<Command> command);
		void BindKeyboardInput(SDL_Keycode key, KeyState keyState, std::unique_ptr<Command> command);

		void AddController(DWORD controllerIndex);
		void RemoveController(DWORD controllerIndex);
		bool HasController(DWORD controllerIndex) const;

		void UnBindControllerInput(DWORD controllerIndex, unsigned int button, KeyState keyState);
		void UnBindKeyboardInput(SDL_Keycode key, KeyState keyState);

	private:
		std::map<DWORD, std::unique_ptr<ControllerInput>> m_controllers;

		std::map<ControllerKey, std::unique_ptr<Command>> m_controllerCommands;
		std::map<KeyboardKey, std::unique_ptr<Command>> m_keyboardCommands;
		std::map<SDL_Keycode, bool> m_previousKeyboardState;
	};
}
