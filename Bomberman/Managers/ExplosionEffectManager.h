#pragma once
#include <vector>
#include <SDL3/SDL_rect.h>

namespace dae
{
	class Scene;
	class GameObject;

	class ExplosionEffectManager final
	{
	public:
		ExplosionEffectManager(Scene& scene, float tileWorldSize);

		void SpawnExplosion(float centerX, float centerY, int explosionRange, GameObject* bombParent);

	private:
		Scene* m_pScene{};
		float m_TileWorldSize{};

		int ClassifyTile(GameObject* bombParent, float centerX, float centerY);
		std::vector<SDL_FRect> BuildExplosionFrames(int tileColumn, int tileRow) const;
	};
}
