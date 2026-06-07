#pragma once
#include "BaseComponent.h"

namespace dae
{
	class BombRangeComponent final : public BaseComponent
	{
	public:
		explicit BombRangeComponent(GameObject* pOwner, int maxRange = 4);

		void Update() override {}

		int GetRange() const { return m_Range; }
		int GetMaxRange() const { return m_MaxRange; }
		bool CanIncrease() const { return m_Range < m_MaxRange; }
		void IncreaseRange();

	private:
		int m_Range{ 1 };
		int m_MaxRange{ 4 };
	};
}
