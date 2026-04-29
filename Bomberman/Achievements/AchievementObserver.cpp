#include "AchievementObserver.h"

#if USE_STEAMWORKS
#include "CSteamAchievements.h"

namespace dae
{
	AchievementObserver::AchievementObserver(CSteamAchievements* pAchievements)
		: m_pAchievements(pAchievements)
	{
	}

	void AchievementObserver::Notify(const dae::GameObject&, dae::Event event)
	{
		if (event.id == dae::make_sdbm_hash("PlayerWon"))
		{
			if (m_pAchievements)
				m_pAchievements->SetAchievement("ACH_WIN_ONE_GAME");
		}
		else if (event.id == dae::make_sdbm_hash("ResetAchievements"))
		{
			if (m_pAchievements)
				m_pAchievements->ClearAchievement("ACH_WIN_ONE_GAME");
		}
	}
}
#endif // USE_STEAMWORKS
