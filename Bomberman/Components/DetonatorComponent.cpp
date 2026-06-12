#include "DetonatorComponent.h"
#include "EventQueue/EventManager.h"
#include "Core/GameTime.h"

dae::DetonatorComponent::DetonatorComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{}

void dae::DetonatorComponent::Update()
{
	if (!m_ChainActive)
		return;

	m_ChainCooldown -= GameTime::GetInstance().GetDeltaTime();
	if (m_ChainCooldown > 0.0f)
		return;

	m_ChainCooldown = kChainInterval;

	Event remoteDetonateEvent(make_sdbm_hash("RemoteDetonateEvent"));
	remoteDetonateEvent.nbArgs = 0;
	EventManager::GetInstance().BroadcastEvent(remoteDetonateEvent, GetOwner());
}

void dae::DetonatorComponent::StartDetonateChain()
{
	m_ChainActive = true;
	m_ChainCooldown = 0.0f;
}

void dae::DetonatorComponent::StopDetonateChain()
{
	m_ChainActive = false;
	m_ChainCooldown = 0.0f;
}
