#pragma once
#include "BaseComponent.h"

namespace dae
{
	class SkateComponent final : public BaseComponent
	{
	public:
		explicit SkateComponent(GameObject* pOwner);

		void Update() override {}

		bool HasSkate() const noexcept { return m_HasSkate; }
		void SetHasSkate(bool has) noexcept { m_HasSkate = has; }

	private:
		bool m_HasSkate{ false };
	};
}
