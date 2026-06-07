#pragma once
#include "BaseComponent.h"

namespace dae
{
	class BombComponent final : public BaseComponent
	{
	public:
		BombComponent(GameObject* pOwnerBomb, GameObject* pOwnerPlayer, float tileSize, int explosionRange = 1);

		void Update() override;
		int GetExplosionRange() const { return m_ExplosionRange; }
		GameObject* GetOwnerPlayer() const { return m_pOwnerPlayer; }

	private:
		GameObject* m_pOwnerPlayer;
		float m_TileSize;
		int m_ExplosionRange;
		bool m_HasAddedCollision{};

		bool OwnerOverlapsBomb() const;
		void AddSolidCollider();
	};
}
