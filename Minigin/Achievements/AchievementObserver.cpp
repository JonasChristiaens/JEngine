#if USE_STEAMWORKS

#include "AchievementObserver.h"
#include "CSteamAchievements.h"

namespace dae
{
	AchievementObserver::AchievementObserver(CSteamAchievements* pSteamAchievements)
		: m_pSteamAchievements(pSteamAchievements)
	{
	}

	void AchievementObserver::Notify(Event e, GameObject*)
	{
		if (e == Event::PlayerWonGame)
		{
			if (m_pSteamAchievements)
			{
				m_pSteamAchievements->SetAchievement("ACH_WIN_ONE_GAME");
			}
		}
	}
}

#endif // USE_STEAMWORKS