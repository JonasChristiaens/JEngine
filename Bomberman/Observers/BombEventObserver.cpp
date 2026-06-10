#include "BombEventObserver.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/DelayedEventComponent.h"
#include "Components/BombComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/DetonatorComponent.h"
#include "Managers/ExplosionEffectManager.h"
#include "EventQueue/EventManager.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace
{
	constexpr dae::EventId kPlaceBombEventId = dae::make_sdbm_hash("PlaceBombEvent");
	constexpr dae::EventId kDetonateBombEventId = dae::make_sdbm_hash("DetonateBombEvent");
	constexpr dae::EventId kPlayAudioEventId = dae::make_sdbm_hash("PlayAudioEvent");
	constexpr dae::EventId kRemoteDetonateEventId = dae::make_sdbm_hash("RemoteDetonateEvent");

	constexpr const char* kBombLaySfxPath = "bomb_lay.wav";
	constexpr const char* kBombExplosionSfxPath = "bomb_explosion.wav";

	constexpr float kBombScale = 2.0f;
}

dae::BombEventObserver::BombEventObserver(Scene& scene, float tileWorldSize)
	: m_pScene(&scene)
	, m_TileWorldSize(tileWorldSize)
	, m_pExplosionManager(std::make_unique<ExplosionEffectManager>(scene, tileWorldSize))
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

void dae::BombEventObserver::Notify(GameObject& actor, Event event)
{
	if (m_pScene == nullptr)
		return;

	if (event.id == kPlaceBombEventId)
	{
		auto* actorTransform = actor.GetComponent<dae::TransformComponent>();
		if (!actorTransform)
			return;

		const auto& actorLocalPos = actorTransform->GetLocalPosition();
		float x = actorLocalPos.x;
		float y = actorLocalPos.y;

		if (event.nbArgs >= 2)
		{
			x = event.args[0].f;
			y = event.args[1].f;
		}

		int explosionRange = kDefaultExplosionRange;
		if (event.nbArgs >= 3)
		{
			explosionRange = event.args[2].i;
		}

		const float tileCoordX = (x / m_TileWorldSize) - 0.5f;
		const float tileCoordY = (y / m_TileWorldSize) - 0.5f;
		const int column = std::max(0, static_cast<int>(std::lround(tileCoordX)));
		const int row = std::max(0, static_cast<int>(std::lround(tileCoordY)));
		const float snappedX = (static_cast<float>(column) + 0.5f) * m_TileWorldSize;
		const float snappedY = (static_cast<float>(row) + 0.5f) * m_TileWorldSize;

		x = snappedX;
		y = snappedY;

		if (IsBombAtPosition({ x, y }))
			return;

		dae::Event playAudioEvent(kPlayAudioEventId);
		playAudioEvent.nbArgs = 1;
		playAudioEvent.args[0].p = kBombLaySfxPath;
		dae::EventManager::GetInstance().BroadcastEvent(playAudioEvent, &actor);

		auto bomb = std::make_unique<dae::GameObject>();
		auto* transform = bomb->AddComponent<dae::TransformComponent>();
		transform->SetLocalPosition(x, y, 0.0f);

		auto* render = bomb->AddComponent<dae::RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetScale(kBombScale + 0.5f);
		render->SetRenderLayer(1);
		render->SetSourceRectangle(0.f, 48.f, 16.f, 16.f);
		render->SetPivot({ 0.5f, 0.5f });

		auto* animator = bomb->AddComponent<dae::SpriteAnimatorComponent>();
		animator->SetAnimation(
			{
			SDL_FRect{ 0.f, 48.f, 16.f, 16.f },
			SDL_FRect{ 16.f, 48.f, 16.f, 16.f },
			SDL_FRect{ 32.f, 48.f, 16.f, 16.f }
			},
			8.0f,
			true
		);

		auto* detonator = actor.GetComponent<DetonatorComponent>();
		const bool hasDetonator = detonator && detonator->HasDetonator();

		if (!hasDetonator)
		{
			dae::Event detonateEvent(kDetonateBombEventId);
			detonateEvent.nbArgs = 2;
			detonateEvent.args[0].f = x;
			detonateEvent.args[1].f = y;
			bomb->AddComponent<dae::DelayedEventComponent>(detonateEvent, 3.0f);
		}

		bomb->AddComponent<dae::BombComponent>(&actor, m_TileWorldSize, explosionRange);

		auto* capacity = actor.GetComponent<BombCapacityComponent>();
		if (capacity)
		{
			capacity->RegisterBombPlaced();
		}

		m_PlayerBombs[&actor].push_back(bomb.get());

		if (auto* parent = actor.GetParent())
		{
			bomb->SetParent(parent, false);
		}
		m_pScene->Add(std::move(bomb));
		m_ActiveBombPositions.emplace_back(x, y);
	}
	else if (event.id == kDetonateBombEventId)
	{
		auto* bombComp = actor.GetComponent<dae::BombComponent>();
		if (bombComp)
		{
			auto* owner = bombComp->GetOwnerPlayer();
			if (owner)
			{
				auto& bombs = m_PlayerBombs[owner];
				bombs.erase(std::remove(bombs.begin(), bombs.end(), &actor), bombs.end());

				if (!owner->IsMarkedForDeletion())
				{
					auto* ownerCapacity = owner->GetComponent<BombCapacityComponent>();
					if (ownerCapacity)
					{
						ownerCapacity->RegisterBombDetonated();
					}
				}
			}
		}

		DetonateBomb(actor);
	}
	else if (event.id == kRemoteDetonateEventId)
	{
		auto& bombs = m_PlayerBombs[&actor];
		if (bombs.empty())
			return;

		auto* bomb = bombs.front();
		bombs.erase(bombs.begin());

		if (bomb && !bomb->IsMarkedForDeletion())
		{
			DetonateBomb(*bomb);

			auto* capacity = actor.GetComponent<BombCapacityComponent>();
			if (capacity)
			{
				capacity->RegisterBombDetonated();
			}
		}

		if (!bombs.empty())
		{
			auto chainObj = std::make_unique<dae::GameObject>();
			dae::Event chainEvent(kRemoteDetonateEventId);
			chainEvent.nbArgs = 0;
			chainObj->AddComponent<dae::DelayedEventComponent>(chainEvent, 0.15f);
			m_pScene->Add(std::move(chainObj));
		}
	}
}

void dae::BombEventObserver::DetonateBomb(GameObject& bomb)
{
	auto* bombParent = bomb.GetParent();
	auto* bombComp = bomb.GetComponent<BombComponent>();
	GameObject* pBombOwner = bombComp ? bombComp->GetOwnerPlayer() : nullptr;
	bomb.MarkForDeletion();

	auto* bombTransform = bomb.GetComponent<TransformComponent>();
	if (bombTransform == nullptr)
		return;

	float x = bombTransform->GetLocalPosition().x;
	float y = bombTransform->GetLocalPosition().y;

	const auto bombPos = glm::vec2{ x, y };
	auto it = std::find_if(m_ActiveBombPositions.begin(), m_ActiveBombPositions.end(),
		[&bombPos](const glm::vec2& p) { return p.x == bombPos.x && p.y == bombPos.y; });
	if (it != m_ActiveBombPositions.end())
	{
		m_ActiveBombPositions.erase(it);
	}

	int explosionRange = kDefaultExplosionRange;
	if (bombComp)
	{
		explosionRange = bombComp->GetExplosionRange();
	}

	Event playAudioEvent(kPlayAudioEventId);
	playAudioEvent.nbArgs = 1;
	playAudioEvent.args[0].p = kBombExplosionSfxPath;
	EventManager::GetInstance().BroadcastEvent(playAudioEvent, &bomb);

	m_pExplosionManager->SpawnExplosion(x, y, explosionRange, bombParent, pBombOwner);
}

bool dae::BombEventObserver::IsBombAtPosition(const glm::vec2& pos) const
{
	constexpr float kEpsilon = 0.001f;
	for (const auto& p : m_ActiveBombPositions)
	{
		if (std::abs(p.x - pos.x) < kEpsilon && std::abs(p.y - pos.y) < kEpsilon)
		{
			return true;
		}
	}
	return false;
}