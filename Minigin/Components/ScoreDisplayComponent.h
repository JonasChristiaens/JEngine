#pragma once
#include "BaseComponent.h"
#include "EventQueue/Observer.h"

namespace dae
{
	class TextComponent;
	class ScoreComponent;

	class ScoreDisplayComponent final : public BaseComponent, public Observer
	{
	public:
		ScoreDisplayComponent(GameObject* pOwner, ScoreComponent* pScoreComponent);
		virtual ~ScoreDisplayComponent() = default;

		ScoreDisplayComponent(const ScoreDisplayComponent& other) = delete;
		ScoreDisplayComponent(ScoreDisplayComponent&& other) = delete;
		ScoreDisplayComponent& operator=(const ScoreDisplayComponent& other) = delete;
		ScoreDisplayComponent& operator=(ScoreDisplayComponent&& other) = delete;

		void Update() override {}
		void Notify(Event e, GameObject* pSubjectActor) override;

	private:
		TextComponent* m_pTextComponent;
		ScoreComponent* m_pScoreComponent;

		void UpdateText();
	};
}