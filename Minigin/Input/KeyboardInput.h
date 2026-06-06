#pragma once
#include <memory>
#include "KeyCodes.h"

namespace dae
{
	class KeyboardInput final
	{
	public:
		KeyboardInput();
		~KeyboardInput();

		KeyboardInput(const KeyboardInput&) = delete;
		KeyboardInput& operator=(const KeyboardInput&) = delete;
		KeyboardInput(KeyboardInput&&) noexcept;
		KeyboardInput& operator=(KeyboardInput&&) noexcept;

		void Update();

		bool IsDownThisFrame(KeyCode key) const;
		bool IsUpThisFrame(KeyCode key) const;
		bool IsPressed(KeyCode key) const;

	private:
		class KeyboardInputImpl;
		std::unique_ptr<KeyboardInputImpl> m_pImpl;
	};
}
