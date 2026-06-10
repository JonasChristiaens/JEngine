#include "SkateEffect.h"
#include "Components/SkateComponent.h"
#include "Scene/GameObject.h"

void dae::SkateEffect::Apply(GameObject* target)
{
	if (!target)
		return;

	auto* skate = target->GetComponent<SkateComponent>();
	if (skate)
	{
		skate->SetHasSkate(true);
	}
}
