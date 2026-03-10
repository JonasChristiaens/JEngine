#include "SpriteAnimatorComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include <cmath>

dae::SpriteAnimatorComponent::SpriteAnimatorComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::SpriteAnimatorComponent::Update(float deltaTime)
{
	if (m_pRenderComponent == nullptr)
	{
		m_pRenderComponent = GetOwner()->GetComponent<RenderComponent>();
		if (m_pRenderComponent == nullptr)
			return;
	}

	if (!m_isMoving)
	{
		m_animationTimer = 0.0f;
		m_currentFrame = 1;
		m_pRenderComponent->SetSprite(4, 0);
		return;
	}

	m_animationTimer += deltaTime;

	if (m_animationTimer >= m_frameDuration)
	{
		m_animationTimer -= m_frameDuration;
		m_currentFrame = (m_currentFrame + 1) % 3;
		UpdateSprite();
	}

	m_isMoving = false;
}

void dae::SpriteAnimatorComponent::SetDirection(const glm::vec3& direction)
{
	if (std::abs(direction.x) < 0.01f && std::abs(direction.y) < 0.01f)
		return;

	m_isMoving = true;

	if (std::abs(direction.x) > std::abs(direction.y))
	{
		if (direction.x > 0)
			m_currentDirection = Direction::Right;
		else
			m_currentDirection = Direction::Left;
	}
	else
	{
		if (direction.y > 0)
			m_currentDirection = Direction::Down;
		else
			m_currentDirection = Direction::Up;
	}
}

void dae::SpriteAnimatorComponent::SetAnimationSpeed(float framesPerSecond)
{
	if (framesPerSecond > 0.0f)
		m_frameDuration = 1.0f / framesPerSecond;
}

void dae::SpriteAnimatorComponent::UpdateSprite()
{
	if (m_pRenderComponent == nullptr)
		return;

	int column = 0;
	int row = 0;

	switch (m_currentDirection)
	{
	case Direction::Left:
		column = m_currentFrame;
		row = 0;
		break;
	case Direction::Down:
		column = 3 + m_currentFrame;
		row = 0;
		break;
	case Direction::Right:
		column = m_currentFrame;
		row = 1;
		break;
	case Direction::Up:
		column = 3 + m_currentFrame;
		row = 1;
		break;
	}

	m_pRenderComponent->SetSprite(column, row);
}
