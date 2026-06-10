#include "LevelDataLoader.h"

#include <fstream>
#include <string>
#include <vector>

namespace
{
	constexpr const char* kMagic = "JLVL";
	constexpr int kVersion = 1;
}

namespace dae
{
	std::vector<LevelData> LevelDataLoader::Load(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::binary);
		if (!file.is_open())
			return {};

		char magic[5]{};
		file.read(magic, 4);
		if (std::string(magic) != kMagic)
			return {};

		int version = 0;
		file.read(reinterpret_cast<char*>(&version), sizeof(version));
		if (version != kVersion)
			return {};

		int levelCount = 0;
		file.read(reinterpret_cast<char*>(&levelCount), sizeof(levelCount));

		std::vector<LevelData> levels{};
		levels.reserve(static_cast<size_t>(levelCount));

		for (int i = 0; i < levelCount; ++i)
		{
			LevelData level{};

			file.read(reinterpret_cast<char*>(&level.softBlockCount), sizeof(level.softBlockCount));

			int tileCount = 0;
			file.read(reinterpret_cast<char*>(&tileCount), sizeof(tileCount));
			level.reservedTiles.reserve(static_cast<size_t>(tileCount));
			for (int t = 0; t < tileCount; ++t)
			{
				int x = 0, y = 0;
				file.read(reinterpret_cast<char*>(&x), sizeof(x));
				file.read(reinterpret_cast<char*>(&y), sizeof(y));
				level.reservedTiles.emplace_back(x, y);
			}

			file.read(reinterpret_cast<char*>(&level.balloomCount), sizeof(level.balloomCount));
			file.read(reinterpret_cast<char*>(&level.onealCount), sizeof(level.onealCount));
			file.read(reinterpret_cast<char*>(&level.dollCount), sizeof(level.dollCount));
			file.read(reinterpret_cast<char*>(&level.minvoCount), sizeof(level.minvoCount));

			int pickupInt = 0;
			file.read(reinterpret_cast<char*>(&pickupInt), sizeof(pickupInt));
			level.pickupType = static_cast<PickupType>(pickupInt);

			levels.push_back(level);
		}

		return levels;
	}
}
