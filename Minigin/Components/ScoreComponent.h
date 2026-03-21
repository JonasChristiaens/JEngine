#pragma once
#include "BaseComponent.h"
#include "../EventQueue/ISubject.h"
#include "../EventQueue/IObserver.h"

namespace dae
{
	class ScoreComponent final : public BaseComponent, public ISubject, public IObserver
	{
	public:
		ScoreComponent(GameObject* pOwner, int score = 0);
		virtual ~ScoreComponent();

		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		virtual void Update() override {};
		void ChangeCurrentScore(int amount);

		int GetScore() const { return m_CurrentScore; }

		virtual void Notify(const GameObject& pGameActor, Event event) override;

	private:
		int m_CurrentScore;
		bool m_WonTriggered{ false };
	};
}