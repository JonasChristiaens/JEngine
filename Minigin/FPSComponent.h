#pragma once
#include "BaseComponent.h"
#include <chrono>

namespace dae
{
	class TextComponent;

	class FPSComponent final : public BaseComponent
	{
	public:
		FPSComponent(GameObject* pOwner);
		virtual ~FPSComponent() = default;

		void Update(float deltaTime) override;

	private:
		float m_fpsUpdateInterval{ 0.1f };
		float m_timeSinceLastUpdate{ 0.0f };
		float m_currentFPS{ 0.0f };
		int m_frameCount{ 0 };

		TextComponent* m_textComponent{ nullptr };
	};
}