#include "AudioEventObserver.h"
#include "ServiceLocator.h"
#include "../EventQueue/EventManager.h"

namespace
{
	constexpr dae::EventId kPlayAudioEventId = dae::make_sdbm_hash("PlayAudioEvent");
	constexpr dae::EventId kPlayLoopingAudioEventId = dae::make_sdbm_hash("PlayLoopingAudioEvent");
}

dae::AudioEventObserver::AudioEventObserver()
{
	dae::EventManager::GetInstance().AddObserver(*this);
}

dae::AudioEventObserver::~AudioEventObserver()
{
	if (dae::EventManager::IsAlive())
	{
		dae::EventManager::GetInstance().RemoveObserver(*this);
	}
}

void dae::AudioEventObserver::Notify(GameObject& /*actor*/, Event event)
{
	if (event.id != kPlayAudioEventId && event.id != kPlayLoopingAudioEventId)
		return;

	if (event.nbArgs < 1 || event.args[0].p == nullptr)
		return;

	const auto* pPath = static_cast<const char*>(event.args[0].p);

	if (event.id == kPlayAudioEventId)
		dae::ServiceLocator::GetSoundService().PlaySound(pPath);
	else
		dae::ServiceLocator::GetSoundService().PlayLooping(pPath);
}
