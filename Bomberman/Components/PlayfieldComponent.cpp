#include "PlayfieldComponent.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include <limits>
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace dae
{
 PlayfieldComponent::PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale, PlayfieldConfig config)
		: BaseComponent(pOwner)
		, m_pScene(&scene)
		, m_playfieldWidth(playfieldWidth)
		, m_playfieldHeight(playfieldHeight)
		, m_playfieldScale(playfieldScale)
       , m_config(std::move(config))
	{
		BuildPlayfield();
	}

	void PlayfieldComponent::Rebuild(const PlayfieldConfig& config)
	{
		m_config = config;
		BuildPlayfield();
	}

	void PlayfieldComponent::BuildPlayfield()
	{
		if (m_pScene == nullptr)
			return;

       ClearSpawnedObjects();

		const float tileSize = m_config.tileSize;
		const float tileScale = m_playfieldScale;
		const float tileWorldSize = tileSize * tileScale;
		const float gridOriginX = 0.0f;
		const float gridOriginY = 0.0f;
      const int gridColumns = static_cast<int>(std::floor(m_playfieldWidth / tileSize));
		const int gridRows = static_cast<int>(std::floor(m_playfieldHeight / tileSize));

		m_occupiedTiles.assign(gridRows, std::vector<bool>(gridColumns, false));

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
                m_spawnedBlocks.push_back(block.get());
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
     const float brickRatio = m_config.softBlockRatio;
		const size_t brickCount = (m_config.softBlockCount > 0)
			? std::min(static_cast<size_t>(m_config.softBlockCount), brickCandidates.size())
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
			brickRender->SetTexture(m_config.softBlockTexture);
			brickRender->SetSourceRectangle(m_config.softBlockSource.x, m_config.softBlockSource.y, m_config.softBlockSource.w, m_config.softBlockSource.h);
			brickRender->SetScale(tileScale);
			brickRender->SetRenderLayer(m_config.softBlockRenderLayer);
			brick->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize);
			brick->AddComponent<HealthComponent>(1);
			brick->SetParent(GetOwner(), false);
            m_spawnedBlocks.push_back(brick.get());
			m_pScene->Add(std::move(brick));
			m_occupiedTiles[row][column] = true;
		}
	}

	void PlayfieldComponent::ClearSpawnedObjects()
	{
		for (auto* block : m_spawnedBlocks)
		{
			if (block && !block->IsMarkedForDeletion())
			{
				block->MarkForDeletion();
			}
		}
		m_spawnedBlocks.clear();
	}

	bool PlayfieldComponent::IsReservedTile(int column, int row) const
	{
		for (const auto& tile : m_config.reservedTiles)
		{
			if (tile.x == column && tile.y == row)
			{
				return true;
			}
		}
		return false;
	}
}
