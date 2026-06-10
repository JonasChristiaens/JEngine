#include "BombComponent.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/RectBounds.h"

dae::BombComponent::BombComponent(GameObject* pOwnerBomb, GameObject* pOwnerPlayer, float tileSize, int explosionRange)
	: BaseComponent(pOwnerBomb)
	, m_pOwnerPlayer(pOwnerPlayer)
	, m_TileSize(tileSize)
	, m_ExplosionRange(explosionRange)
{}

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
	if (!ownerTransform || !bombTransform)
		return false;

	const auto& ownerWorldPos = ownerTransform->GetWorldPosition();
	const auto& bombWorldPos = bombTransform->GetWorldPosition();

	const RectBounds ownerBox = [&]()
		{
			auto* ownerCollider = m_pOwnerPlayer->GetComponent<CollisionComponent>();
			if (ownerCollider)
			{
				return RectBounds::FromOffset(ownerWorldPos.x, ownerWorldPos.y,
					ownerCollider->GetWidth(), ownerCollider->GetHeight(),
					ownerCollider->GetOffset().x, ownerCollider->GetOffset().y);
			}
			return RectBounds::FromOffset(ownerWorldPos.x, ownerWorldPos.y, 0.0f, 0.0f, 0.0f, 0.0f);
		}();

	const RectBounds bombBox = RectBounds::FromCenterSize(bombWorldPos.x, bombWorldPos.y, m_TileSize, m_TileSize);

	return ownerBox.Overlaps(bombBox);
}

void dae::BombComponent::AddSolidCollider()
{
	m_HasAddedCollision = true;
	auto* collider = GetOwner()->AddComponent<CollisionComponent>(m_TileSize, m_TileSize);
	collider->SetOffset({ -m_TileSize * 0.5f, -m_TileSize * 0.5f });
}
