#pragma once
#include "BaseComponent.h"

namespace dae
{
	class TransformComponent;

	class MoveComponent final : public BaseComponent
	{
	public:
		MoveComponent(GameObject* pOwner, float radius, float speed);
		virtual ~MoveComponent() = default;

		MoveComponent(const MoveComponent& other) = delete;
		MoveComponent(MoveComponent&& other) = delete;
		MoveComponent& operator=(const MoveComponent& other) = delete;
		MoveComponent& operator=(MoveComponent&& other) = delete;

		virtual void Update(float deltaTime) override;

		void SetRadius(float radius) { m_radius = radius; }
		void SetSpeed(float speed) { m_speed = speed; }
		void SetRotationCenter(float x, float y) { m_centerX = x; m_centerY = y; }

		float GetRadius() const { return m_radius; }
		float GetSpeed() const { return m_speed; }

	private:
		TransformComponent* m_pTransform{ nullptr };
		float m_radius;
		float m_speed;
		float m_angle{ 0.0f };
		float m_centerX{ 0.0f };
		float m_centerY{ 0.0f };
	};
}