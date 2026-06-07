#include "BombEventObserver.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/DelayedEventComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BombComponent.h"
#include "Components/BombCapacityComponent.h"
#include "Components/DetonatorComponent.h"
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
	constexpr float kExplosionFrameSize = 80.0f;
	constexpr float kExplosionTileSize = 16.0f;
	constexpr float kExplosionStartX = 0.0f;
	constexpr float kExplosionStartY = 65.0f;
	constexpr int kExplosionFrameColumns = 2;
	constexpr int kExplosionFrameCount = 4;
	constexpr float kExplosionFramesPerSecond = 12.0f;
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
		playAudioEvent.args[0].p = const_cast<char*>(kBombLaySfxPath);
		dae::EventManager::GetInstance().BroadcastEvent(playAudioEvent, &actor);

		auto bomb = std::make_unique<dae::GameObject>();
		auto* transform = bomb->AddComponent<dae::TransformComponent>();
		transform->SetLocalPosition(x, y, 0.0f);

		auto* render = bomb->AddComponent<dae::RenderComponent>();
		render->SetTexture("BombermanSprites_General.png");
		render->SetScale(kBombScale + 0.5f);
		render->SetRenderLayer(1);
		render->SetSourceRectangle(0.f, 49.f, 16.f, 16.f);
		render->SetPivot({ 0.5f, 0.5f });

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
	auto* bombComp = bomb.GetComponent<BombComponent>();
	if (bombComp)
	{
		explosionRange = bombComp->GetExplosionRange();
	}

	Event playAudioEvent(kPlayAudioEventId);
	playAudioEvent.nbArgs = 1;
	playAudioEvent.args[0].p = const_cast<char*>(kBombExplosionSfxPath);
	EventManager::GetInstance().BroadcastEvent(playAudioEvent, &bomb);

	const float bombCenterX = x;
	const float bombCenterY = y;

	std::vector<GameObject*> damageTiles{};
	damageTiles.reserve(static_cast<size_t>(1 + explosionRange * 4));

	std::vector<GameObject*> explosionTiles{};
	explosionTiles.reserve(static_cast<size_t>(1 + explosionRange * 4));

	auto buildExplosionFrames = [](int tileColumn, int tileRow)
		{
			std::vector<SDL_FRect> frames{};
			frames.reserve(kExplosionFrameCount);
			for (int frameIndex = 0; frameIndex < kExplosionFrameCount; ++frameIndex)
			{
				const int frameColumn = frameIndex % kExplosionFrameColumns;
				const int frameRow = frameIndex / kExplosionFrameColumns;
				const float originX = kExplosionStartX + static_cast<float>(frameColumn) * kExplosionFrameSize;
				const float originY = kExplosionStartY + static_cast<float>(frameRow) * kExplosionFrameSize;
				frames.push_back(SDL_FRect{
					originX + static_cast<float>(tileColumn) * kExplosionTileSize,
					originY + static_cast<float>(tileRow) * kExplosionTileSize,
					kExplosionTileSize,
					kExplosionTileSize
					});
			}
			return frames;
		};

	auto addExplosionTile = [&](float centerX, float centerY, int tileColumn, int tileRow)
		{
			auto tile = std::make_unique<GameObject>();
			auto* transform = tile->AddComponent<TransformComponent>();
			transform->SetLocalPosition(centerX, centerY, 0.0f);

			auto* render = tile->AddComponent<RenderComponent>();
			render->SetTexture("BombermanSprites_General.png");
			render->SetScale(m_TileWorldSize / kExplosionTileSize);
			render->SetPivot({ 0.5f, 0.5f });
			render->SetRenderLayer(2);

			auto* animator = tile->AddComponent<SpriteAnimatorComponent>();
			animator->SetAnimation(buildExplosionFrames(tileColumn, tileRow), kExplosionFramesPerSecond, false);

			if (bombParent)
			{
				tile->SetParent(bombParent, false);
			}

			auto* tilePtr = tile.get();
			explosionTiles.push_back(tilePtr);
			m_pScene->Add(std::move(tile));
			return animator;
		};

	auto addDamageTile = [&](float centerX, float centerY)
		{
			auto damageTile = std::make_unique<GameObject>();
			auto* transform = damageTile->AddComponent<TransformComponent>();
			transform->SetLocalPosition(centerX, centerY, 0.0f);

			auto* collider = damageTile->AddComponent<CollisionComponent>(m_TileWorldSize, m_TileWorldSize, true);
			collider->SetOffset({ -m_TileWorldSize * 0.5f, -m_TileWorldSize * 0.5f });
			collider->SetOnCollisionCallback([](GameObject* other)
				{
					if (!other || other->IsMarkedForDeletion())
						return;

					auto* health = other->GetComponent<HealthComponent>();
					if (!health)
						return;

					Event damageEvent(make_sdbm_hash("ChangeHealthEvent"));
					damageEvent.nbArgs = 1;
					damageEvent.args[0].i = -1;
					EventManager::GetInstance().BroadcastEvent(damageEvent, other);
				});

			if (bombParent)
			{
				damageTile->SetParent(bombParent, false);
			}
			damageTiles.push_back(damageTile.get());
			m_pScene->Add(std::move(damageTile));
		};

	addDamageTile(bombCenterX, bombCenterY);
	auto* centerAnimator = addExplosionTile(bombCenterX, bombCenterY, 2, 2);
	for (int step = 1; step <= explosionRange; ++step)
	{
		const float offset = static_cast<float>(step) * m_TileWorldSize;
		addDamageTile(bombCenterX - offset, bombCenterY);
		addDamageTile(bombCenterX + offset, bombCenterY);
		addDamageTile(bombCenterX, bombCenterY - offset);
		addDamageTile(bombCenterX, bombCenterY + offset);

		const bool isFinalStep = step == explosionRange;
		addExplosionTile(bombCenterX - offset, bombCenterY, isFinalStep ? 0 : 1, 2);
		addExplosionTile(bombCenterX + offset, bombCenterY, isFinalStep ? 4 : 3, 2);
		addExplosionTile(bombCenterX, bombCenterY - offset, 2, isFinalStep ? 0 : 1);
		addExplosionTile(bombCenterX, bombCenterY + offset, 2, isFinalStep ? 4 : 3);
	}

	if (centerAnimator)
	{
		centerAnimator->SetOnAnimationFinishedCallback(
			[tiles = std::move(damageTiles), visuals = std::move(explosionTiles)]() mutable
			{
				for (auto* visual : visuals)
				{
					if (visual)
					{
						visual->MarkForDeletion();
					}
				}
				for (auto* tile : tiles)
				{
					if (tile)
					{
						tile->MarkForDeletion();
					}
				}
			});
	}
}

bool dae::BombEventObserver::IsBombAtPosition(const glm::vec2& pos) const
{
	for (const auto& p : m_ActiveBombPositions)
	{
		if (p.x == pos.x && p.y == pos.y)
		{
			return true;
		}
	}
	return false;
}