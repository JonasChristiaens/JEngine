#pragma once
#include "BaseComponent.h"

namespace dae
{
	class BombCapacityComponent final : public BaseComponent
	{
	public:
		explicit BombCapacityComponent(GameObject* pOwner, int maxBombs = 1);

		void Update() override {}

		int GetMaxBombs() const { return m_MaxBombs; }
		int GetActiveBombs() const { return m_ActiveBombs; }
		bool CanPlaceBomb() const { return m_ActiveBombs < m_MaxBombs; }
		bool CanIncrease() const { return m_MaxBombs < kAbsoluteMax; }
		void IncreaseCapacity();

		void RegisterBombPlaced();
		void RegisterBombDetonated();

	private:
		static constexpr int kAbsoluteMax{ 9 };

		int m_MaxBombs{ 1 };
		int m_ActiveBombs{ 0 };
	};
}
