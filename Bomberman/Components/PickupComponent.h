#pragma once
#include "BaseComponent.h"
#include <memory>

namespace dae
{
	class PowerupEffect;

	class PickupComponent final : public BaseComponent
	{
	public:
		PickupComponent(GameObject* pOwner, int scoreValue = 10, std::unique_ptr<PowerupEffect> pEffect = nullptr);
		virtual ~PickupComponent() = default;

		void Update() override {}
		void Render() const override {}

		void OnCollision(GameObject* other);
	private:
		int m_ScoreValue;
		std::unique_ptr<PowerupEffect> m_pEffect;
	};
}
