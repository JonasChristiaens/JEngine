#pragma once
#include "BaseComponent.h"

namespace dae
{
	class TransformComponent;

	class RotateComponent final : public BaseComponent
	{
	public:
		RotateComponent(GameObject* pOwner, float radius, float speed);
		virtual ~RotateComponent() = default;

		RotateComponent(const RotateComponent& other) = delete;
		RotateComponent(RotateComponent&& other) = delete;
		RotateComponent& operator=(const RotateComponent& other) = delete;
		RotateComponent& operator=(RotateComponent&& other) = delete;

		void Update() override;

		void SetRadius(float radius) { m_Radius = radius; }
		void SetSpeed(float speed) { m_Speed = speed; }

		float GetRadius() const { return m_Radius; }
		float GetSpeed() const { return m_Speed; }

	private:
		TransformComponent* m_pTransform{ nullptr };
		float m_Radius;
		float m_Speed;
		float m_Angle{ 0.0f };
	};
}