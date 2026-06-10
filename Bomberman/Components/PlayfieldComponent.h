#pragma once
#include "BaseComponent.h"
#include "EventQueue/IObserver.h"
#include "Level/LevelData.h"
#include "PlayfieldGrid.h"
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace dae
{
	class Scene;
	class HiddenItemManager;

	class PlayfieldComponent final : public BaseComponent, public IObserver
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
			PickupType pickupType;

			PlayfieldConfig()
				: softBlockCount(0)
				, softBlockRatio(0.0f)
				, tileSize(16.0f)
				, softBlockTexture("BombermanSprites_General.png")
				, softBlockSource{ 64.0f, 48.0f, 16.0f, 16.0f }
				, softBlockRenderLayer(2)
				, reservedTiles{}
				, pickupType(PickupType::None)
			{}
		};

		PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale, PlayfieldConfig config = {});
		~PlayfieldComponent() override;

		void Update() override;
		const std::vector<std::vector<bool>>& GetOccupiedTiles() const { return m_Grid.GetGrid(); }
		void Rebuild(const PlayfieldConfig& config);
		void RegisterEnemySpawned() { m_Grid.RegisterEnemySpawned(); }
		void ClearOccupiedTile(float localX, float localY) { m_Grid.ClearTile(localX, localY, m_PlayfieldScale * m_Config.tileSize); }
		void Notify(GameObject& actor, Event event) override;
		bool AreAllEnemiesDead() const { return m_Grid.AreAllEnemiesDead(); }

	private:
		Scene* m_pScene{};
		float m_PlayfieldWidth{};
		float m_PlayfieldHeight{};
		float m_PlayfieldScale{};
		PlayfieldGrid m_Grid{};
		std::vector<GameObject*> m_SpawnedBlocks{};
		PlayfieldConfig m_Config{};
		std::unique_ptr<HiddenItemManager> m_pItems{};

		void BuildPlayfield();
		void ClearSpawnedObjects();
	};
}
