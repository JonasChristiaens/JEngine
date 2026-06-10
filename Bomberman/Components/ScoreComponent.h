#pragma once
#include "BaseComponent.h"
#include "EventQueue/Subject.h"
#include "EventQueue/IObserver.h"

namespace dae
{
	class ScoreComponent final : public BaseComponent, public Subject, public IObserver
	{
	public:
		ScoreComponent(GameObject* pOwner, int score = 0);
		virtual ~ScoreComponent();

		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		void Update() override {};
		void ChangeCurrentScore(int amount);

		int GetScore() const { return m_CurrentScore; }

		void Notify(GameObject& actor, Event event) override;

	private:
		int m_CurrentScore;
	};
}