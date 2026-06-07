#include "DetonatorEffect.h"
#include "Components/DetonatorComponent.h"
#include "Scene/GameObject.h"

void dae::DetonatorEffect::Apply(GameObject* target)
{
	if (!target)
		return;

	auto* detonator = target->GetComponent<DetonatorComponent>();
	if (detonator)
	{
		detonator->SetHasDetonator(true);
	}
}
