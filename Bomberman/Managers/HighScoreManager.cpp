#include "HighScoreManager.h"
#include <algorithm>
#include <filesystem>
#include <fstream>

namespace
{
	constexpr const char* kMagic = "JHSC";
	constexpr int kVersion = 1;
}

namespace dae
{
	const std::array<HighScoreEntry, HighScoreManager::kMaxEntries>& HighScoreManager::GetEntries()
	{
		return s_Entries;
	}

	bool HighScoreManager::IsHighScore(int score)
	{
		if (score <= 0)
			return false;

		for (const auto& entry : s_Entries)
		{
			if (entry.score == 0)
				return true;
		}

		const auto lowestIt = std::min_element(s_Entries.begin(), s_Entries.end(),
			[](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score < b.score; });
		return score > lowestIt->score;
	}

	int HighScoreManager::SubmitScore(const std::string& name, int score)
	{
		if (!IsHighScore(score))
			return -1;

		auto lowestIt = std::min_element(s_Entries.begin(), s_Entries.end(),
			[](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score < b.score; });

		if (score > lowestIt->score || lowestIt->score == 0)
		{
			lowestIt->score = score;
			std::copy_n(name.begin(), std::min<size_t>(name.size(), kMaxNameLength), lowestIt->name);
			lowestIt->name[kMaxNameLength] = '\0';
		}

		std::sort(s_Entries.begin(), s_Entries.end(),
			[](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });

		for (int i = 0; i < kMaxEntries; ++i)
		{
			if (s_Entries[i].score == score && std::string(s_Entries[i].name) == name)
				return i;
		}
		return -1;
	}

	void HighScoreManager::UpdateName(int entryIndex, const std::string& name)
	{
		if (entryIndex < 0 || entryIndex >= kMaxEntries)
			return;

		auto& entry = s_Entries[entryIndex];
		std::copy_n(name.begin(), std::min<size_t>(name.size(), kMaxNameLength), entry.name);
		entry.name[kMaxNameLength] = '\0';
	}

	void HighScoreManager::Save(const std::string& filepath)
	{
		const std::filesystem::path path(filepath);
		std::error_code ec;
		std::filesystem::create_directories(path.parent_path(), ec);

		std::ofstream file(path, std::ios::binary);
		if (!file.is_open())
			return;

		file.write(kMagic, 4);

		const int version = kVersion;
		file.write(reinterpret_cast<const char*>(&version), sizeof(version));

		for (int i = 0; i < kMaxEntries; ++i)
		{
			file.write(s_Entries[i].name, sizeof(s_Entries[i].name));
			file.write(reinterpret_cast<const char*>(&s_Entries[i].score), sizeof(s_Entries[i].score));
		}
	}

	void HighScoreManager::Load(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::binary);
		if (!file.is_open())
			return;

		char magic[5]{};
		file.read(magic, 4);
		if (std::string(magic) != kMagic)
			return;

		int version = 0;
		file.read(reinterpret_cast<char*>(&version), sizeof(version));
		if (version != kVersion)
			return;

		for (int i = 0; i < kMaxEntries; ++i)
		{
			file.read(s_Entries[i].name, sizeof(s_Entries[i].name));
			file.read(reinterpret_cast<char*>(&s_Entries[i].score), sizeof(s_Entries[i].score));
		}

		std::sort(s_Entries.begin(), s_Entries.end(),
			[](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });
	}
}
