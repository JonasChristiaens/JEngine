#include "ScoreDisplayComponent.h"
#include "TextComponent.h"
#include "ScoreComponent.h"
#include "GameObject.h"

dae::ScoreDisplayComponent::ScoreDisplayComponent(GameObject* pOwner, ScoreComponent* pScoreComponent)
	: BaseComponent(pOwner)
	, m_pScoreComponent(pScoreComponent)
{
	m_pTextComponent = GetOwner()->GetComponent<TextComponent>();

	if (m_pScoreComponent)
	{
		m_pScoreComponent->AddObserver(*this);
	}

	UpdateText();
}

void dae::ScoreDisplayComponent::Notify(Event e, GameObject* pSubjectActor)
{
	if ((e == Event::PlayerScoreSmallChanged || e == Event::PlayerScoreLargeChanged) 
		&& m_pScoreComponent 
		&& pSubjectActor == m_pScoreComponent->GetOwner())
	{
		UpdateText();
	}
}

void dae::ScoreDisplayComponent::UpdateText()
{
	if (m_pTextComponent && m_pScoreComponent)
	{
		m_pTextComponent->SetText("# Score: " + std::to_string(m_pScoreComponent->GetScore()));
	}
}