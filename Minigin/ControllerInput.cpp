#include "ControllerInput.h"

#ifdef _WIN32
#include <windows.h>
#include <Xinput.h>
#else
#include <SDL3/SDL.h>
#endif

// Implementation class for XInput (Windows)
#ifdef _WIN32
class ControllerInput::ControllerInputImpl
{
public:
    ControllerInputImpl(unsigned int controllerIndex)
        : m_controllerIndex(controllerIndex)
    {
        ZeroMemory(&currentState, sizeof(XINPUT_STATE));
        ZeroMemory(&previousState, sizeof(XINPUT_STATE));
    }

    void Update()
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        DWORD dwResult = XInputGetState(m_controllerIndex, &state);

        if (dwResult == ERROR_SUCCESS)
        {
            CopyMemory(&previousState, &currentState, sizeof(XINPUT_STATE));
            ZeroMemory(&currentState, sizeof(XINPUT_STATE));
            XInputGetState(m_controllerIndex, &currentState);

            auto buttonChanges = currentState.Gamepad.wButtons ^ previousState.Gamepad.wButtons;
            buttonsPressedThisFrame = buttonChanges & currentState.Gamepad.wButtons;
            buttonsReleasedThisFrame = buttonChanges & (~currentState.Gamepad.wButtons);
        }
    }

    bool IsDownThisFrame(unsigned int button) const
    {
        return buttonsPressedThisFrame & button;
    }

    bool IsUpThisFrame(unsigned int button) const
    {
        return buttonsReleasedThisFrame & button;
    }

    bool IsPressed(unsigned int button) const
    {
        return currentState.Gamepad.wButtons & button;
    }

private:
    DWORD m_controllerIndex;
    XINPUT_STATE currentState{};
    XINPUT_STATE previousState{};
    WORD buttonsPressedThisFrame{};
    WORD buttonsReleasedThisFrame{};
};
#else
// Implementation class for SDL (Emscripten and other platforms)
class ControllerInput::ControllerInputImpl
{
public:
    ControllerInputImpl(unsigned int controllerIndex)
        : m_controllerIndex(controllerIndex)
        , m_gamepad(nullptr)
        , currentButtonState(0)
        , previousButtonState(0)
        , buttonsPressedThisFrame(0)
        , buttonsReleasedThisFrame(0)
    {
        // Open the gamepad
        SDL_JoystickID* joysticks = SDL_GetJoysticks(nullptr);
        if (joysticks && static_cast<int>(controllerIndex) < SDL_GetNumJoysticks())
        {
            m_gamepad = SDL_OpenGamepad(joysticks[controllerIndex]);
        }
    }

    ~ControllerInputImpl()
    {
        if (m_gamepad)
        {
            SDL_CloseGamepad(m_gamepad);
        }
    }

    void Update()
    {
        if (!m_gamepad)
            return;

        previousButtonState = currentButtonState;
        currentButtonState = 0;

        // Map SDL gamepad buttons to XInput-like button mask
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_SOUTH)) currentButtonState |= 0x1000; // A
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_EAST)) currentButtonState |= 0x2000;  // B
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_WEST)) currentButtonState |= 0x4000;  // X
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_NORTH)) currentButtonState |= 0x8000; // Y
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) currentButtonState |= 0x0100;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) currentButtonState |= 0x0200;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_BACK)) currentButtonState |= 0x0020;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_START)) currentButtonState |= 0x0010;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP)) currentButtonState |= 0x0001;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) currentButtonState |= 0x0002;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)) currentButtonState |= 0x0004;
        if (SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) currentButtonState |= 0x0008;

        auto buttonChanges = currentButtonState ^ previousButtonState;
        buttonsPressedThisFrame = buttonChanges & currentButtonState;
        buttonsReleasedThisFrame = buttonChanges & (~currentButtonState);
    }

    bool IsDownThisFrame(unsigned int button) const
    {
        return buttonsPressedThisFrame & button;
    }

    bool IsUpThisFrame(unsigned int button) const
    {
        return buttonsReleasedThisFrame & button;
    }

    bool IsPressed(unsigned int button) const
    {
        return currentButtonState & button;
    }

private:
    unsigned int m_controllerIndex;
    SDL_Gamepad* m_gamepad;
    unsigned short currentButtonState;
    unsigned short previousButtonState;
    unsigned short buttonsPressedThisFrame;
    unsigned short buttonsReleasedThisFrame;
};
#endif

// ControllerInput public interface implementation
ControllerInput::ControllerInput(unsigned int controllerIndex)
    : m_pImpl(std::make_unique<ControllerInputImpl>(controllerIndex))
{
}

ControllerInput::~ControllerInput() = default;

ControllerInput::ControllerInput(ControllerInput&&) noexcept = default;
ControllerInput& ControllerInput::operator=(ControllerInput&&) noexcept = default;

void ControllerInput::Update()
{
    m_pImpl->Update();
}

bool ControllerInput::IsDownThisFrame(unsigned int button) const
{
    return m_pImpl->IsDownThisFrame(button);
}

bool ControllerInput::IsUpThisFrame(unsigned int button) const
{
    return m_pImpl->IsUpThisFrame(button);
}

bool ControllerInput::IsPressed(unsigned int button) const
{
    return m_pImpl->IsPressed(button);
}