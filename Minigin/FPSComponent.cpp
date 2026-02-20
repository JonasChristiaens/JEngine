#include "FPSComponent.h"
#include "GameObject.h"
#include "TextComponent.h"
#include <sstream>
#include <iomanip>

dae::FPSComponent::FPSComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
	// Get the TextComponent from the same GameObject
	m_textComponent = GetOwner()->GetComponent<TextComponent>();
}

void dae::FPSComponent::Update(float deltaTime)
{
	// Accumulate time and frame count
	m_timeSinceLastUpdate += deltaTime;
	m_frameCount++;

	// Update FPS display at the specified interval
	if (m_timeSinceLastUpdate >= m_fpsUpdateInterval)
	{
		// Calculate FPS
		m_currentFPS = static_cast<float>(m_frameCount) / m_timeSinceLastUpdate;

		// Update the text component if available
		if (m_textComponent != nullptr)
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(1) << m_currentFPS << " FPS";
			m_textComponent->SetText(ss.str());
		}

		// Reset counters
		m_frameCount = 0;
		m_timeSinceLastUpdate = 0.0f;
	}
}