#pragma once

#include "LevelData.h"
#include <string>
#include <vector>

namespace dae
{
	class LevelDataLoader
	{
	public:
		static std::vector<LevelData> Load(const std::string& filepath);
	};
}
