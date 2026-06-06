#pragma once
#include "BaseComponent.h"
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace dae
{
	class Scene;

	class PlayfieldComponent final : public BaseComponent
	{
	public:
		struct PlayfieldConfig
		{
			int softBlockCount;
			float softBlockRatio;
			float tileSize;
			std::string softBlockTexture;
			SDL_FRect softBlockSource;
			int softBlockRenderLayer;
			std::vector<glm::ivec2> reservedTiles;

			PlayfieldConfig()
				: softBlockCount(0)
				, softBlockRatio(0.0f)
				, tileSize(16.0f)
				, softBlockTexture("BombermanSprites_Playfield.png")
				, softBlockSource{ 16.0f, 210.0f, 16.0f, 16.0f }
				, softBlockRenderLayer(2)
				, reservedTiles{}
			{
			}
		};

		PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale, PlayfieldConfig config = {});
		virtual ~PlayfieldComponent() = default;

		void Update() override {}
		const std::vector<std::vector<bool>>& GetOccupiedTiles() const { return m_occupiedTiles; }
		void Rebuild(const PlayfieldConfig& config);

	private:
		Scene* m_pScene{};
		float m_playfieldWidth{};
		float m_playfieldHeight{};
		float m_playfieldScale{};
		std::vector<std::vector<bool>> m_occupiedTiles{};
		std::vector<GameObject*> m_spawnedBlocks{};
		PlayfieldConfig m_config{};

		void BuildPlayfield();
		void ClearSpawnedObjects();
		bool IsReservedTile(int column, int row) const;
	};
}
