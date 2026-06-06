#include <SDL3/SDL.h>
#include "InputManager.h"
#include <vector>

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_EVENT_QUIT)
		{
			return false;
		}
	}

	for (auto& [index, controller] : m_Controllers)
	{
		controller->Update();
	}

	m_KeyboardInput.Update();

	std::vector<ControllerKey> controllerCommandsToRun{};
	controllerCommandsToRun.reserve(m_ControllerCommands.size());
	for (auto& [controllerKey, command] : m_ControllerCommands)
	{
		auto controllerIt = m_Controllers.find(controllerKey.controllerIndex);
		if (controllerIt != m_Controllers.end())
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
				controllerCommandsToRun.push_back(controllerKey);
			}
		}
	}
	for (const auto& controllerKey : controllerCommandsToRun)
	{
		auto commandIt = m_ControllerCommands.find(controllerKey);
		if (commandIt != m_ControllerCommands.end())
		{
			commandIt->second->Execute();
		}
	}

	std::vector<KeyboardKey> keyboardCommandsToRun{};
	keyboardCommandsToRun.reserve(m_KeyboardCommands.size());
	for (auto& [keyboardKey, command] : m_KeyboardCommands)
	{
		bool shouldExecute = false;

		switch (keyboardKey.keyState)
		{
		case KeyState::Down:
			shouldExecute = m_KeyboardInput.IsDownThisFrame(keyboardKey.key);
			break;
		case KeyState::Up:
			shouldExecute = m_KeyboardInput.IsUpThisFrame(keyboardKey.key);
			break;
		case KeyState::Pressed:
			shouldExecute = m_KeyboardInput.IsPressed(keyboardKey.key);
			break;
		}

		if (shouldExecute)
		{
			keyboardCommandsToRun.push_back(keyboardKey);
		}
	}
	for (const auto& keyboardKey : keyboardCommandsToRun)
	{
		auto commandIt = m_KeyboardCommands.find(keyboardKey);
		if (commandIt != m_KeyboardCommands.end())
		{
			commandIt->second->Execute();
		}
	}

	return true;
}

bool dae::InputManager::BindControllerInput(unsigned int controllerIndex, unsigned int button, KeyState keyState, std::unique_ptr<Command> command)
{
	if (m_Controllers.find(controllerIndex) == m_Controllers.end())
	{
		return false;
	}

	m_ControllerCommands.emplace(ControllerKey{ controllerIndex, button, keyState }, std::move(command));
	return true;
}

void dae::InputManager::BindKeyboardInput(KeyCode key, KeyState keyState, std::unique_ptr<Command> command)
{
	m_KeyboardCommands.emplace(KeyboardKey{ key, keyState }, std::move(command));
}

void dae::InputManager::UnBindControllerInput(unsigned int controllerIndex, unsigned int button, KeyState keyState)
{
	ControllerKey key{ controllerIndex, button, keyState };
	m_ControllerCommands.erase(key);
}

void dae::InputManager::UnBindKeyboardInput(KeyCode key, KeyState keyState)
{
	KeyboardKey keyboardKey{ key, keyState };
	m_KeyboardCommands.erase(keyboardKey);
}

void dae::InputManager::AddController(unsigned int controllerIndex)
{
	if (m_Controllers.find(controllerIndex) == m_Controllers.end())
	{
		m_Controllers[controllerIndex] = std::make_unique<ControllerInput>(controllerIndex);
	}
}

void dae::InputManager::RemoveController(unsigned int controllerIndex)
{
	m_Controllers.erase(controllerIndex);
}

bool dae::InputManager::HasController(unsigned int controllerIndex) const
{
	return m_Controllers.find(controllerIndex) != m_Controllers.end();
}

void dae::InputManager::ClearAllBindings()
{
	m_KeyboardCommands.clear();
	m_ControllerCommands.clear();
}
