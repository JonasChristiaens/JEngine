#include "HiddenItemManager.h"
#include "PlayfieldGrid.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/PickupComponent.h"
#include "Components/BombRangeComponent.h"
#include "Powerups/FlamesEffect.h"
#include "Powerups/ExtraBombEffect.h"
#include "Powerups/DetonatorEffect.h"
#include "Powerups/SkateEffect.h"
#include "EventQueue/EventManager.h"
#include <memory>

namespace
{
	constexpr const char* kPowerupTexture = "BombermanSprites_Items.png";
	constexpr SDL_FRect kFlamesSourceRect{ 16.0f, 0.0f, 16.0f, 16.0f };
	constexpr SDL_FRect kExtraBombSourceRect{ 0.0f, 0.0f, 16.0f, 16.0f };
	constexpr SDL_FRect kDetonatorSourceRect{ 64.0f, 0.0f, 16.0f, 16.0f };
	constexpr SDL_FRect kSkateSourceRect{ 32.0f, 0.0f, 16.0f, 16.0f };
	constexpr SDL_FRect kDoorSourceRect{ 176.0f, 48.0f, 16.0f, 16.0f };
	constexpr const char* kDoorTexture = "BombermanSprites_General.png";
	constexpr int kPowerupScoreValue = 1000;
	constexpr int kHiddenRenderLayer = 1;
	constexpr int kActiveRenderLayer = 3;
	constexpr int kVulnerabilityFrameDelay = 30;
	constexpr dae::EventId kLevelCompletedEventId = dae::make_sdbm_hash("LevelCompleted");
}

namespace dae
{
	HiddenItemManager::HiddenItemManager(Scene& scene, GameObject& owner, float playfieldScale)
		: m_pScene(&scene)
		, m_pOwner(&owner)
		, m_PlayfieldScale(playfieldScale)
	{}

	HiddenItemManager::~HiddenItemManager()
	{
		ClearItems();
	}

	void HiddenItemManager::PlacePowerup(const glm::vec2& worldPos, PickupType pickupType, GameObject* pBrick)
	{
		m_PickupType = pickupType;
		m_pPowerupBrick = pBrick;
		CreateHiddenPowerup(worldPos, pickupType);
	}

	void HiddenItemManager::PlaceDoor(const glm::vec2& worldPos, GameObject* pBrick)
	{
		m_pDoorBrick = pBrick;
		CreateDoor(worldPos);
	}

	void HiddenItemManager::ClearItems()
	{
		if (m_pPowerupObject && !m_pPowerupObject->IsMarkedForDeletion())
		{
			m_pPowerupObject->MarkForDeletion();
		}
		m_pPowerupObject = nullptr;
		m_pPowerupBrick = nullptr;
		m_PowerupActivated = false;
		m_VulnerabilityDelay = 0;

		if (m_pDoorObject && !m_pDoorObject->IsMarkedForDeletion())
		{
			m_pDoorObject->MarkForDeletion();
		}
		m_pDoorObject = nullptr;
		m_pDoorBrick = nullptr;
		m_DoorActivated = false;
	}

	void HiddenItemManager::OnBrickDestroyed(const GameObject* pBrick, PlayfieldGrid& grid, float tileWorldSize)
	{
		if (!m_PowerupActivated && pBrick == m_pPowerupBrick)
		{
			ActivatePowerup(tileWorldSize);
			m_PowerupActivated = true;
			m_pPowerupBrick = nullptr;
			m_VulnerabilityDelay = kVulnerabilityFrameDelay;
		}

		if (!m_DoorActivated && pBrick == m_pDoorBrick)
		{
			ActivateDoor(&grid, tileWorldSize);
			m_DoorActivated = true;
			m_pDoorBrick = nullptr;
		}
	}

	bool HiddenItemManager::UpdateVulnerabilityDelay()
	{
		if (m_VulnerabilityDelay > 0)
		{
			--m_VulnerabilityDelay;
			return m_VulnerabilityDelay == 0;
		}
		return false;
	}

	void HiddenItemManager::CreateHiddenPowerup(const glm::vec2& worldPos, PickupType pickupType)
	{
		SDL_FRect sourceRect{};
		switch (pickupType)
		{
		case PickupType::Flames:
			sourceRect = kFlamesSourceRect;
			break;
		case PickupType::Bomb:
			sourceRect = kExtraBombSourceRect;
			break;
		case PickupType::RemoteControl:
			sourceRect = kDetonatorSourceRect;
			break;
		case PickupType::Skate:
			sourceRect = kSkateSourceRect;
			break;
		default:
			return;
		}

		auto powerup = std::make_unique<GameObject>();
		auto* transform = powerup->AddComponent<TransformComponent>();
		transform->SetLocalPosition(worldPos.x, worldPos.y, 0.0f);

		auto* render = powerup->AddComponent<RenderComponent>();
		render->SetTexture(kPowerupTexture);
		render->SetSourceRectangle(sourceRect.x, sourceRect.y, sourceRect.w, sourceRect.h);
		render->SetScale(m_PlayfieldScale);
		render->SetPivot({ 0.5f, 0.5f });
		render->SetRenderLayer(kHiddenRenderLayer);

		powerup->SetParent(m_pOwner, false);

		m_pPowerupObject = powerup.get();
		m_pScene->Add(std::move(powerup));
	}

	void HiddenItemManager::ActivatePowerup(float tileWorldSize)
	{
		if (m_pPowerupObject == nullptr)
			return;

		std::unique_ptr<PowerupEffect> pEffect;
		switch (m_PickupType)
		{
		case PickupType::Flames:
			pEffect = std::make_unique<FlamesEffect>();
			break;
		case PickupType::Bomb:
			pEffect = std::make_unique<ExtraBombEffect>();
			break;
		case PickupType::RemoteControl:
			pEffect = std::make_unique<DetonatorEffect>();
			break;
		case PickupType::Skate:
			pEffect = std::make_unique<SkateEffect>();
			break;
		default:
			return;
		}

		auto* render = m_pPowerupObject->GetComponent<RenderComponent>();
		if (render)
		{
			render->SetRenderLayer(kActiveRenderLayer);
		}

		auto* collider = m_pPowerupObject->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize, true);
		collider->SetOffset({ -tileWorldSize * 0.5f, -tileWorldSize * 0.5f });

		m_pPowerupObject->AddComponent<PickupComponent>(kPowerupScoreValue, std::move(pEffect));
		GameObject* pPowerup = m_pPowerupObject;
		collider->SetOnCollisionCallback([pPowerup](GameObject* other)
			{
				if (!pPowerup || pPowerup->IsMarkedForDeletion())
					return;
				if (other && other->HasComponent<BombRangeComponent>())
				{
					if (auto* pup = pPowerup->GetComponent<PickupComponent>())
						pup->OnCollision(other);
				}
			});
	}

	void HiddenItemManager::CreateDoor(const glm::vec2& worldPos)
	{
		auto door = std::make_unique<GameObject>();
		auto* transform = door->AddComponent<TransformComponent>();
		transform->SetLocalPosition(worldPos.x, worldPos.y, 0.0f);

		auto* render = door->AddComponent<RenderComponent>();
		render->SetTexture(kDoorTexture);
		render->SetSourceRectangle(kDoorSourceRect.x, kDoorSourceRect.y, kDoorSourceRect.w, kDoorSourceRect.h);
		render->SetScale(m_PlayfieldScale);
		render->SetPivot({ 0.5f, 0.5f });
		render->SetRenderLayer(kHiddenRenderLayer);

		door->SetParent(m_pOwner, false);
		m_pDoorObject = door.get();
		m_pScene->Add(std::move(door));
	}

	void HiddenItemManager::ActivateDoor(PlayfieldGrid* pGrid, float tileWorldSize)
	{
		if (m_pDoorObject == nullptr)
			return;

		auto* render = m_pDoorObject->GetComponent<RenderComponent>();
		if (render)
			render->SetRenderLayer(kActiveRenderLayer);

		auto* collider = m_pDoorObject->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize, true);
		collider->SetOffset({ -tileWorldSize * 0.5f, -tileWorldSize * 0.5f });
		GameObject* pDoor = m_pDoorObject;
		collider->SetOnCollisionCallback([pDoor, pGrid](GameObject* other)
			{
				if (!pDoor || pDoor->IsMarkedForDeletion())
					return;
				if (other && other->HasComponent<BombRangeComponent>() && pGrid->AreAllEnemiesDead())
				{
					Event levelCompleteEvent(kLevelCompletedEventId);
					EventManager::GetInstance().BroadcastEvent(levelCompleteEvent, other);
				}
			});
	}
}
