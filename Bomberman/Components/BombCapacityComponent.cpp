#include "BombCapacityComponent.h"

dae::BombCapacityComponent::BombCapacityComponent(GameObject* pOwner, int maxBombs)
	: BaseComponent(pOwner)
	, m_MaxBombs(maxBombs)
{
}

void dae::BombCapacityComponent::IncreaseCapacity()
{
	if (m_MaxBombs < kAbsoluteMax)
	{
		++m_MaxBombs;
	}
}

void dae::BombCapacityComponent::RegisterBombPlaced()
{
	++m_ActiveBombs;
}

void dae::BombCapacityComponent::RegisterBombDetonated()
{
	if (m_ActiveBombs > 0)
	{
		--m_ActiveBombs;
	}
}
