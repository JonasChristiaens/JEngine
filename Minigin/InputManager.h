#pragma once
#include "Singleton.h"

#include "ControllerInput.h"
#include "Command.h"

#include <memory>
#include <unordered_map>

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
		unsigned int controllerIndex;
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

		bool operator==(const ControllerKey& other) const
		{
			return controllerIndex == other.controllerIndex && 
				   button == other.button && 
				   keyState == other.keyState;
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

		bool operator==(const KeyboardKey& other) const
		{
			return key == other.key && keyState == other.keyState;
		}
	};

	struct ControllerKeyHash
	{
		std::size_t operator()(const ControllerKey& k) const
		{
			std::size_t h1 = std::hash<unsigned int>{}(k.controllerIndex);
			std::size_t h2 = std::hash<unsigned int>{}(k.button);
			std::size_t h3 = std::hash<int>{}(static_cast<int>(k.keyState));
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	struct KeyboardKeyHash
	{
		std::size_t operator()(const KeyboardKey& k) const
		{
			std::size_t h1 = std::hash<SDL_Keycode>{}(k.key);
			std::size_t h2 = std::hash<int>{}(static_cast<int>(k.keyState));
			return h1 ^ (h2 << 1);
		}
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

		bool BindControllerInput(unsigned int controllerIndex, unsigned int button, KeyState keyState, std::unique_ptr<Command> command);
		void BindKeyboardInput(SDL_Keycode key, KeyState keyState, std::unique_ptr<Command> command);

		void AddController(unsigned int controllerIndex);
		void RemoveController(unsigned int controllerIndex);
		bool HasController(unsigned int controllerIndex) const;

		void UnBindControllerInput(unsigned int controllerIndex, unsigned int button, KeyState keyState);
		void UnBindKeyboardInput(SDL_Keycode key, KeyState keyState);

	private:
		std::unordered_map<unsigned int, std::unique_ptr<ControllerInput>> m_controllers;

		std::unordered_map<ControllerKey, std::unique_ptr<Command>, ControllerKeyHash> m_controllerCommands;
		std::unordered_map<KeyboardKey, std::unique_ptr<Command>, KeyboardKeyHash> m_keyboardCommands;
		std::unordered_map<SDL_Keycode, bool> m_previousKeyboardState;
	};
}
