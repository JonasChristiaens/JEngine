#pragma once
#include "BaseComponent.h"

namespace dae
{
	class BombComponent final : public BaseComponent
	{
	public:
		BombComponent(GameObject* pOwnerBomb, GameObject* pOwnerPlayer, float tileSize);

		void Update() override;

	private:
		GameObject* m_pOwnerPlayer;
		float m_TileSize;
		bool m_HasAddedCollision{};

		bool OwnerOverlapsBomb() const;
		void AddSolidCollider();
	};
}
