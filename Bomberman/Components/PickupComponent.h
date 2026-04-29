#pragma once
#include "BaseComponent.h"

namespace dae
{
	class PickupComponent final : public BaseComponent
	{
	public:
		PickupComponent(GameObject* pOwner, int scoreValue = 10);
		virtual ~PickupComponent() = default;

		void Update() override {}
		void Render() const override {}

		void OnCollision(GameObject* other);
	private:
		int m_scoreValue;
	};
}
