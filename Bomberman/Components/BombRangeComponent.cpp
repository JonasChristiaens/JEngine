#include "BombRangeComponent.h"

dae::BombRangeComponent::BombRangeComponent(GameObject* pOwner, int maxRange)
	: BaseComponent(pOwner)
	, m_MaxRange(maxRange)
{}

void dae::BombRangeComponent::IncreaseRange()
{
	if (m_Range < m_MaxRange)
	{
		++m_Range;
	}
}
