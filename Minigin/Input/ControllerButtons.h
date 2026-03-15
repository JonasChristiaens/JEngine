#pragma once

// Platform-agnostic controller button definitions
// These values match XInput button masks for compatibility
namespace ControllerButton
{
    constexpr unsigned int DPadUp = 0x0001;
    constexpr unsigned int DPadDown = 0x0002;
    constexpr unsigned int DPadLeft = 0x0004;
    constexpr unsigned int DPadRight = 0x0008;
    constexpr unsigned int Start = 0x0010;
    constexpr unsigned int Back = 0x0020;
    constexpr unsigned int LeftThumb = 0x0040;
    constexpr unsigned int RightThumb = 0x0080;
    constexpr unsigned int LeftShoulder = 0x0100;
    constexpr unsigned int RightShoulder = 0x0200;
    constexpr unsigned int A = 0x1000;
    constexpr unsigned int B = 0x2000;
    constexpr unsigned int X = 0x4000;
    constexpr unsigned int Y = 0x8000;
}
