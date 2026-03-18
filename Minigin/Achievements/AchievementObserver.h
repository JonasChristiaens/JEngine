#pragma once
#include "EventQueue/Observer.h"

#if USE_STEAMWORKS

namespace dae
{
	class CSteamAchievements;

	class AchievementObserver final : public Observer
	{
	public:
		AchievementObserver(CSteamAchievements* pSteamAchievements);
		~AchievementObserver() override = default;

		void Notify(Event e, GameObject* pSubjectActor) override;

	private:
		CSteamAchievements* m_pSteamAchievements;
	};
}

#endif // USE_STEAMWORKS