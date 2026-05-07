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

		void Notify(const GameObject& pGameActor, Event event) override;

	private:
		Scene* m_pScene{};
		float m_TileWorldSize{ 0.0f };
		int m_ExplosionRange{ 1 };
	};
}