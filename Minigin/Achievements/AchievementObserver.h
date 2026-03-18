#pragma once
#include "EventQueue/Observer.h"

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