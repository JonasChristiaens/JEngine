#include "BombEventObserver.h"
#include "Scene.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/DelayedEventComponent.h"
#include "EventQueue/EventManager.h"

namespace
{
    constexpr dae::EventId kPlaceBombEventId = dae::make_sdbm_hash("PlaceBombEvent");
    constexpr dae::EventId kDetonateBombEventId = dae::make_sdbm_hash("DetonateBombEvent");
    constexpr dae::EventId kPlayAudioEventId = dae::make_sdbm_hash("PlayAudioEvent");

    constexpr const char* kBombLaySfxPath = "Audio/Effects/bomb_lay.wav";
    constexpr const char* kBombExplosionSfxPath = "Audio/Effects/bomb_explosion.wav";
}

dae::BombEventObserver::BombEventObserver(Scene& scene)
    : m_pScene(&scene)
{
    dae::EventManager::GetInstance().AddObserver(*this);
}

dae::BombEventObserver::~BombEventObserver()
{
    dae::EventManager::GetInstance().RemoveObserver(*this);
}

void dae::BombEventObserver::Notify(const GameObject& pGameActor, Event event)
{
    if (m_pScene == nullptr)
        return;

    auto* pSubject = const_cast<dae::GameObject*>(&pGameActor);

    if (event.id == kPlaceBombEventId)
    {
        float x = pGameActor.GetWorldPosition().x;
        float y = pGameActor.GetWorldPosition().y;

        if (event.nbArgs >= 2)
        {
            x = event.args[0].f;
            y = event.args[1].f;
        }

        dae::Event playAudioEvent(kPlayAudioEventId);
        playAudioEvent.nbArgs = 1;
        playAudioEvent.args[0].p = const_cast<char*>(kBombLaySfxPath);
        dae::EventManager::GetInstance().BroadcastEvent(playAudioEvent, pSubject);

        auto bomb = std::make_unique<dae::GameObject>();
        auto* transform = bomb->AddComponent<dae::TransformComponent>();
        transform->SetLocalPosition(x, y, 0.0f);

        auto* render = bomb->AddComponent<dae::RenderComponent>();
        render->SetTexture("Resources/BombermanSprites_General.png");
        render->SetScale(2.0f);
        render->SetRenderLayer(1);
        render->SetSourceRectangle(0.f, 49.f, 16.f, 16.f);

        auto* animator = bomb->AddComponent<dae::SpriteAnimatorComponent>();
        animator->SetAnimation(
            {
                SDL_FRect{ 0.f, 49.f, 16.f, 16.f },
                SDL_FRect{ 16.f, 49.f, 16.f, 16.f },
                SDL_FRect{ 32.f, 49.f, 16.f, 16.f }
            },
            8.0f,
            true
        );

        ++m_NextBombId;
        dae::Event detonateEvent(kDetonateBombEventId);
        detonateEvent.nbArgs = 3;
        detonateEvent.args[0].i = m_NextBombId;
        detonateEvent.args[1].f = x;
        detonateEvent.args[2].f = y;

        bomb->AddComponent<dae::DelayedEventComponent>(detonateEvent, 3.0f);

        m_pScene->Add(std::move(bomb));
    }
    else if (event.id == kDetonateBombEventId)
    {
        pSubject->MarkForDeletion();

        float x = pGameActor.GetWorldPosition().x;
        float y = pGameActor.GetWorldPosition().y;
        if (event.nbArgs >= 3)
        {
            x = event.args[1].f;
            y = event.args[2].f;
        }

        dae::Event playAudioEvent(kPlayAudioEventId);
        playAudioEvent.nbArgs = 1;
        playAudioEvent.args[0].p = const_cast<char*>(kBombExplosionSfxPath);
        dae::EventManager::GetInstance().BroadcastEvent(playAudioEvent, pSubject);

        auto explosion = std::make_unique<dae::GameObject>();
        auto* transform = explosion->AddComponent<dae::TransformComponent>();
        transform->SetLocalPosition(x - 24.0f, y - 24.0f, 0.0f);

        auto* render = explosion->AddComponent<dae::RenderComponent>();
        render->SetTexture("Resources/BombermanSprites_General.png");
        render->SetSourceRectangle(0.f, 65.f, 80.f, 80.f);
        render->SetScale(1.0f);
        render->SetRenderLayer(2);

        auto* animator = explosion->AddComponent<dae::SpriteAnimatorComponent>();
        animator->SetAnimation(
            {
                SDL_FRect{ 0.f, 65.f, 80.f, 80.f },
                SDL_FRect{ 80.f, 65.f, 80.f, 80.f },
                SDL_FRect{ 0.f, 145.f, 80.f, 80.f },
                SDL_FRect{ 80.f, 145.f, 80.f, 80.f }
            },
            12.0f,
            false
        );

        animator->SetOnAnimationFinishedCallback([explosionPtr = explosion.get()]() {
            explosionPtr->MarkForDeletion();
        });

        m_pScene->Add(std::move(explosion));
    }
}