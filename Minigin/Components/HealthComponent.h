#pragma once
#include "BaseComponent.h"
#include "../EventQueue/ISubject.h"

namespace dae
{
	class HealthComponent final : public BaseComponent, public ISubject
	{
	public:
		HealthComponent(GameObject* pOwner, int health = 1);
		virtual ~HealthComponent() = default;

		HealthComponent(const HealthComponent& other) = delete;
		HealthComponent(HealthComponent&& other) = delete;
		HealthComponent& operator=(const HealthComponent& other) = delete;
		HealthComponent& operator=(HealthComponent&& other) = delete;

		virtual void Update() override {};
		void ChangeCurrentHealth(int amount);

		int GetHealth() const { return m_CurrentHealth; }
			
	private:
		int m_CurrentHealth;
	};
}