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

		void Update() override;

	private:
		float m_FpsUpdateInterval{ 0.1f };
		float m_TimeSinceLastUpdate{ 0.0f };
		float m_CurrentFPS{ 0.0f };
		int m_FrameCount{ 0 };

		TextComponent* m_TextComponent{ nullptr };
	};
}