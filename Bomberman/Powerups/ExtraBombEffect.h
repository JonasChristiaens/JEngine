#pragma once
#include "PowerupEffect.h"

namespace dae
{
	class ExtraBombEffect final : public PowerupEffect
	{
	public:
		void Apply(GameObject* target) override;
	};
}
