#pragma once
#include "BaseComponent.h"

namespace dae
{
	class DetonatorComponent final : public BaseComponent
	{
	public:
		explicit DetonatorComponent(GameObject* pOwner);

		void Update() override {}

		bool HasDetonator() const noexcept { return m_HasDetonator; }
		void SetHasDetonator(bool has) noexcept { m_HasDetonator = has; }

	private:
		bool m_HasDetonator{ false };
	};
}
