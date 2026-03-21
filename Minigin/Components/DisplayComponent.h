#pragma once
#include "BaseComponent.h"
#include "TextComponent.h"
#include "../EventQueue/IObserver.h"
#include "../GameObject.h"
#include <string>
#include <functional>

namespace dae
{
	template<typename T>
	class DisplayComponent final : public BaseComponent, public IObserver
	{
	public:
		DisplayComponent(GameObject* pOwner, TextComponent* pTextComponent, EventId observedEvent, const std::string& prefix, std::function<std::string(const T*)> valueAccessor)
			: BaseComponent(pOwner)
			, m_pTextComponent(pTextComponent)
			, m_ObservedEvent(observedEvent)
			, m_Prefix(prefix)
			, m_ValueAccessor(valueAccessor)
		{
		}

		virtual ~DisplayComponent() = default;

		DisplayComponent(const DisplayComponent& other) = delete;
		DisplayComponent(DisplayComponent&& other) = delete;
		DisplayComponent& operator=(const DisplayComponent& other) = delete;
		DisplayComponent& operator=(DisplayComponent&& other) = delete;

		virtual void Update() override {};

		virtual void Notify(const GameObject& pGameActor, Event event) override
		{
			if (event.id == m_ObservedEvent)
			{
				if (m_pTextComponent)
				{
					auto comp = pGameActor.GetComponent<T>();
					if (comp)
					{
						m_pTextComponent->SetText(m_Prefix + m_ValueAccessor(comp));
					}
				}
			}
		}

	private:
		TextComponent* m_pTextComponent;
		EventId m_ObservedEvent;
		std::string m_Prefix;
		std::function<std::string(const T*)> m_ValueAccessor;
	};
}
