#pragma once

namespace dae
{
	class GameObject;

	class PowerupEffect
	{
	public:
		virtual ~PowerupEffect() = default;
		virtual void Apply(GameObject* target) = 0;
	};
}
