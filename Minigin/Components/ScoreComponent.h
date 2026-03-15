#pragma once
#include "BaseComponent.h"
#include "EventQueue/Subject.h"

namespace dae
{
	class ScoreComponent final : public BaseComponent, public Subject
	{
	public:
		ScoreComponent(GameObject* pOwner, int startScore = 0);
		virtual ~ScoreComponent() = default;

		ScoreComponent(const ScoreComponent& other) = delete;
		ScoreComponent(ScoreComponent&& other) = delete;
		ScoreComponent& operator=(const ScoreComponent& other) = delete;
		ScoreComponent& operator=(ScoreComponent&& other) = delete;

		void Update() override {}

		void ChangeScore(int amount);
		int GetScore() const { return m_score; }

	private:
		int m_score;
	};
}