#include "LivesDisplayComponent.h"
#include "TextComponent.h"
#include "HealthComponent.h"
#include "GameObject.h"

dae::LivesDisplayComponent::LivesDisplayComponent(GameObject* pOwner, HealthComponent* pHealthComponent)
	: BaseComponent(pOwner)
	, m_pHealthComponent(pHealthComponent)
{
	m_pTextComponent = GetOwner()->GetComponent<TextComponent>();

	if (m_pHealthComponent)
	{
		m_pHealthComponent->AddObserver(*this);
	}

	UpdateText();
}

void dae::LivesDisplayComponent::Notify(Event e, GameObject* pSubjectActor)
{
	if (e == Event::PlayerDamaged && m_pHealthComponent && pSubjectActor == m_pHealthComponent->GetOwner())
	{
		UpdateText();
	}
}

void dae::LivesDisplayComponent::UpdateText()
{
	if (m_pTextComponent && m_pHealthComponent)
	{
		m_pTextComponent->SetText("# Lives: " + std::to_string(m_pHealthComponent->GetLives()));
	}
}