#pragma once
#include "Singleton.h"

namespace dae
{
	class GameTime final : public Singleton<GameTime>
	{
	public:
		float GetDeltaTime() const noexcept { return m_DeltaTime; }
		void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; m_TotalTime += deltaTime; }
		float GetTotalTime() const noexcept { return m_TotalTime; }

	private:
		friend class Singleton<GameTime>;
		GameTime() = default;

		float m_DeltaTime{ 0.0f };
		float m_TotalTime{ 0.0f };
	};
}
