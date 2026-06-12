#include "SpriteAnimatorComponent.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "GameTime.h"

dae::SpriteAnimatorComponent::SpriteAnimatorComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{}

void dae::SpriteAnimatorComponent::Update()
{
	// Ensure we have a RenderComponent to communicate with
	if (m_pRenderComponent == nullptr)
	{
		m_pRenderComponent = GetOwner()->GetComponent<RenderComponent>();
		if (m_pRenderComponent == nullptr)
			return;
	}

	if (!m_IsPlaying || m_IsFinished)
		return;

	const float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();
	m_AnimationTimer += deltaTime;

	if (m_AnimationTimer >= m_FrameDuration)
	{
		m_AnimationTimer -= m_FrameDuration;
		m_CurrentFrame++;

		if (m_CurrentFrame >= m_FrameCount)
		{
			if (m_IsLooping)
			{
				m_CurrentFrame = 0;
			}
			else
			{
				m_CurrentFrame = m_FrameCount - 1; // Stay on the last frame
				m_IsFinished = true;
				m_IsPlaying = false;

				if (m_OnFinishedCallback)
				{
					m_OnFinishedCallback();
				}
			}
		}

		UpdateSprite();
	}
}

void dae::SpriteAnimatorComponent::SetAnimation(int startColumn, int startRow, int columns, int frameCount, float framesPerSecond, bool isLooping)
{
	// Ensure we don't reset the current animation if it shouldn't change
	if (m_StartColumn == startColumn && m_StartRow == startRow && m_IsPlaying)
		return;

	m_UseExplicitFrames = false;
	m_ExplicitFrames.clear();

	m_StartColumn = startColumn;
	m_StartRow = startRow;
	m_Columns = columns > 0 ? columns : 1;
	m_FrameCount = frameCount > 0 ? frameCount : 1;

	if (framesPerSecond > 0.0f)
		m_FrameDuration = 1.0f / framesPerSecond;

	m_IsLooping = isLooping;

	// Reset runtime state
	m_CurrentFrame = 0;
	m_AnimationTimer = 0.0f;
	m_IsFinished = false;
	m_IsPlaying = true;

	UpdateSprite();
}

void dae::SpriteAnimatorComponent::SetAnimation(const std::vector<SDL_FRect>& frames, float framesPerSecond, bool isLooping)
{
	m_ExplicitFrames = frames;
	m_UseExplicitFrames = !m_ExplicitFrames.empty();
	m_FrameCount = static_cast<int>(m_ExplicitFrames.size());

	if (framesPerSecond > 0.0f)
		m_FrameDuration = 1.0f / framesPerSecond;

	m_IsLooping = isLooping;
	m_CurrentFrame = 0;
	m_AnimationTimer = 0.0f;
	m_IsFinished = false;
	m_IsPlaying = (m_FrameCount > 0);

	UpdateSprite();
}

void dae::SpriteAnimatorComponent::Play()
{
	if (m_IsFinished)
	{
		m_CurrentFrame = 0;
		m_IsFinished = false;
	}
	m_IsPlaying = true;
}

void dae::SpriteAnimatorComponent::Pause()
{
	m_IsPlaying = false;
}

void dae::SpriteAnimatorComponent::Stop()
{
	m_IsPlaying = false;
	m_CurrentFrame = 0;
	m_AnimationTimer = 0.0f;
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

	if (m_UseExplicitFrames)
	{
		if (m_CurrentFrame < static_cast<int>(m_ExplicitFrames.size()))
			m_pRenderComponent->SetSourceRectangle(m_ExplicitFrames[m_CurrentFrame]);
		return;
	}

	int columnOffset = m_CurrentFrame % m_Columns;
	int rowOffset = m_CurrentFrame / m_Columns;
	m_pRenderComponent->SetSprite(m_StartColumn + columnOffset, m_StartRow + rowOffset);
}
