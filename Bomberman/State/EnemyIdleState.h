#pragma once
#include "EnemyState.h"

namespace dae
{
	class RenderComponent;
	class SpriteAnimatorComponent;

	class EnemyIdleState final : public EnemyState
	{
	public:
		EnemyIdleState(GameObject* owner, float spriteX, float spriteY, float spriteWidth, float spriteHeight, float scale);

		void OnEnter() override;

	private:
		float m_SpriteX{};
		float m_SpriteY{};
		float m_SpriteWidth{};
		float m_SpriteHeight{};
		float m_Scale{};

		RenderComponent* m_pRenderComponent{};
		SpriteAnimatorComponent* m_pAnimator{};
	};
}
