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

		virtual void Update() override;

		void SetRadius(float radius) { m_radius = radius; }
		void SetSpeed(float speed) { m_speed = speed; }

		float GetRadius() const { return m_radius; }
		float GetSpeed() const { return m_speed; }

	private:
		TransformComponent* m_pTransform{ nullptr };
		float m_radius;
		float m_speed;
		float m_angle{ 0.0f };
	};
}