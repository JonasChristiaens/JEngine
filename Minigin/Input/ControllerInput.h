#pragma once
#include <memory>

class ControllerInput
{
public:
    ControllerInput(unsigned int controllerIndex);
    ~ControllerInput();

    ControllerInput(const ControllerInput&) = delete;
    ControllerInput& operator=(const ControllerInput&) = delete;
    ControllerInput(ControllerInput&&) noexcept;
    ControllerInput& operator=(ControllerInput&&) noexcept;

    void Update();

    bool IsDownThisFrame(unsigned int button) const;
    bool IsUpThisFrame(unsigned int button) const;
    bool IsPressed(unsigned int button) const;

private:
    class ControllerInputImpl;
    std::unique_ptr<ControllerInputImpl> m_pImpl;
};