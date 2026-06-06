#pragma once

// Platform-agnostic controller button definitions
// These values match XInput button masks for compatibility
namespace ControllerButton
{
	constexpr unsigned int kDpadUp = 0x0001;
	constexpr unsigned int kDpadDown = 0x0002;
	constexpr unsigned int kDpadLeft = 0x0004;
	constexpr unsigned int kDpadRight = 0x0008;
	constexpr unsigned int kStart = 0x0010;
	constexpr unsigned int kBack = 0x0020;
	constexpr unsigned int kLeftThumb = 0x0040;
	constexpr unsigned int kRightThumb = 0x0080;
	constexpr unsigned int kLeftShoulder = 0x0100;
	constexpr unsigned int kRightShoulder = 0x0200;
	constexpr unsigned int kA = 0x1000;
	constexpr unsigned int kB = 0x2000;
	constexpr unsigned int kX = 0x4000;
	constexpr unsigned int kY = 0x8000;
}
