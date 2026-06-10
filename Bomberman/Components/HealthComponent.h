#pragma once
#include "BaseComponent.h"
#include "EventQueue/ISubject.h"
#include "EventQueue/IObserver.h"

namespace dae
{
	class HealthComponent final : public BaseComponent, public ISubject, public IObserver
	{
	public:
		HealthComponent(GameObject* pOwner, int health = 1);
		virtual ~HealthComponent();

		HealthComponent(const HealthComponent& other) = delete;
		HealthComponent(HealthComponent&& other) = delete;
		HealthComponent& operator=(const HealthComponent& other) = delete;
		HealthComponent& operator=(HealthComponent&& other) = delete;

		void Update() override;
		void ChangeCurrentHealth(int amount);

		int GetHealth() const { return m_CurrentHealth; }
		bool IsDead() const { return m_IsDead; }

		void Notify(GameObject& actor, Event event) override;

	private:
		int m_CurrentHealth;
		bool m_IsDead{ false };
		float m_InvulnerabilityTimer{ 0.0f };
	};
}