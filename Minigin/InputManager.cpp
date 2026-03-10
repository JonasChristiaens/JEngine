#include <SDL3/SDL.h>
#include "InputManager.h"

bool dae::InputManager::ProcessInput()
{
	// Update all controllers
	for (auto& [index, controller] : m_controllers)
	{
		controller->Update();
	}

	// Get current keyboard state
	int numKeys;
	const bool* keyboardState = SDL_GetKeyboardState(&numKeys);

	// Execute controller commands
	for (auto& [controllerKey, command] : m_controllerCommands)
	{
		auto controllerIt = m_controllers.find(controllerKey.controllerIndex);
		if (controllerIt != m_controllers.end())
		{
			bool shouldExecute = false;

			switch (controllerKey.keyState)
			{
			case KeyState::Down:
				shouldExecute = controllerIt->second->IsDownThisFrame(controllerKey.button);
				break;
			case KeyState::Up:
				shouldExecute = controllerIt->second->IsUpThisFrame(controllerKey.button);
				break;
			case KeyState::Pressed:
				shouldExecute = controllerIt->second->IsPressed(controllerKey.button);
				break;
			}

			if (shouldExecute)
			{
				command->Execute();
			}
		}
	}

	// Execute keyboard commands
	for (auto& [keyboardKey, command] : m_keyboardCommands)
	{
		SDL_Scancode scancode = SDL_GetScancodeFromKey(keyboardKey.key, nullptr);
		bool isCurrentlyPressed = keyboardState[scancode];
		bool wasPreviouslyPressed = m_previousKeyboardState[keyboardKey.key];
		bool shouldExecute = false;

		switch (keyboardKey.keyState)
		{
		case KeyState::Down:
			shouldExecute = isCurrentlyPressed && !wasPreviouslyPressed;
			break;
		case KeyState::Up:
			shouldExecute = !isCurrentlyPressed && wasPreviouslyPressed;
			break;
		case KeyState::Pressed:
			shouldExecute = isCurrentlyPressed;
			break;
		}

		if (shouldExecute)
		{
			command->Execute();
		}

		m_previousKeyboardState[keyboardKey.key] = isCurrentlyPressed;
	}

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {

		}
		// etc...
	}
	return true;
}

bool dae::InputManager::BindControllerInput(unsigned int controllerIndex, unsigned int button, KeyState keyState, std::unique_ptr<Command> command)
{
	// If no controller present, return early
	if (m_controllers.find(controllerIndex) == m_controllers.end())
	{
		return false;
	}

	m_controllerCommands.emplace(ControllerKey{ controllerIndex, button, keyState }, std::move(command));
	return true;
}

void dae::InputManager::BindKeyboardInput(SDL_Keycode key, KeyState keyState, std::unique_ptr<Command> command)
{
	m_keyboardCommands.emplace(KeyboardKey{ key, keyState }, std::move(command));
}

void dae::InputManager::UnBindControllerInput(unsigned int controllerIndex, unsigned int button, KeyState keyState)
{
	ControllerKey key{ controllerIndex, button, keyState };
	m_controllerCommands.erase(key);
}

void dae::InputManager::UnBindKeyboardInput(SDL_Keycode key, KeyState keyState)
{
	KeyboardKey keyboardKey{ key, keyState };
	m_keyboardCommands.erase(keyboardKey);
}

void dae::InputManager::AddController(unsigned int controllerIndex)
{
	if (m_controllers.find(controllerIndex) == m_controllers.end())
	{
		m_controllers[controllerIndex] = std::make_unique<ControllerInput>(controllerIndex);
	}
}

void dae::InputManager::RemoveController(unsigned int controllerIndex)
{
	m_controllers.erase(controllerIndex);
}

bool dae::InputManager::HasController(unsigned int controllerIndex) const
{
	return m_controllers.find(controllerIndex) != m_controllers.end();
}
