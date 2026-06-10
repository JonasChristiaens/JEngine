#pragma once
#include "PowerupEffect.h"

namespace dae
{
	class SkateEffect final : public PowerupEffect
	{
	public:
		void Apply(GameObject* target) override;
	};
}
