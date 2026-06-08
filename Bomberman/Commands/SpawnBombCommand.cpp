#include "SpawnBombCommand.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/HealthComponent.h"

namespace dae
{
	void SpawnBombCommand::Execute()
	{
		if (m_pGameActor == nullptr)
			return;

		auto* health = m_pGameActor->GetComponent<HealthComponent>();
		if (health && health->IsDead())
			return;

		auto* pTransform = m_pGameActor->GetComponent<TransformComponent>();
		if (pTransform == nullptr)
			return;

		auto* capacity = m_pGameActor->GetComponent<BombCapacityComponent>();
		if (capacity && !capacity->CanPlaceBomb())
			return;

		const auto& pos = pTransform->GetLocalPosition();

		int explosionRange = 1;
		auto* bombRange = m_pGameActor->GetComponent<BombRangeComponent>();
		if (bombRange)
		{
			explosionRange = bombRange->GetRange();
		}

		Event placeBombEvent(make_sdbm_hash("PlaceBombEvent"));
		placeBombEvent.nbArgs = 3;
		placeBombEvent.args[0].f = pos.x;
		placeBombEvent.args[1].f = pos.y;
		placeBombEvent.args[2].i = explosionRange;

		EventManager::GetInstance().BroadcastEvent(placeBombEvent, m_pGameActor);
	}
}