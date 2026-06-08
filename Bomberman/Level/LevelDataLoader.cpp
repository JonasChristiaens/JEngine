#include "LevelDataLoader.h"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace dae
{
	namespace
	{
		struct JsonView
		{
			const std::string& text;
			size_t pos{ 0 };

			void SkipWhitespace()
			{
				while (pos < text.size())
				{
					const char c = text[pos];
					if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
					{
						++pos;
					}
					else
					{
						break;
					}
				}
			}

			void ThrowError(const std::string& message) const
			{
				throw std::runtime_error(message);
			}

			bool Match(char expected)
			{
				SkipWhitespace();
				if (pos < text.size() && text[pos] == expected)
				{
					++pos;
					return true;
				}
				return false;
			}

			void Expect(char expected)
			{
				if (!Match(expected))
				{
					ThrowError("Invalid JSON format");
				}
			}

			std::string ParseString()
			{
				SkipWhitespace();
				if (pos >= text.size() || text[pos] != '"')
				{
					ThrowError("Expected string");
				}
				++pos;
				std::string result{};
				while (pos < text.size() && text[pos] != '"')
				{
					result.push_back(text[pos]);
					++pos;
				}
				if (pos >= text.size())
				{
					ThrowError("Unterminated string");
				}
				++pos;
				return result;
			}

			int ParseInt()
			{
				SkipWhitespace();
				bool negative = false;
				if (pos < text.size() && text[pos] == '-')
				{
					negative = true;
					++pos;
				}
				if (pos >= text.size() || text[pos] < '0' || text[pos] > '9')
				{
					ThrowError("Expected integer");
				}
				int value = 0;
				while (pos < text.size() && text[pos] >= '0' && text[pos] <= '9')
				{
					value = value * 10 + (text[pos] - '0');
					++pos;
				}
				return negative ? -value : value;
			}
		};

		void ParseReservedTiles(JsonView& view, LevelData& level)
		{
			view.Expect('[');
			view.SkipWhitespace();
			if (view.Match(']'))
			{
				return;
			}
			while (true)
			{
				view.Expect('[');
				const int x = view.ParseInt();
				view.Expect(',');
				const int y = view.ParseInt();
				view.Expect(']');
				level.reservedTiles.emplace_back(x, y);
				view.SkipWhitespace();
				if (view.Match(']'))
				{
					break;
				}
				view.Expect(',');
			}
		}

		PickupType ParsePickupType(const std::string& value)
		{
			if (value == "none")
			{
				return PickupType::None;
			}
			if (value == "bomb")
			{
				return PickupType::Bomb;
			}
			if (value == "flames")
			{
				return PickupType::Flames;
			}
			if (value == "skate")
			{
				return PickupType::Skate;
			}
			if (value == "remoteControl")
			{
				return PickupType::RemoteControl;
			}
			throw std::runtime_error("Unknown pickup type");
		}

		void ParseLevelObject(JsonView& view, LevelData& level)
		{
			view.Expect('{');
			while (true)
			{
				view.SkipWhitespace();
				if (view.Match('}'))
				{
					break;
				}
			const std::string key = view.ParseString();
				view.Expect(':');
				if (key == "softBlockCount")
				{
					level.softBlockCount = view.ParseInt();
				}
				else if (key == "balloomCount")
				{
					level.balloomCount = view.ParseInt();
				}
				else if (key == "onealCount")
				{
					level.onealCount = view.ParseInt();
				}
				else if (key == "dollCount")
				{
					level.dollCount = view.ParseInt();
				}
				else if (key == "minvoCount")
				{
					level.minvoCount = view.ParseInt();
				}
				else if (key == "pickupType")
				{
					level.pickupType = ParsePickupType(view.ParseString());
				}
				else if (key == "reservedTiles")
				{
					ParseReservedTiles(view, level);
				}
				else
				{
					view.ThrowError("Unexpected key in level data");
				}
				view.SkipWhitespace();
				if (view.Match('}'))
				{
					break;
				}
				view.Expect(',');
			}
		}
	}

	std::vector<LevelData> LevelDataLoader::LoadFromText(const std::string& jsonText)
	{
		return ParseLevels(jsonText);
	}

	std::vector<LevelData> LevelDataLoader::ParseLevels(const std::string& jsonText)
	{
		JsonView view{ jsonText };
		view.SkipWhitespace();
		view.Expect('{');

		std::vector<LevelData> levels{};
		bool foundLevels = false;

		while (true)
		{
			view.SkipWhitespace();
			if (view.Match('}'))
			{
				break;
			}

			const std::string key = view.ParseString();
			view.Expect(':');
			if (key != "levels")
			{
				view.ThrowError("Expected levels array");
			}
			foundLevels = true;
			view.Expect('[');
			view.SkipWhitespace();
			if (!view.Match(']'))
			{
				while (true)
				{
					LevelData level{};
					ParseLevelObject(view, level);
					levels.push_back(level);
					view.SkipWhitespace();
					if (view.Match(']'))
					{
						break;
					}
					view.Expect(',');
				}
			}
			view.SkipWhitespace();
			if (view.Match('}'))
			{
				break;
			}
			view.Expect(',');
		}

		if (!foundLevels)
		{
			view.ThrowError("Level data missing levels array");
		}

		return levels;
	}
}
