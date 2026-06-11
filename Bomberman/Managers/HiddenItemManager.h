#pragma once
#include "Level/LevelData.h"
#include <glm/vec2.hpp>

namespace dae
{
	class Scene;
	class GameObject;
	class PlayfieldGrid;

	class HiddenItemManager final
	{
	public:
		HiddenItemManager(Scene& scene, GameObject& owner, float playfieldScale);
		~HiddenItemManager();

		void PlacePowerup(const glm::vec2& worldPos, PickupType pickupType, GameObject* pBrick);
		void PlaceDoor(const glm::vec2& worldPos, GameObject* pBrick);
		void ClearItems();

		bool IsPowerupBrick(const GameObject* pObj) const { return pObj == m_pPowerupBrick; }
		bool IsDoorBrick(const GameObject* pObj) const { return pObj == m_pDoorBrick; }
		bool IsPowerupActivated() const { return m_PowerupActivated; }
		bool IsDoorActivated() const { return m_DoorActivated; }

		void OnBrickDestroyed(const GameObject* pBrick, PlayfieldGrid& grid, float tileWorldSize);
		bool UpdateVulnerabilityDelay();
		GameObject* GetPowerupObject() const { return m_pPowerupObject; }

	private:
		Scene* m_pScene;
		GameObject* m_pOwner;
		float m_PlayfieldScale;

		GameObject* m_pPowerupBrick{ nullptr };
		GameObject* m_pPowerupObject{ nullptr };
		bool m_PowerupActivated{ false };
		int m_VulnerabilityDelay{ 0 };
		PickupType m_PickupType{ PickupType::None };

		GameObject* m_pDoorBrick{ nullptr };
		GameObject* m_pDoorObject{ nullptr };
		bool m_DoorActivated{ false };

		void CreateHiddenPowerup(const glm::vec2& worldPos, PickupType pickupType);
		void ActivatePowerup(float tileWorldSize);
		void CreateDoor(const glm::vec2& worldPos);
		void ActivateDoor(PlayfieldGrid* pGrid, float tileWorldSize);
	};
}
