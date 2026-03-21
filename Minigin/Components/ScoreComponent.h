#pragma once
#include "BaseComponent.h"
#include "../EventQueue/ISubject.h"

namespace dae
{
	class ScoreComponent final : public BaseComponent, public ISubject
	{
	public:
		ScoreComponent(GameObject* pOwner, int score = 0);
		virtual ~ScoreComponent() = default;

		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		virtual void Update() override {};
		void ChangeCurrentScore(int amount);

		int GetScore() const { return m_CurrentScore; }

	private:
		int m_CurrentScore;
	};
}