#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>

namespace dae
{
	class SpriteAnimatorComponent;
	class TransformComponent;

	class PlayerAnimatorComponent final : public BaseComponent
	{
	public:
		PlayerAnimatorComponent(GameObject* pOwner);
		virtual ~PlayerAnimatorComponent() = default;

		void Update() override;
		void Render() const override;

	private:
		SpriteAnimatorComponent* m_pAnimator{ nullptr };
		TransformComponent* m_pTransform{ nullptr };
		
		glm::vec3 m_lastPosition{};
		bool m_isInitialized{ false };
	};
}