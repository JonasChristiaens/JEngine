#pragma once
#include "BaseComponent.h"
#include "EventQueue/IObserver.h"
#include "Level/LevelData.h"
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

namespace dae
{
	class Scene;

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
			{
			}
		};

		PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale, PlayfieldConfig config = {});
		~PlayfieldComponent() override;

		void Update() override;
		const std::vector<std::vector<bool>>& GetOccupiedTiles() const { return m_OccupiedTiles; }
		void Rebuild(const PlayfieldConfig& config);
		void RegisterEnemySpawned();
		void Notify(GameObject& actor, Event event) override;
		bool AreAllEnemiesDead() const { return m_AliveEnemyCount <= 0; }

	private:
		Scene* m_pScene{};
		float m_PlayfieldWidth{};
		float m_PlayfieldHeight{};
		float m_PlayfieldScale{};
		std::vector<std::vector<bool>> m_OccupiedTiles{};
		std::vector<GameObject*> m_SpawnedBlocks{};
		PlayfieldConfig m_Config{};
		GameObject* m_pPowerupBrick{ nullptr };
		GameObject* m_pPowerupObject{ nullptr };
		glm::vec2 m_PowerupWorldPos{};
		bool m_PowerupActivated{ false };
		int m_VulnerabilityDelay{ 0 };

		GameObject* m_pDoorBrick{ nullptr };
		GameObject* m_pDoorObject{ nullptr };
		glm::vec2 m_DoorWorldPos{};
		bool m_DoorActivated{ false };

		int m_AliveEnemyCount{ 0 };

		void BuildPlayfield();
		void ClearSpawnedObjects();
		bool IsReservedTile(int column, int row) const;
		void CreateHiddenPowerup();
		void ActivatePowerup();
		void CreateDoor();
		void ActivateDoor();
	};
}
