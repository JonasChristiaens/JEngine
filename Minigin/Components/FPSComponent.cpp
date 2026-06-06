#include "FPSComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "GameTime.h"
#include <sstream>
#include <iomanip>

dae::FPSComponent::FPSComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
	// Get the TextComponent from the same GameObject
	m_TextComponent = GetOwner()->GetComponent<TextComponent>();
}

void dae::FPSComponent::Update()
{
	const float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

	// Accumulate time and frame count
	m_TimeSinceLastUpdate += deltaTime;
	m_FrameCount++;

	// Update FPS display at the specified interval
	if (m_TimeSinceLastUpdate >= m_FpsUpdateInterval)
	{
		// Calculate FPS
		m_CurrentFPS = static_cast<float>(m_FrameCount) / m_TimeSinceLastUpdate;

		// Update the text component if available
		if (m_TextComponent != nullptr)
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(1) << m_CurrentFPS << " FPS";
			m_TextComponent->SetText(ss.str());
		}

		// Reset counters
		m_FrameCount = 0;
		m_TimeSinceLastUpdate = 0.0f;
	}
}