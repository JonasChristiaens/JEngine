#pragma once
#include "PowerupEffect.h"

namespace dae
{
	class DetonatorEffect final : public PowerupEffect
	{
	public:
		void Apply(GameObject* target) override;
	};
}
