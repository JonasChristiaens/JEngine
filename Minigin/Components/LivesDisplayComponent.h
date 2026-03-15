#pragma once
#include "BaseComponent.h"
#include "EventQueue/Observer.h"

namespace dae
{
	class TextComponent;
	class HealthComponent;

	class LivesDisplayComponent final : public BaseComponent, public Observer
	{
	public:
		LivesDisplayComponent(GameObject* pOwner, HealthComponent* pHealthComponent);
		virtual ~LivesDisplayComponent() = default;

		LivesDisplayComponent(const LivesDisplayComponent& other) = delete;
		LivesDisplayComponent(LivesDisplayComponent&& other) = delete;
		LivesDisplayComponent& operator=(const LivesDisplayComponent& other) = delete;
		LivesDisplayComponent& operator=(LivesDisplayComponent&& other) = delete;

		void Update() override {}
		void Notify(Event e, GameObject* pSubjectActor) override;

	private:
		TextComponent* m_pTextComponent;
		HealthComponent* m_pHealthComponent;

		void UpdateText();
	};
}