#include "BombComponent.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"

dae::BombComponent::BombComponent(GameObject* pOwnerBomb, GameObject* pOwnerPlayer, float tileSize, int explosionRange)
	: BaseComponent(pOwnerBomb)
	, m_pOwnerPlayer(pOwnerPlayer)
	, m_TileSize(tileSize)
	, m_ExplosionRange(explosionRange)
{
}

void dae::BombComponent::Update()
{
	if (m_HasAddedCollision)
		return;

	if (!OwnerOverlapsBomb())
	{
		AddSolidCollider();
	}
}

bool dae::BombComponent::OwnerOverlapsBomb() const
{
	if (!m_pOwnerPlayer || m_pOwnerPlayer->IsMarkedForDeletion())
		return false;

	auto* ownerTransform = m_pOwnerPlayer->GetComponent<TransformComponent>();
	auto* bombTransform = GetOwner()->GetComponent<TransformComponent>();
	auto* ownerCollider = m_pOwnerPlayer->GetComponent<CollisionComponent>();
	if (!ownerTransform || !bombTransform)
		return false;

	const auto& ownerWorldPos = ownerTransform->GetWorldPosition();
	const auto& bombWorldPos = bombTransform->GetWorldPosition();

	float ownerLeft, ownerRight, ownerTop, ownerBottom;
	if (ownerCollider)
	{
		ownerLeft   = ownerWorldPos.x + ownerCollider->GetOffset().x;
		ownerRight  = ownerLeft + ownerCollider->GetWidth();
		ownerTop    = ownerWorldPos.y + ownerCollider->GetOffset().y;
		ownerBottom = ownerTop + ownerCollider->GetHeight();
	}
	else
	{
		ownerLeft   = ownerWorldPos.x;
		ownerRight  = ownerWorldPos.x;
		ownerTop    = ownerWorldPos.y;
		ownerBottom = ownerWorldPos.y;
	}

	const float bombHalf = m_TileSize * 0.5f;
	const float bombLeft   = bombWorldPos.x - bombHalf;
	const float bombRight  = bombWorldPos.x + bombHalf;
	const float bombTop    = bombWorldPos.y - bombHalf;
	const float bombBottom = bombWorldPos.y + bombHalf;

	return ownerLeft < bombRight && ownerRight > bombLeft &&
	       ownerTop < bombBottom && ownerBottom > bombTop;
}

void dae::BombComponent::AddSolidCollider()
{
	m_HasAddedCollision = true;
	auto* collider = GetOwner()->AddComponent<CollisionComponent>(m_TileSize, m_TileSize);
	collider->SetOffset({ -m_TileSize * 0.5f, -m_TileSize * 0.5f });
}