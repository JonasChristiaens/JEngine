#pragma once
#include "BaseComponent.h"

namespace dae
{
	class EnemyComponent final : public BaseComponent
	{
	public:
		EnemyComponent(GameObject* pOwner, int points);

		void Update() override {}

		int GetPoints() const { return m_Points; }

	private:
		int m_Points{};
	};
}
