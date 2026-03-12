#pragma once
#include "Singleton.h"

namespace dae
{
	class GameTime final : public Singleton<GameTime>
	{
	public:
		float GetDeltaTime() const { return m_deltaTime; }
		void SetDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }

	private:
		friend class Singleton<GameTime>;
		GameTime() = default;

		float m_deltaTime{ 0.0f };
	};
}
