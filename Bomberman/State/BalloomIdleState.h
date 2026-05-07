#pragma once
#include "EnemyState.h"

namespace dae
{
	class RenderComponent;
	class SpriteAnimatorComponent;

	class BalloomIdleState final : public EnemyState
	{
	public:
		explicit BalloomIdleState(GameObject* owner)
			: EnemyState(owner)
		{}

		void OnEnter() override;

	private:
		RenderComponent* m_pRenderComponent{};
		SpriteAnimatorComponent* m_pAnimator{};
	};
}
