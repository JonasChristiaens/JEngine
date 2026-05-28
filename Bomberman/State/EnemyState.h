#pragma once
#include "State/State.h"

namespace dae
{
	class GameObject;

	class EnemyState : public State
	{
	public:
		explicit EnemyState(GameObject* owner)
			: m_pOwner(owner)
		{}

		virtual ~EnemyState() = default;

		GameObject* GetOwner() const { return m_pOwner; }

	private:
		GameObject* m_pOwner{};
	};
}
