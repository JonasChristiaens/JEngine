#include "ControllerInput.h"

ControllerInput::ControllerInput(DWORD controllerIndex)
    : m_controllerIndex(controllerIndex)
{
    ZeroMemory(&currentState, sizeof(XINPUT_STATE));
    ZeroMemory(&previousState, sizeof(XINPUT_STATE));
}

void ControllerInput::Update()
{
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // Simply get the state of the controller from XInput.
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

bool ControllerInput::IsDownThisFrame(unsigned int button) const
{
    return buttonsPressedThisFrame & button;
}

bool ControllerInput::IsUpThisFrame(unsigned int button) const
{
    return buttonsReleasedThisFrame & button;
}

bool ControllerInput::IsPressed(unsigned int button) const
{
    return currentState.Gamepad.wButtons & button;
}