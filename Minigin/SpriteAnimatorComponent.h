#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>

namespace dae
{
	class RenderComponent;

	enum class Direction
	{
		Left,
		Right,
		Up,
		Down
	};

	class SpriteAnimatorComponent final : public BaseComponent
	{
	public:
		SpriteAnimatorComponent(GameObject* pOwner);
		virtual ~SpriteAnimatorComponent() = default;

		void Update() override;
		void Render() const override {}

		void SetDirection(const glm::vec3& direction);
		void SetAnimationSpeed(float framesPerSecond);

	private:
		void UpdateSprite();

		RenderComponent* m_pRenderComponent{ nullptr };
		Direction m_currentDirection{ Direction::Down };
		int m_currentFrame{ 0 };
		float m_animationTimer{ 0.0f };
		float m_frameDuration{ 0.1f };
		bool m_isMoving{ false };
		glm::vec3 m_lastDirection{ 0.0f, 0.0f, 0.0f };
	};
}
