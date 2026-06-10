#include "EnemyIdleState.h"
#include "Scene/GameObject.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"

dae::EnemyIdleState::EnemyIdleState(GameObject* owner, float spriteX, float spriteY, float spriteWidth, float spriteHeight, float scale)
	: EnemyState(owner)
	, m_SpriteX(spriteX)
	, m_SpriteY(spriteY)
	, m_SpriteWidth(spriteWidth)
	, m_SpriteHeight(spriteHeight)
	, m_Scale(scale)
{}

void dae::EnemyIdleState::OnEnter()
{
	auto* owner = GetOwner();
	if (!owner)
		return;

	m_pRenderComponent = owner->GetComponent<RenderComponent>();
	m_pAnimator = owner->GetComponent<SpriteAnimatorComponent>();
	if (!m_pRenderComponent || !m_pAnimator)
		return;

	m_pRenderComponent->SetTexture("BombermanSprites_General.png");
	m_pRenderComponent->SetSourceRectangle(m_SpriteX, m_SpriteY, m_SpriteWidth, m_SpriteHeight);
	m_pRenderComponent->SetScale(m_Scale);
	m_pRenderComponent->SetPivot({ 0.5f, 0.5f });

	const std::vector<SDL_FRect> frames{
		{ m_SpriteX + m_SpriteWidth * 0.0f, m_SpriteY, m_SpriteWidth, m_SpriteHeight },
		{ m_SpriteX + m_SpriteWidth * 1.0f, m_SpriteY, m_SpriteWidth, m_SpriteHeight },
		{ m_SpriteX + m_SpriteWidth * 2.0f, m_SpriteY, m_SpriteWidth, m_SpriteHeight },
		{ m_SpriteX + m_SpriteWidth * 3.0f, m_SpriteY, m_SpriteWidth, m_SpriteHeight },
		{ m_SpriteX + m_SpriteWidth * 4.0f, m_SpriteY, m_SpriteWidth, m_SpriteHeight },
		{ m_SpriteX + m_SpriteWidth * 5.0f, m_SpriteY, m_SpriteWidth, m_SpriteHeight }
	};
	m_pAnimator->SetAnimation(frames, 6.0f, true);
}
