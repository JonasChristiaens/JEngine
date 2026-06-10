#include "EnemyComponent.h"

namespace dae
{
	EnemyComponent::EnemyComponent(GameObject* pOwner, int points)
		: BaseComponent(pOwner)
		, m_Points(points)
	{}
}
