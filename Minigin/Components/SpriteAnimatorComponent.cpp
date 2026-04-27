	#include "SpriteAnimatorComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "GameTime.h"

dae::SpriteAnimatorComponent::SpriteAnimatorComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::SpriteAnimatorComponent::Update()
{
	// Ensure we have a RenderComponent to communicate with
	if (m_pRenderComponent == nullptr)
	{
		m_pRenderComponent = GetOwner()->GetComponent<RenderComponent>();
		if (m_pRenderComponent == nullptr)
			return;
	}

	if (!m_isPlaying || m_isFinished)
		return;

	float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();
	m_animationTimer += deltaTime;

	if (m_animationTimer >= m_frameDuration)
	{
		m_animationTimer -= m_frameDuration;
		m_currentFrame++;

		if (m_currentFrame >= m_frameCount)
		{
			if (m_isLooping)
			{
				m_currentFrame = 0;
			}
			else
			{
				m_currentFrame = m_frameCount - 1; // Stay on the last frame
				m_isFinished = true;
				m_isPlaying = false;

				if (m_onFinishedCallback)
				{
					m_onFinishedCallback();
				}
			}
		}

		UpdateSprite();
	}
}

void dae::SpriteAnimatorComponent::SetAnimation(int startColumn, int startRow, int columns, int frameCount, float framesPerSecond, bool isLooping)
{
	// Ensure we don't reset the current animation if it shouldn't change
	if (m_startColumn == startColumn && m_startRow == startRow && m_isPlaying)
		return;

	m_useExplicitFrames = false;
	m_explicitFrames.clear();

	m_startColumn = startColumn;
	m_startRow = startRow;
	m_columns = columns > 0 ? columns : 1;
	m_frameCount = frameCount > 0 ? frameCount : 1;
	
	if (framesPerSecond > 0.0f)
		m_frameDuration = 1.0f / framesPerSecond;

	m_isLooping = isLooping;
	
	// Reset runtime state
	m_currentFrame = 0;
	m_animationTimer = 0.0f;
	m_isFinished = false;
	m_isPlaying = true;

	UpdateSprite();
}

void dae::SpriteAnimatorComponent::SetAnimation(const std::vector<SDL_FRect>& frames, float framesPerSecond, bool isLooping)
{
	m_explicitFrames = frames;
	m_useExplicitFrames = !m_explicitFrames.empty();
	m_frameCount = static_cast<int>(m_explicitFrames.size());

	if (framesPerSecond > 0.0f)
		m_frameDuration = 1.0f / framesPerSecond;

	m_isLooping = isLooping;
	m_currentFrame = 0;
	m_animationTimer = 0.0f;
	m_isFinished = false;
	m_isPlaying = (m_frameCount > 0);

	UpdateSprite();
}

void dae::SpriteAnimatorComponent::Play()
{
	if (m_isFinished)
	{
		m_currentFrame = 0;
		m_isFinished = false;
	}
	m_isPlaying = true;
}

void dae::SpriteAnimatorComponent::Pause()
{
	m_isPlaying = false;
}

void dae::SpriteAnimatorComponent::Stop()
{
	m_isPlaying = false;
	m_currentFrame = 0;
	m_animationTimer = 0.0f;
	UpdateSprite();
}

void dae::SpriteAnimatorComponent::UpdateSprite()
{
	if (m_pRenderComponent == nullptr)
	{
		m_pRenderComponent = GetOwner()->GetComponent<RenderComponent>();
		if (m_pRenderComponent == nullptr)
			return;
	}

	if (m_useExplicitFrames)
	{
		if (m_currentFrame >= 0 && m_currentFrame < static_cast<int>(m_explicitFrames.size()))
			m_pRenderComponent->SetSourceRectangle(m_explicitFrames[m_currentFrame]);
		return;
	}

	int columnOffset = m_currentFrame % m_columns;
	int rowOffset = m_currentFrame / m_columns;
	m_pRenderComponent->SetSprite(m_startColumn + columnOffset, m_startRow + rowOffset);
}
