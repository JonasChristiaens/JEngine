#include "BalloomIdleState.h"
#include "Scene/GameObject.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"

void dae::BalloomIdleState::OnEnter()
{
    auto* owner = GetOwner();
    if (!owner)
        return;

    m_pRenderComponent = owner->GetComponent<RenderComponent>();
    m_pAnimator = owner->GetComponent<SpriteAnimatorComponent>();
    if (!m_pRenderComponent || !m_pAnimator)
        return;

    constexpr float spriteX = 0.0f;
    constexpr float spriteY = 241.0f;
    constexpr float spriteWidth = 16.0f;
    constexpr float spriteHeight = 16.0f;

    m_pRenderComponent->SetTexture("Textures/BombermanSprites_General.png");
    m_pRenderComponent->SetSourceRectangle(spriteX, spriteY, spriteWidth, spriteHeight);
    m_pRenderComponent->SetScale(3.0f);
    m_pRenderComponent->SetPivot({ 0.5f, 0.5f });

    const std::vector<SDL_FRect> frames{
        { spriteX + spriteWidth * 0.0f, spriteY, spriteWidth, spriteHeight },
        { spriteX + spriteWidth * 1.0f, spriteY, spriteWidth, spriteHeight },
        { spriteX + spriteWidth * 2.0f, spriteY, spriteWidth, spriteHeight },
        { spriteX + spriteWidth * 3.0f, spriteY, spriteWidth, spriteHeight },
        { spriteX + spriteWidth * 4.0f, spriteY, spriteWidth, spriteHeight },
        { spriteX + spriteWidth * 5.0f, spriteY, spriteWidth, spriteHeight }
    };
    m_pAnimator->SetAnimation(frames, 6.0f, true);
}
