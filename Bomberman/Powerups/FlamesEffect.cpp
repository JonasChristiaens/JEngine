#include "FlamesEffect.h"
#include "Components/BombRangeComponent.h"
#include "Scene/GameObject.h"

void dae::FlamesEffect::Apply(GameObject* target)
{
	if (!target)
		return;

	auto* bombRange = target->GetComponent<BombRangeComponent>();
	if (bombRange && bombRange->CanIncrease())
	{
		bombRange->IncreaseRange();
	}
}
