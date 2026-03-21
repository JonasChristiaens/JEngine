#include "PickupComponent.h"
#include "../EventQueue/EventManager.h"
#include "../GameObject.h"

namespace dae
{
	PickupComponent::PickupComponent(GameObject* pOwner, int scoreValue)
		: BaseComponent(pOwner)
		, m_scoreValue(scoreValue)
	{
	}

	void PickupComponent::OnCollision(GameObject* other)
	{
		if (GetOwner()->IsMarkedForDeletion()) return;

		Event scoreEvent(make_sdbm_hash("ChangeScoreEvent"));
		scoreEvent.nbArgs = 1;
		scoreEvent.args[0].i = m_scoreValue;
		EventManager::GetInstance().BroadcastEvent(scoreEvent, other);

		GetOwner()->MarkForDeletion();
	}
}
