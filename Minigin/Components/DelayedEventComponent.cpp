#include "DelayedEventComponent.h"
#include "../EventQueue/EventManager.h"
#include "../GameTime.h"

dae::DelayedEventComponent::DelayedEventComponent(GameObject* pOwner, const Event& eventToSend, float delaySeconds)
    : BaseComponent(pOwner)
    , m_EventToSend(eventToSend)
    , m_DelaySeconds(delaySeconds)
{
}

void dae::DelayedEventComponent::Update()
{
    if (m_HasSent)
        return;

    m_Elapsed += dae::GameTime::GetInstance().GetDeltaTime();
    if (m_Elapsed < m_DelaySeconds)
        return;

    dae::EventManager::GetInstance().BroadcastEvent(m_EventToSend, GetOwner());
    m_HasSent = true;
}