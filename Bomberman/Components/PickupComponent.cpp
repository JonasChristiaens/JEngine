#include "PickupComponent.h"
#include "Powerups/PowerupEffect.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"

namespace dae
{
	PickupComponent::PickupComponent(GameObject* pOwner, int scoreValue, std::unique_ptr<PowerupEffect> pEffect)
		: BaseComponent(pOwner)
		, m_ScoreValue(scoreValue)
		, m_pEffect(std::move(pEffect))
	{
	}

	void PickupComponent::OnCollision(GameObject* other)
	{
		if (GetOwner()->IsMarkedForDeletion())
		{
			return;
		}

		if (m_pEffect)
		{
			m_pEffect->Apply(other);
		}

		Event scoreEvent(make_sdbm_hash("ChangeScoreEvent"));
		scoreEvent.nbArgs = 1;
		scoreEvent.args[0].i = m_ScoreValue;
		EventManager::GetInstance().BroadcastEvent(scoreEvent, other);

		Event playAudioEvent(make_sdbm_hash("PlayAudioEvent"));
		playAudioEvent.nbArgs = 1;
		playAudioEvent.args[0].p = "powerup.wav";
		EventManager::GetInstance().BroadcastEvent(playAudioEvent, other);

		GetOwner()->MarkForDeletion();
	}
}