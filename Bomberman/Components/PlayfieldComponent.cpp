#include "PlayfieldComponent.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RenderComponent.h"
#include "Components/TransformComponent.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace dae
{
	PlayfieldComponent::PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale)
		: BaseComponent(pOwner)
		, m_pScene(&scene)
		, m_playfieldWidth(playfieldWidth)
		, m_playfieldHeight(playfieldHeight)
		, m_playfieldScale(playfieldScale)
	{
		BuildPlayfield();
	}

	void PlayfieldComponent::BuildPlayfield()
	{
		if (m_pScene == nullptr)
			return;

		constexpr float tileSize = 16.0f;
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
				m_pScene->Add(std::move(block));
			};

		std::vector<std::pair<int, int>> brickCandidates{};
		brickCandidates.reserve(static_cast<size_t>(gridColumns * gridRows));
		const int safeSpawnColumn = 1;
		const int safeSpawnRow = 1;

		for (int row = 0; row < gridRows; ++row)
		{
			for (int column = 0; column < gridColumns; ++column)
			{
				const bool isBorder = row == 0 || column == 0 || row == gridRows - 1 || column == gridColumns - 1;
				const bool isPillar = !isBorder && (row % 2 == 0) && (column % 2 == 0);

				const bool isSafeSpawn = column == safeSpawnColumn && row == safeSpawnRow;
				if (isBorder || isPillar)
				{
					createSolidCollider(column, row);
				}
				else if (!isSafeSpawn)
				{
					brickCandidates.emplace_back(column, row);
				}
			}
		}

		std::random_device randomDevice{};
		std::mt19937 rng(randomDevice());
		std::shuffle(brickCandidates.begin(), brickCandidates.end(), rng);
		const float brickRatio = 0.28f;
		const size_t brickCount = static_cast<size_t>(brickCandidates.size() * brickRatio);

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
			brickRender->SetTexture("Textures/BombermanSprites_Playfield.png");
			brickRender->SetSourceRectangle(16.0f, 210.0f, tileSize, tileSize);
			brickRender->SetScale(tileScale);
			brickRender->SetRenderLayer(2);
			brick->AddComponent<CollisionComponent>(tileWorldSize, tileWorldSize);
			brick->AddComponent<HealthComponent>(1);
			brick->SetParent(GetOwner(), false);
			m_pScene->Add(std::move(brick));
			m_occupiedTiles[row][column] = true;
		}
	}
}
