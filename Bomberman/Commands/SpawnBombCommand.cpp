#include "SpawnBombCommand.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"

void SpawnBombCommand::Execute()
{
	if (m_pGameActor == nullptr)
		return;

	auto* pTransform = m_pGameActor->GetComponent<dae::TransformComponent>();
	if (pTransform == nullptr)
		return;

	const auto& pos = pTransform->GetWorldPosition();
	auto* collider = m_pGameActor->GetComponent<dae::CollisionComponent>();
	const float bombOffsetY = collider ? collider->GetHeight() : 0.0f;

	dae::Event placeBombEvent(dae::make_sdbm_hash("PlaceBombEvent"));
	placeBombEvent.nbArgs = 2;
	placeBombEvent.args[0].f = pos.x;
	placeBombEvent.args[1].f = pos.y - bombOffsetY;

	dae::EventManager::GetInstance().BroadcastEvent(placeBombEvent, m_pGameActor);
}