#pragma once
#include "BaseComponent.h"

namespace dae
{
	class DetonatorComponent final : public BaseComponent
	{
	public:
		explicit DetonatorComponent(GameObject* pOwner);

		void Update() override;

		bool HasDetonator() const noexcept { return m_HasDetonator; }
		void SetHasDetonator(bool has) noexcept { m_HasDetonator = has; }

		void StartDetonateChain();
		void StopDetonateChain();

	private:
		static constexpr float kChainInterval{ 0.15f };

		bool m_HasDetonator{ false };
		bool m_ChainActive{ false };
		float m_ChainCooldown{ 0.0f };
	};
}
