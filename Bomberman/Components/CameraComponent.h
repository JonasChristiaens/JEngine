#pragma once
#include "BaseComponent.h"

namespace dae
{
	class GameObject;

	class CameraComponent final : public BaseComponent
	{
	public:
		CameraComponent(GameObject* pOwner, GameObject* pTarget, float windowWidth, float playfieldWidth);
		virtual ~CameraComponent() = default;

		void Update() override;

	private:
		GameObject* m_pTarget{};
		float m_windowWidth{};
		float m_playfieldWidth{};
	};
}
