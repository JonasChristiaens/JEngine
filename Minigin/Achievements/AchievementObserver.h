#pragma once

#if USE_STEAMWORKS
#include "../EventQueue/IObserver.h"
#include "../EventQueue/Event.h"

namespace dae
{
	class CSteamAchievements;

	class AchievementObserver final : public dae::IObserver
	{
	public:
		AchievementObserver(CSteamAchievements* pAchievements);
		virtual void Notify(const dae::GameObject& pGameActor, dae::Event event) override;

	private:
		CSteamAchievements* m_pAchievements;
	};
}
#endif // USE_STEAMWORKS
