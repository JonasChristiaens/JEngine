#pragma once
#include "BaseComponent.h"
#include "EventQueue/Subject.h"

namespace dae
{
	class HealthComponent final : public BaseComponent, public Subject
	{
	public:
		HealthComponent(GameObject* pOwner, int lives = 3);
		virtual ~HealthComponent() = default;

		HealthComponent(const HealthComponent& other) = delete;
		HealthComponent(HealthComponent&& other) = delete;
		HealthComponent& operator=(const HealthComponent& other) = delete;
		HealthComponent& operator=(HealthComponent&& other) = delete;

		void Update() override {}

		void ChangeHealth(int amount);
		int GetLives() const { return m_lives; }

	private:
		int m_lives;
	};
}