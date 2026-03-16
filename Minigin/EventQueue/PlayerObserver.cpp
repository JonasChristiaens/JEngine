#include "PlayerObserver.h"
#include "GameObject.h"
#include "Components/HealthComponent.h"
#include "Components/ScoreComponent.h"

void dae::PlayerObserver::Notify(Event e, GameObject* pSubjectActor)
{
	if (!pSubjectActor) return;

	switch (e)
	{
	case Event::PlayerDamaged:
		if (auto pHealth = pSubjectActor->GetComponent<HealthComponent>())
		{
			pHealth->ChangeHealth(-1);
		}
		break;
	case Event::PlayerScoreSmallChanged:
		if (auto pScore = pSubjectActor->GetComponent<ScoreComponent>())
		{
			pScore->ChangeScore(10);
		}
		break;
	case Event::PlayerScoreLargeChanged:
		if (auto pScore = pSubjectActor->GetComponent<ScoreComponent>())
		{
			pScore->ChangeScore(100);
		}
		break;
	default:
		break;
	}
}