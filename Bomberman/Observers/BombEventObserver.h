#pragma once
#include "EventQueue/IObserver.h"

namespace dae
{
	class Scene;

	class BombEventObserver final : public IObserver
	{
	public:
		explicit BombEventObserver(Scene& scene, float tileWorldSize);
		~BombEventObserver() override;

		void Notify(GameObject& actor, Event event) override;

	private:
		static constexpr int kDefaultExplosionRange{ 1 };

		Scene* m_pScene{};
		float m_TileWorldSize{ 0.0f };
	};
}