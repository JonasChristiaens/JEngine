#include "HealthComponent.h"
#include "EventQueue/EventManager.h"
#include "Scene/GameObject.h"
#include "Components/DeathAnimatorComponent.h"
#include "Components/BombRangeComponent.h"
#include "Components/PlayfieldComponent.h"
#include "Components/TransformComponent.h"
#include "Core/GameTime.h"

dae::HealthComponent::HealthComponent(GameObject* pOwner, int health)
	: BaseComponent(pOwner)
	, m_CurrentHealth(health)
{
	EventManager::GetInstance().AddObserver(*this);
}

dae::HealthComponent::~HealthComponent()
{
	if (EventManager::IsAlive())
	{
		EventManager::GetInstance().RemoveObserver(*this);
	}
}

void dae::HealthComponent::Update()
{
	if (m_InvulnerabilityTimer > 0.0f)
	{
		m_InvulnerabilityTimer -= GameTime::GetInstance().GetDeltaTime();
		if (m_InvulnerabilityTimer < 0.0f)
			m_InvulnerabilityTimer = 0.0f;
	}
}

void dae::HealthComponent::ChangeCurrentHealth(int amount)
{
	if (m_IsDead)
		return;

	if (amount < 0 && m_InvulnerabilityTimer > 0.0f)
		return;

	m_CurrentHealth += amount;

	if (amount < 0)
		m_InvulnerabilityTimer = 1.0f;

	NotifyObservers(Event(make_sdbm_hash("HealthChanged")), GetOwner());

	if (m_CurrentHealth <= 0)
	{
		m_IsDead = true;

		GameObject* pParent = GetOwner()->GetParent();
		if (pParent)
		{
			auto* playfield = pParent->GetComponent<PlayfieldComponent>();
			auto* tx = GetOwner()->GetComponent<TransformComponent>();
			if (playfield && tx)
			{
				const auto& localPos = tx->GetLocalPosition();
				playfield->ClearOccupiedTile(localPos.x, localPos.y);
			}
		}

		auto* deathAnim = GetOwner()->GetComponent<DeathAnimatorComponent>();
		if (deathAnim)
		{
			deathAnim->Play();
		}
		else
		{
			Event diedEvent(make_sdbm_hash("EntityDied"));
			EventManager::GetInstance().BroadcastEvent(diedEvent, GetOwner());
		}
	}

	if (amount < 0)
	{
		if (GetOwner()->HasComponent<BombRangeComponent>())
		{
			Event playAudioEvent(make_sdbm_hash("PlayAudioEvent"));
			playAudioEvent.nbArgs = 1;
			playAudioEvent.args[0].p = "bomberman_killed.wav";
			EventManager::GetInstance().BroadcastEvent(playAudioEvent, GetOwner());
		}

		Event tookDamageEvent(make_sdbm_hash("TookDamageEvent"));
		tookDamageEvent.nbArgs = 1;
		tookDamageEvent.args[0].i = -amount;
		EventManager::GetInstance().BroadcastEvent(tookDamageEvent, GetOwner());
	}
}

void dae::HealthComponent::Notify(GameObject& actor, Event event)
{
	if (&actor == GetOwner())
	{
		if (event.id == make_sdbm_hash("ChangeHealthEvent"))
		{
			if (event.nbArgs > 0)
			{
				ChangeCurrentHealth(event.args[0].i);
			}
		}
	}
}