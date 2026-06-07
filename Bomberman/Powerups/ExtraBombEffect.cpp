#include "ExtraBombEffect.h"
#include "Components/BombCapacityComponent.h"
#include "Scene/GameObject.h"

void dae::ExtraBombEffect::Apply(GameObject* target)
{
	if (!target)
		return;

	auto* capacity = target->GetComponent<BombCapacityComponent>();
	if (capacity && capacity->CanIncrease())
	{
		capacity->IncreaseCapacity();
	}
}
