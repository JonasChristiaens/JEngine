#pragma once
#include <vector>
#include <SDL3/SDL_rect.h>

namespace dae
{
	class Scene;
	class GameObject;
	class PlayfieldComponent;

	class ExplosionEffectManager final
	{
	public:
		ExplosionEffectManager(Scene& scene, float tileWorldSize);

		void SpawnExplosion(float centerX, float centerY, int explosionRange, GameObject* bombParent, GameObject* pBombOwner = nullptr);

	private:
		Scene* m_pScene{};
		float m_TileWorldSize{};

		static int ClassifyTile(const PlayfieldComponent& playfield, float tileWorldSize, float x, float y);
		std::vector<SDL_FRect> BuildExplosionFrames(int tileColumn, int tileRow) const;
	};
}
