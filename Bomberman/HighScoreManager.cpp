#include "HighScoreManager.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

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

	void HighScoreManager::SaveToFile(const std::string& filepath)
	{
		std::ofstream file(filepath);
		if (!file.is_open())
			return;

		file << "{\n  \"highScores\": [\n";
		for (int i = 0; i < kMaxEntries; ++i)
		{
			file << "    { \"name\": \"" << s_Entries[i].name << "\", \"score\": " << s_Entries[i].score << " }";
			if (i < kMaxEntries - 1)
				file << ",";
			file << "\n";
		}
		file << "  ]\n}\n";
	}

	void HighScoreManager::LoadFromFile(const std::string& filepath)
	{
		std::ifstream file(filepath);
		if (!file.is_open())
			return;

		std::stringstream buffer;
		buffer << file.rdbuf();
		const std::string content = buffer.str();

		s_Entries = {};

		size_t pos = content.find("\"highScores\"");
		if (pos == std::string::npos)
			return;

		pos = content.find('[', pos);
		if (pos == std::string::npos)
			return;
		++pos;

		int entryIndex = 0;
		while (entryIndex < kMaxEntries)
		{
			pos = content.find('{', pos);
			if (pos == std::string::npos)
				break;

			size_t nameStart = content.find("\"name\"", pos);
			if (nameStart == std::string::npos)
				break;
			nameStart = content.find('\"', nameStart + 7);
			if (nameStart == std::string::npos)
				break;
			const size_t nameEnd = content.find('\"', nameStart + 1);
			if (nameEnd == std::string::npos)
				break;
			std::string name = content.substr(nameStart + 1, nameEnd - nameStart - 1);

			size_t scoreStart = content.find("\"score\"", pos);
			if (scoreStart == std::string::npos)
				break;
			scoreStart = content.find(':', scoreStart);
			if (scoreStart == std::string::npos)
				break;
			++scoreStart;
			while (scoreStart < content.size() && (content[scoreStart] == ' ' || content[scoreStart] == '\t'))
				++scoreStart;
			const size_t scoreEnd = content.find_first_of(",}\n", scoreStart);
			std::string scoreStr = content.substr(scoreStart, scoreEnd - scoreStart);
			int score = std::stoi(scoreStr);

			s_Entries[entryIndex].score = score;
			std::copy_n(name.begin(), std::min<size_t>(name.size(), kMaxNameLength), s_Entries[entryIndex].name);
			s_Entries[entryIndex].name[kMaxNameLength] = '\0';
			++entryIndex;

			pos = scoreEnd;
		}

		std::sort(s_Entries.begin(), s_Entries.end(),
			[](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });
	}
}
