#pragma once
#include <windows.h>
#include <Xinput.h>

class ControllerInput
{
public:
    ControllerInput(DWORD controllerIndex);

    void Update();

    bool IsDownThisFrame(unsigned int button) const;
    bool IsUpThisFrame(unsigned int button) const;
    bool IsPressed(unsigned int button) const;

private:
    DWORD m_controllerIndex;
    XINPUT_STATE currentState{};
    XINPUT_STATE previousState{};
    WORD buttonsPressedThisFrame{};
    WORD buttonsReleasedThisFrame{};
};