#pragma once
#include "Singleton.h"

namespace dae
{
	class GameTime final : public Singleton<GameTime>
	{
	public:
		float GetDeltaTime() const { return m_DeltaTime; }
		void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }

	private:
		friend class Singleton<GameTime>;
		GameTime() = default;

		float m_DeltaTime{ 0.0f };
	};
}
