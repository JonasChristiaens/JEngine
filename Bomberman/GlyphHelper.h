#pragma once
#include <utility>

namespace dae
{
	namespace GlyphHelper
	{
		constexpr float kGlyphWidth{ 8.0f };
		constexpr float kGlyphRowY{ 264.0f };
		constexpr float kGlyphRow2Y{ 272.0f };

		inline std::pair<float, float> GetCharacterSrcRect(char character)
		{
			if (character >= '0' && character <= '9')
			{
				const int index = character - '0';
				return { static_cast<float>(index) * kGlyphWidth, kGlyphRowY };
			}

			char upper = character;
			if (upper >= 'a' && upper <= 'z')
				upper = static_cast<char>(upper - ('a' - 'A'));

			if (upper >= 'A' && upper <= 'Z')
			{
				const int index = upper - 'A';
				return { static_cast<float>(index) * kGlyphWidth, kGlyphRow2Y };
			}

			return { 0.0f, kGlyphRow2Y };
		}
	}
}
