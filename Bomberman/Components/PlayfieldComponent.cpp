#include "PlayfieldComponent.h"
#include "Managers/HiddenItemManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Components/DeathAnimatorComponent.h"
#include "Components/EnemyComponent.h"
#include "EventQueue/EventManager.h"
#include "Config/EnemyConfig.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <utility>
#include <vector>

namespace
{
	constexpr dae::EventId kEntityDiedEventId = dae::make_sdbm_hash("EntityDied");
	constexpr dae::EventId kTookDamageEventId = dae::make_sdbm_hash("TookDamageEvent");
	constexpr dae::EventId kChangeHealthEventId = dae::make_sdbm_hash("ChangeHealthEvent");
}

namespace dae
{
	PlayfieldComponent::PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale, PlayfieldConfig config)
		: BaseComponent(pOwner)
		, m_pScene(&scene)
		, m_PlayfieldWidth(playfieldWidth)
		, m_PlayfieldHeight(playfieldHeight)
		, m_PlayfieldScale(playfieldScale)
		, m_Config(std::move(config))
		, m_pItems(std::make_unique<HiddenItemManager>(scene, *pOwner, playfieldScale))
	{
		EventManager::GetInstance().AddObserver(*this);
		BuildPlayfield();
	}

	PlayfieldComponent::~PlayfieldComponent()
	{
		if (EventManager::IsAlive())
			EventManager::GetInstance().RemoveObserver(*this);
	}

	void PlayfieldComponent::Rebuild(const PlayfieldConfig& config)
	{
		m_Config = config;
		BuildPlayfield();
	}

	void PlayfieldComponent::Update()
	{
		if (m_pItems->UpdateVulnerabilityDelay() && m_pItems->GetPowerupObject() && !m_pItems->GetPowerupObject()->IsMarkedForDeletion())
		{
			m_pItems->GetPowerupObject()->AddComponent<HealthComponent>(1);
		}
	}

	void PlayfieldComponent::BuildPlayfield()
	{
		if (m_pScene == nullptr)
			return;

		ClearSpawnedObjects();
		m_pItems->ClearItems();

		const float tileSize = m_Config.tileSize;
		const float tileScale = m_PlayfieldScale;
		const float tileWorldSize = tileSize * tileScale;
		const float gridOriginX = 0.0f;
		const float gridOriginY = 0.0f;
		const int gridColumns = static_cast<int>(std::floor(m_PlayfieldWidth / tileSize));
		const int gridRows = static_cast<int>(std::floor(m_PlayfieldHeight / tileSize));

		m_Grid.Initialize(gridColumns, gridRows);

		auto createSolidCollider = [&](int column, int row)
			{
				auto block = std::make_unique<GameObject>();
				auto* blockTransform = block->AddComponent<TransformComponent>();
				blockTransform->SetLocalPosition(
					gridOriginX + column * tileWorldSize,
					gridOriginY + row * tileWorldSize,
					0.0f
				);
				block->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize);
				block->SetParent(GetOwner(), false);
				m_SpawnedBlocks.push_back(block.get());
				m_pScene->Add(std::move(block));
			};

		std::vector<std::pair<int, int>> brickCandidates{};
		brickCandidates.reserve(static_cast<size_t>(gridColumns * gridRows));

		for (int row = 0; row < gridRows; ++row)
		{
			for (int column = 0; column < gridColumns; ++column)
			{
				const bool isBorder = row == 0 || column == 0 || row == gridRows - 1 || column == gridColumns - 1;
				const bool isPillar = !isBorder && (row % 2 == 0) && (column % 2 == 0);

				const bool isReserved = m_Grid.IsReservedTile(column, row, m_Config.reservedTiles);
				if (isBorder || isPillar)
				{
					createSolidCollider(column, row);
				}
				else if (!isReserved)
				{
					brickCandidates.emplace_back(column, row);
				}
			}
		}

		std::random_device randomDevice{};
		std::mt19937 rng(randomDevice());
		std::shuffle(brickCandidates.begin(), brickCandidates.end(), rng);
		const float brickRatio = m_Config.softBlockRatio;
		const size_t brickCount = (m_Config.softBlockCount > 0)
			? std::min(static_cast<size_t>(m_Config.softBlockCount), brickCandidates.size())
			: static_cast<size_t>(brickCandidates.size() * brickRatio);

		for (size_t i = 0; i < brickCount; ++i)
		{
			const auto [column, row] = brickCandidates[i];
			auto brick = std::make_unique<GameObject>();
			auto* brickTransform = brick->AddComponent<TransformComponent>();
			brickTransform->SetLocalPosition(
				gridOriginX + column * tileWorldSize,
				gridOriginY + row * tileWorldSize,
				0.0f
			);
			auto* brickRender = brick->AddComponent<RenderComponent>();
			brickRender->SetTexture(m_Config.softBlockTexture);
			brickRender->SetSourceRectangle(m_Config.softBlockSource.x, m_Config.softBlockSource.y, m_Config.softBlockSource.w, m_Config.softBlockSource.h);
			brickRender->SetScale(tileScale);
			brickRender->SetRenderLayer(m_Config.softBlockRenderLayer);
			brick->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize);
			brick->AddComponent<HealthComponent>(1);
			brick->AddComponent<DeathAnimatorComponent>("BombermanSprites_General.png", BuildHorizontalFrames(80.0f, 48.0f, 6, 16.0f, 16.0f), 10.0f, tileScale);
			brick->SetParent(GetOwner(), false);
			m_SpawnedBlocks.push_back(brick.get());
			m_pScene->Add(std::move(brick));
			m_Grid.SetOccupied(row, column, true);
		}

		if (m_Config.pickupType != PickupType::None && brickCount > 0)
		{
			std::uniform_int_distribution<size_t> dist(0, brickCount - 1);
			const size_t powerupIndex = dist(rng);
			const size_t firstBrickIndex = m_SpawnedBlocks.size() - brickCount;

			auto* pBrick = m_SpawnedBlocks[powerupIndex + firstBrickIndex];
			auto* brickTransform = pBrick->GetComponent<TransformComponent>();
			if (brickTransform)
			{
				const float halfTile = tileWorldSize * 0.5f;
				const auto& pos = brickTransform->GetLocalPosition();
				m_pItems->PlacePowerup({ pos.x + halfTile, pos.y + halfTile }, m_Config.pickupType, pBrick);
			}

			if (brickCount > 1)
			{
				size_t doorIndex = powerupIndex;
				while (doorIndex == powerupIndex)
					doorIndex = dist(rng);

				auto* pDoorBrick = m_SpawnedBlocks[doorIndex + firstBrickIndex];
				auto* doorBrickTransform = pDoorBrick->GetComponent<TransformComponent>();
				if (doorBrickTransform)
				{
					const float halfTile = tileWorldSize * 0.5f;
					const auto& pos = doorBrickTransform->GetLocalPosition();
					m_pItems->PlaceDoor({ pos.x + halfTile, pos.y + halfTile }, pDoorBrick);
				}
			}
		}
	}

	void PlayfieldComponent::ClearSpawnedObjects()
	{
		for (auto* block : m_SpawnedBlocks)
		{
			if (block && !block->IsMarkedForDeletion())
			{
				block->MarkForDeletion();
			}
		}
		m_SpawnedBlocks.clear();
	}

	void PlayfieldComponent::Notify(GameObject& actor, Event event)
	{
		if (event.id == kEntityDiedEventId)
		{
			auto* collider = actor.GetComponent<CollisionComponent>();
			auto* tx = actor.GetComponent<TransformComponent>();
			if (collider && !collider->IsTrigger() && tx)
			{
				const auto& localPos = tx->GetLocalPosition();
				ClearOccupiedTile(localPos.x, localPos.y);
			}

			const float tileWorldSize = m_Config.tileSize * m_PlayfieldScale;
			m_pItems->OnBrickDestroyed(&actor, m_Grid, tileWorldSize);

			if (actor.HasComponent<EnemyComponent>())
				m_Grid.DecrementEnemyCount();
		}

		if (event.id == kChangeHealthEventId || event.id == kEntityDiedEventId || event.id == kTookDamageEventId)
		{
			auto* collider = actor.GetComponent<CollisionComponent>();
			auto* health = actor.GetComponent<HealthComponent>();
			auto* tx = actor.GetComponent<TransformComponent>();
			if (collider && !collider->IsTrigger() && health && health->GetHealth() <= 0 && tx)
			{
				const auto& localPos = tx->GetLocalPosition();
				ClearOccupiedTile(localPos.x, localPos.y);
			}
		}
	}
}
