#pragma once
#include <array>
#include <string>

namespace dae
{
	struct HighScoreEntry
	{
		char name[5]{};
		int score{ 0 };
	};

	class HighScoreManager
	{
	public:
		static constexpr int kMaxEntries{ 4 };
		static constexpr int kMaxNameLength{ 4 };

		static const std::array<HighScoreEntry, kMaxEntries>& GetEntries();
		static bool IsHighScore(int score);
		static int SubmitScore(const std::string& name, int score);
		static void UpdateName(int entryIndex, const std::string& name);
		static void Load(const std::string& filepath);
		static void Save(const std::string& filepath);

	private:
		inline static std::array<HighScoreEntry, kMaxEntries> s_Entries{};
	};
}
