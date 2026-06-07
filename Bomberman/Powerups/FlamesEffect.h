#pragma once
#include "PowerupEffect.h"

namespace dae
{
	class FlamesEffect final : public PowerupEffect
	{
	public:
		void Apply(GameObject* target) override;
	};
}
