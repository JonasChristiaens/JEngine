#pragma once
#include "BaseComponent.h"
#include <functional>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace dae
{
	class TransformComponent;
	class CollisionGrid;

	class CollisionComponent final : public BaseComponent
	{
		friend class CollisionGrid;

	public:
		CollisionComponent(GameObject* pOwner, float width, float height, bool isTrigger = false);
		~CollisionComponent() override;

		void Update() override;

		void SetOnCollisionCallback(std::function<void(GameObject*)> callback) { m_OnCollision = callback; }

		float GetWidth() const noexcept { return m_Width; }
		float GetHeight() const noexcept { return m_Height; }
		bool IsTrigger() const noexcept { return m_IsTrigger; }
		const glm::vec2& GetOffset() const noexcept { return m_Offset; }
		void SetOffset(const glm::vec2& offset);
		bool WouldCollide(const glm::vec3& worldPosition) const;

		static bool RectOverlaps(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
		{
			return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
		}

	private:
		float m_Width;
		float m_Height;
		bool m_IsTrigger;
		glm::vec2 m_Offset{ 0.0f, 0.0f };
		std::function<void(GameObject*)> m_OnCollision{ nullptr };
		mutable TransformComponent* m_pTransform{};
		int m_LastQueryId{ 0 };
		mutable bool m_Registered{ false };
		mutable glm::vec3 m_LastRegisteredPos{ -1.0f, -1.0f, -1.0f };
	};
}
