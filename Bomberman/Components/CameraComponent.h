#pragma once
#include "BaseComponent.h"

namespace dae
{
	class GameObject;
	class TransformComponent;

	class CameraComponent final : public BaseComponent
	{
	public:
		CameraComponent(GameObject* pOwner, GameObject* pTarget, float windowWidth, float playfieldWidth);
		~CameraComponent() = default;

		void Update() override;
		void SetSecondTarget(GameObject* pTarget) { m_pSecondTarget = pTarget; }

	private:
		GameObject* m_pTarget{};
		GameObject* m_pSecondTarget{};
		float m_WindowWidth{};
		float m_PlayfieldWidth{};
		TransformComponent* m_pTransform{};
	};
}
