#include "ResetAchievementsCommand.h"
#if USE_STEAMWORKS
#include "EventQueue/EventManager.h"

void ResetAchievementsCommand::Execute()
{
	dae::EventManager::GetInstance().BroadcastEvent(dae::Event(dae::make_sdbm_hash("ResetAchievements")), nullptr);
}
#endif // USE_STEAMWORKS
