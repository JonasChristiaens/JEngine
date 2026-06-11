#include "DeathAnimatorComponent.h"
#include "Scene/GameObject.h"
#include "Components/RenderComponent.h"
#include "Components/SpriteAnimatorComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/EnemyMovementComponent.h"
#include "EventQueue/EventManager.h"

namespace dae
{
	DeathAnimatorComponent::DeathAnimatorComponent(GameObject* pOwner, const std::string& texture, const std::vector<SDL_FRect>& frames, float fps, float scale, bool broadcastEntityDied)
		: BaseComponent(pOwner)
		, m_DeathTexture(texture)
		, m_DeathFrames(frames)
		, m_DeathFps(fps)
		, m_DeathScale(scale)
		, m_BroadcastEntityDied(broadcastEntityDied)
	{}

	void DeathAnimatorComponent::Play()
	{
		if (m_IsPlaying)
			return;

		m_IsPlaying = true;

		auto* owner = GetOwner();

		auto* collider = owner->GetComponent<CollisionComponent>();
		if (collider)
		{
			collider->SetOffset({ -100000.0f, -100000.0f });
		}

		auto* movement = owner->GetComponent<EnemyMovementComponent>();
		if (movement)
		{
			movement->SetEnabled(false);
		}

		auto* render = owner->GetComponent<RenderComponent>();
		if (render && !m_DeathFrames.empty())
		{
			render->SetTexture(m_DeathTexture);
			render->SetSourceRectangle(m_DeathFrames[0]);
			render->SetScale(m_DeathScale);
		}

		auto* animator = owner->GetComponent<SpriteAnimatorComponent>();
		if (!animator)
		{
			animator = owner->AddComponent<SpriteAnimatorComponent>();
		}

		auto finalizeDeath = [this, owner]()
			{
				m_IsPlaying = false;

				auto* deathRender = owner->GetComponent<RenderComponent>();
				if (deathRender)
				{
					deathRender->SetSourceRectangle(0.0f, 0.0f, 0.0f, 0.0f);
				}

				if (!m_BroadcastEntityDied)
					return;

				if (!EventManager::IsAlive())
					return;

				Event diedEvent(make_sdbm_hash("EntityDied"));
				EventManager::GetInstance().BroadcastEvent(diedEvent, owner);
			};

		if (animator && !m_DeathFrames.empty())
		{
			animator->SetAnimation(m_DeathFrames, m_DeathFps, false);
			animator->SetOnAnimationFinishedCallback(finalizeDeath);
		}
		else if (m_BroadcastEntityDied)
		{
			finalizeDeath();
		}
	}
}
