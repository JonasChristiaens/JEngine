#include "PlayfieldComponent.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PickupComponent.h"
#include "Components/BombRangeComponent.h"
#include "Powerups/FlamesEffect.h"
#include "Powerups/ExtraBombEffect.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <utility>
#include <vector>

namespace
{
	constexpr const char* kPowerupTexture = "BombermanSprites_Items.png";
	constexpr SDL_FRect kFlamesSourceRect{ 17.0f, 0.0f, 16.0f, 16.0f };
	constexpr SDL_FRect kExtraBombSourceRect{ 0.0f, 0.0f, 16.0f, 16.0f };
	constexpr int kPowerupScoreValue = 0;
	constexpr int kHiddenRenderLayer = 1;
	constexpr int kActiveRenderLayer = 3;
	constexpr int kVulnerabilityFrameDelay = 30;
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
	{
		BuildPlayfield();
	}

	void PlayfieldComponent::Rebuild(const PlayfieldConfig& config)
	{
		m_Config = config;
		BuildPlayfield();
	}

	void PlayfieldComponent::Update()
	{
		if (m_pPowerupBrick == nullptr)
			return;

		if (!m_PowerupActivated && m_pPowerupBrick->IsMarkedForDeletion())
		{
			ActivatePowerup();
			m_PowerupActivated = true;
			m_VulnerabilityDelay = kVulnerabilityFrameDelay;
		}

		if (m_VulnerabilityDelay > 0)
		{
			--m_VulnerabilityDelay;
			if (m_VulnerabilityDelay == 0 && m_pPowerupObject && !m_pPowerupObject->IsMarkedForDeletion())
			{
				m_pPowerupObject->AddComponent<HealthComponent>(1);
			}
		}
	}

	void PlayfieldComponent::BuildPlayfield()
	{
		if (m_pScene == nullptr)
			return;

		ClearSpawnedObjects();
		m_pPowerupBrick = nullptr;
		m_pPowerupObject = nullptr;
		m_PowerupActivated = false;
		m_VulnerabilityDelay = 0;

		const float tileSize = m_Config.tileSize;
		const float tileScale = m_PlayfieldScale;
		const float tileWorldSize = tileSize * tileScale;
		const float gridOriginX = 0.0f;
		const float gridOriginY = 0.0f;
		const int gridColumns = static_cast<int>(std::floor(m_PlayfieldWidth / tileSize));
		const int gridRows = static_cast<int>(std::floor(m_PlayfieldHeight / tileSize));

		m_OccupiedTiles.assign(gridRows, std::vector<bool>(gridColumns, false));

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

				const bool isReserved = IsReservedTile(column, row);
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
			brick->SetParent(GetOwner(), false);
			m_SpawnedBlocks.push_back(brick.get());
			m_pScene->Add(std::move(brick));
			m_OccupiedTiles[row][column] = true;
		}

		if (m_Config.pickupType != PickupType::None && brickCount > 0)
		{
			std::uniform_int_distribution<size_t> dist(0, brickCount - 1);
			const size_t powerupIndex = dist(rng);
			const size_t firstBrickIndex = m_SpawnedBlocks.size() - brickCount;
			m_pPowerupBrick = m_SpawnedBlocks[powerupIndex + firstBrickIndex];

			auto* brickTransform = m_pPowerupBrick->GetComponent<TransformComponent>();
			if (brickTransform)
			{
				const float halfTile = tileWorldSize * 0.5f;
				const auto& pos = brickTransform->GetLocalPosition();
				m_PowerupWorldPos = { pos.x + halfTile, pos.y + halfTile };
				CreateHiddenPowerup();
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

		if (m_pPowerupObject && !m_pPowerupObject->IsMarkedForDeletion())
		{
			m_pPowerupObject->MarkForDeletion();
		}
	}

	bool PlayfieldComponent::IsReservedTile(int column, int row) const
	{
		for (const auto& tile : m_Config.reservedTiles)
		{
			if (tile.x == column && tile.y == row)
			{
				return true;
			}
		}
		return false;
	}

	void PlayfieldComponent::CreateHiddenPowerup()
	{
		SDL_FRect sourceRect{};
		switch (m_Config.pickupType)
		{
		case PickupType::Flames:
			sourceRect = kFlamesSourceRect;
			break;
		case PickupType::Bomb:
			sourceRect = kExtraBombSourceRect;
			break;
		default:
			return;
		}

		auto powerup = std::make_unique<GameObject>();
		auto* transform = powerup->AddComponent<TransformComponent>();
		transform->SetLocalPosition(m_PowerupWorldPos.x, m_PowerupWorldPos.y, 0.0f);

		auto* render = powerup->AddComponent<RenderComponent>();
		render->SetTexture(kPowerupTexture);
		render->SetSourceRectangle(sourceRect.x, sourceRect.y, sourceRect.w, sourceRect.h);
		render->SetScale(m_PlayfieldScale);
		render->SetPivot({ 0.5f, 0.5f });
		render->SetRenderLayer(kHiddenRenderLayer);

		powerup->SetParent(GetOwner(), false);

		m_pPowerupObject = powerup.get();
		m_pScene->Add(std::move(powerup));
	}

	void PlayfieldComponent::ActivatePowerup()
	{
		if (m_pPowerupObject == nullptr)
			return;

		std::unique_ptr<PowerupEffect> pEffect;
		switch (m_Config.pickupType)
		{
		case PickupType::Flames:
			pEffect = std::make_unique<FlamesEffect>();
			break;
		case PickupType::Bomb:
			pEffect = std::make_unique<ExtraBombEffect>();
			break;
		default:
			return;
		}

		auto* render = m_pPowerupObject->GetComponent<RenderComponent>();
		if (render)
		{
			render->SetRenderLayer(kActiveRenderLayer);
		}

		const float tileWorldSize = m_Config.tileSize * m_PlayfieldScale;
		auto* collider = m_pPowerupObject->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize, true);
		collider->SetOffset({ -tileWorldSize * 0.5f, -tileWorldSize * 0.5f });

		auto* pickup = m_pPowerupObject->AddComponent<PickupComponent>(kPowerupScoreValue, std::move(pEffect));
		collider->SetOnCollisionCallback([pickup](GameObject* other)
			{
				if (other && other->HasComponent<BombRangeComponent>())
				{
					pickup->OnCollision(other);
				}
			});
	}
}
