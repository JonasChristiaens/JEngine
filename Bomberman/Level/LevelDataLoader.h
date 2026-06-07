#pragma once

#include "LevelData.h"

#include <string>
#include <vector>

namespace dae
{
	class LevelDataLoader
	{
	public:
		static std::vector<LevelData> LoadFromText(const std::string& jsonText);

	private:
		static std::vector<LevelData> ParseLevels(const std::string& jsonText);
	};
}
