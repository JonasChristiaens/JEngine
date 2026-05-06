#include "BombEventObserver.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/DelayedEventComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "EventQueue/EventManager.h"
#include <vector>

namespace
{
	constexpr dae::EventId kPlaceBombEventId = dae::make_sdbm_hash("PlaceBombEvent");
	constexpr dae::EventId kDetonateBombEventId = dae::make_sdbm_hash("DetonateBombEvent");
	constexpr dae::EventId kPlayAudioEventId = dae::make_sdbm_hash("PlayAudioEvent");

	constexpr const char* kBombLaySfxPath = "Audio/Effects/bomb_lay.wav";
	constexpr const char* kBombExplosionSfxPath = "Audio/Effects/bomb_explosion.wav";

	constexpr float kBombScale = 2.0f;
	constexpr float kExplosionScale = 2.0f;
	constexpr float kExplosionSpriteSize = 80.0f;
}

dae::BombEventObserver::BombEventObserver(Scene& scene, float tileWorldSize)
	: m_pScene(&scene)
	, m_TileWorldSize(tileWorldSize)
{
	dae::EventManager::GetInstance().AddObserver(*this);
}

dae::BombEventObserver::~BombEventObserver()
{
	if (dae::EventManager::IsAlive())
	{
		dae::EventManager::GetInstance().RemoveObserver(*this);
	}
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
		render->SetScale(kBombScale);
		render->SetRenderLayer(4);
		render->SetSourceRectangle(0.f, 49.f, 16.f, 16.f);
		render->SetPivot({ 0.5f, 1.0f });

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

		constexpr float bombSpriteSize = 16.0f;
		const float bombCenterX = x;
		const float bombCenterY = y - (bombSpriteSize * kBombScale * 0.5f);
		const int explosionRange = m_ExplosionRange;

		auto explosionVisual = std::make_unique<dae::GameObject>();
		auto* visualTransform = explosionVisual->AddComponent<dae::TransformComponent>();
		const float explosionHalfSize = kExplosionSpriteSize * kExplosionScale * 0.5f;
		visualTransform->SetLocalPosition(bombCenterX - explosionHalfSize, bombCenterY - explosionHalfSize, 0.0f);

		auto* visualRender = explosionVisual->AddComponent<dae::RenderComponent>();
		visualRender->SetTexture("Resources/BombermanSprites_General.png");
		visualRender->SetSourceRectangle(0.f, 65.f, 80.f, 80.f);
		visualRender->SetScale(kExplosionScale);
		visualRender->SetRenderLayer(2);

		auto* visualAnimator = explosionVisual->AddComponent<dae::SpriteAnimatorComponent>();
		visualAnimator->SetAnimation(
			{
				SDL_FRect{ 0.f, 65.f, 80.f, 80.f },
				SDL_FRect{ 80.f, 65.f, 80.f, 80.f },
				SDL_FRect{ 0.f, 145.f, 80.f, 80.f },
				SDL_FRect{ 80.f, 145.f, 80.f, 80.f }
			},
			12.0f,
			false
		);

		std::vector<dae::GameObject*> damageTiles{};
		damageTiles.reserve(static_cast<size_t>(1 + explosionRange * 4));

		auto addDamageTile = [&](float centerX, float centerY)
			{
				auto damageTile = std::make_unique<dae::GameObject>();
				auto* transform = damageTile->AddComponent<dae::TransformComponent>();
				transform->SetLocalPosition(centerX, centerY, 0.0f);

				auto* collider = damageTile->AddComponent<dae::CollisionComponent>(m_TileWorldSize, m_TileWorldSize, true);
				collider->SetOffset({ -m_TileWorldSize * 0.5f, -m_TileWorldSize * 0.5f });
				collider->SetOnCollisionCallback([](dae::GameObject* other)
					{
						if (!other || other->IsMarkedForDeletion())
							return;

						auto* health = other->GetComponent<dae::HealthComponent>();
						if (!health)
							return;

						dae::Event damageEvent(dae::make_sdbm_hash("ChangeHealthEvent"));
						damageEvent.nbArgs = 1;
						damageEvent.args[0].i = -1;
						dae::EventManager::GetInstance().BroadcastEvent(damageEvent, other);
					});

				damageTiles.push_back(damageTile.get());
				m_pScene->Add(std::move(damageTile));
			};

		addDamageTile(bombCenterX, bombCenterY);
		for (int step = 1; step <= explosionRange; ++step)
		{
			const float offset = static_cast<float>(step) * m_TileWorldSize;
			addDamageTile(bombCenterX - offset, bombCenterY);
			addDamageTile(bombCenterX + offset, bombCenterY);
			addDamageTile(bombCenterX, bombCenterY - offset);
			addDamageTile(bombCenterX, bombCenterY + offset);
		}

		auto* visualPtr = explosionVisual.get();
		visualAnimator->SetOnAnimationFinishedCallback(
			[visualPtr, tiles = std::move(damageTiles)]() mutable
			{
				if (visualPtr)
				{
					visualPtr->MarkForDeletion();
				}
				for (auto* tile : tiles)
				{
					if (tile)
					{
						tile->MarkForDeletion();
					}
				}
			});

		m_pScene->Add(std::move(explosionVisual));
	}
}