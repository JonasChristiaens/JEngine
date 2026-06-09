#include "ExplosionEffectManager.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/EnemyComponent.h"
#include "Components/PlayfieldComponent.h"
#include "EventQueue/EventManager.h"

namespace
{
	constexpr float kExplosionFrameSize = 80.0f;
	constexpr float kExplosionTileSize = 16.0f;
	constexpr float kExplosionStartX = 0.0f;
	constexpr float kExplosionStartY = 64.0f;
	constexpr int kExplosionFrameColumns = 2;
	constexpr int kExplosionFrameCount = 4;
	constexpr float kExplosionFramesPerSecond = 12.0f;
	constexpr float kDamageColliderScale = 0.7f;
}

namespace dae
{
	ExplosionEffectManager::ExplosionEffectManager(Scene& scene, float tileWorldSize)
		: m_pScene(&scene)
		, m_TileWorldSize(tileWorldSize)
	{
	}

	int ExplosionEffectManager::ClassifyTile(const PlayfieldComponent& playfield, float tileWorldSize, float x, float y)
	{
		const int col = static_cast<int>(x / tileWorldSize);
		const int row = static_cast<int>(y / tileWorldSize);
		const int gridCols = static_cast<int>(496.0f / 16.0f);
		const int gridRows = static_cast<int>(208.0f / 16.0f);

		if (row < 0 || row >= gridRows || col < 0 || col >= gridCols)
			return 2;

		const bool isBorder = row == 0 || col == 0 || row == gridRows - 1 || col == gridCols - 1;
		const bool isPillar = (row % 2 == 0) && (col % 2 == 0);
		if (isBorder || isPillar)
			return 2;

		const auto& occupied = playfield.GetOccupiedTiles();
		if (static_cast<size_t>(row) < occupied.size() && static_cast<size_t>(col) < occupied[row].size())
		{
			if (occupied[row][col])
				return 1;
		}

		return 0;
	}

	std::vector<SDL_FRect> ExplosionEffectManager::BuildExplosionFrames(int tileColumn, int tileRow) const
	{
		std::vector<SDL_FRect> frames{};
		frames.reserve(kExplosionFrameCount);
		for (int frameIndex = 0; frameIndex < kExplosionFrameCount; ++frameIndex)
		{
			const int frameColumn = frameIndex % kExplosionFrameColumns;
			const int frameRow = frameIndex / kExplosionFrameColumns;
			const float originX = kExplosionStartX + static_cast<float>(frameColumn) * kExplosionFrameSize;
			const float originY = kExplosionStartY + static_cast<float>(frameRow) * kExplosionFrameSize;
			frames.push_back(SDL_FRect{
				originX + static_cast<float>(tileColumn) * kExplosionTileSize,
				originY + static_cast<float>(tileRow) * kExplosionTileSize,
				kExplosionTileSize,
				kExplosionTileSize
				});
		}
		return frames;
	}

	void ExplosionEffectManager::SpawnExplosion(float centerX, float centerY, int explosionRange, GameObject* bombParent, GameObject* pBombOwner)
	{
		PlayfieldComponent* playfield = bombParent ? bombParent->GetComponent<PlayfieldComponent>() : nullptr;

		std::vector<GameObject*> damageTiles{};
		damageTiles.reserve(static_cast<size_t>(1 + explosionRange * 4));

		std::vector<GameObject*> explosionVisuals{};
		explosionVisuals.reserve(static_cast<size_t>(1 + explosionRange * 4));

		auto addDamageTile = [&](float posX, float posY)
		{
			auto damageTile = std::make_unique<GameObject>();
			auto* transform = damageTile->AddComponent<TransformComponent>();
			transform->SetLocalPosition(posX, posY, 0.0f);

			const float damageSize = m_TileWorldSize * kDamageColliderScale;
			auto* collider = damageTile->AddComponent<CollisionComponent>(damageSize, damageSize, true);
			collider->SetOffset({ -damageSize * 0.5f, -damageSize * 0.5f });
			collider->SetOnCollisionCallback([pBombOwner](GameObject* other)
			{
				if (!other || other->IsMarkedForDeletion())
					return;

				auto* health = other->GetComponent<HealthComponent>();
				if (!health)
					return;

				Event damageEvent(make_sdbm_hash("ChangeHealthEvent"));
				damageEvent.nbArgs = 1;
				damageEvent.args[0].i = -1;
				EventManager::GetInstance().BroadcastEvent(damageEvent, other);

				if (health->GetHealth() > 0 && health->GetHealth() <= 1 && pBombOwner && other->HasComponent<EnemyComponent>())
				{
					auto* enemyComp = other->GetComponent<EnemyComponent>();
					if (enemyComp)
					{
						Event scoreEvent(make_sdbm_hash("ChangeScoreEvent"));
						scoreEvent.nbArgs = 1;
						scoreEvent.args[0].i = enemyComp->GetPoints();
						EventManager::GetInstance().BroadcastEvent(scoreEvent, pBombOwner);
					}
				}
			});

			if (bombParent)
			{
				damageTile->SetParent(bombParent, false);
			}
			damageTiles.push_back(damageTile.get());
			m_pScene->Add(std::move(damageTile));
		};

		auto addExplosionTile = [&](float posX, float posY, int tileColumn, int tileRow) -> SpriteAnimatorComponent*
		{
			auto tile = std::make_unique<GameObject>();
			auto* transform = tile->AddComponent<TransformComponent>();
			transform->SetLocalPosition(posX, posY, 0.0f);

			auto* render = tile->AddComponent<RenderComponent>();
			render->SetTexture("BombermanSprites_General.png");
			render->SetScale(m_TileWorldSize / kExplosionTileSize);
			render->SetPivot({ 0.5f, 0.5f });
			render->SetRenderLayer(2);

			auto* animator = tile->AddComponent<SpriteAnimatorComponent>();
			animator->SetAnimation(BuildExplosionFrames(tileColumn, tileRow), kExplosionFramesPerSecond, false);

			if (bombParent)
			{
				tile->SetParent(bombParent, false);
			}

			auto* tilePtr = tile.get();
			explosionVisuals.push_back(tilePtr);
			m_pScene->Add(std::move(tile));
			return animator;
		};

		auto classify = [&](float x, float y)
		{
			return playfield ? ClassifyTile(*playfield, m_TileWorldSize, x, y) : 0;
		};

		addDamageTile(centerX, centerY);
		auto* centerAnimator = addExplosionTile(centerX, centerY, 2, 2);

		bool leftOpen = true, rightOpen = true, upOpen = true, downOpen = true;

		for (int step = 1; step <= explosionRange; ++step)
		{
			const float offset = static_cast<float>(step) * m_TileWorldSize;

			if (leftOpen)
			{
				const float posX = centerX - offset;
				const int blockType = classify(posX, centerY);
				if (blockType == 2) { leftOpen = false; }
				else if (blockType == 1)
				{
					addDamageTile(posX, centerY);
					leftOpen = false;
				}
				else
				{
					addDamageTile(posX, centerY);
					const bool isEnd = (step == explosionRange) || (classify(posX - m_TileWorldSize, centerY) != 0);
					addExplosionTile(posX, centerY, isEnd ? 0 : 1, 2);
				}
			}

			if (rightOpen)
			{
				const float posX = centerX + offset;
				const int blockType = classify(posX, centerY);
				if (blockType == 2) { rightOpen = false; }
				else if (blockType == 1)
				{
					addDamageTile(posX, centerY);
					rightOpen = false;
				}
				else
				{
					addDamageTile(posX, centerY);
					const bool isEnd = (step == explosionRange) || (classify(posX + m_TileWorldSize, centerY) != 0);
					addExplosionTile(posX, centerY, isEnd ? 4 : 3, 2);
				}
			}

			if (upOpen)
			{
				const float posY = centerY - offset;
				const int blockType = classify(centerX, posY);
				if (blockType == 2) { upOpen = false; }
				else if (blockType == 1)
				{
					addDamageTile(centerX, posY);
					upOpen = false;
				}
				else
				{
					addDamageTile(centerX, posY);
					const bool isEnd = (step == explosionRange) || (classify(centerX, posY - m_TileWorldSize) != 0);
					addExplosionTile(centerX, posY, 2, isEnd ? 0 : 1);
				}
			}

			if (downOpen)
			{
				const float posY = centerY + offset;
				const int blockType = classify(centerX, posY);
				if (blockType == 2) { downOpen = false; }
				else if (blockType == 1)
				{
					addDamageTile(centerX, posY);
					downOpen = false;
				}
				else
				{
					addDamageTile(centerX, posY);
					const bool isEnd = (step == explosionRange) || (classify(centerX, posY + m_TileWorldSize) != 0);
					addExplosionTile(centerX, posY, 2, isEnd ? 4 : 3);
				}
			}
		}

		if (centerAnimator)
		{
			centerAnimator->SetOnAnimationFinishedCallback(
				[tiles = std::move(damageTiles), visuals = std::move(explosionVisuals)]() mutable
				{
					for (auto* visual : visuals)
					{
						if (visual)
						{
							visual->MarkForDeletion();
						}
					}
					for (auto* tile : tiles)
					{
						if (tile)
						{
							tile->MarkForDeletion();
						}
					}
				});
		}
	}
}
