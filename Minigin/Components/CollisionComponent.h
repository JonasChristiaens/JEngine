#pragma once
#include "BaseComponent.h"
#include <functional>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace dae
{
	class TransformComponent;

	class CollisionComponent final : public BaseComponent
	{
	public:
		CollisionComponent(GameObject* pOwner, float width, float height, bool isTrigger = false);
		virtual ~CollisionComponent();

		void Update() override;
		void Render() const override;

		void SetOnCollisionCallback(std::function<void(GameObject*)> callback) { m_OnCollision = callback; }

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }
		bool IsTrigger() const { return m_IsTrigger; }
        const glm::vec2& GetOffset() const { return m_Offset; }
		void SetOffset(const glm::vec2& offset);
		bool WouldCollide(const glm::vec3& worldPosition) const;

	private:
		float m_Width;
		float m_Height;
		bool m_IsTrigger;
		glm::vec2 m_Offset{ 0.0f, 0.0f };
		std::function<void(GameObject*)> m_OnCollision{ nullptr };
		mutable TransformComponent* m_pTransform{};

		static std::vector<CollisionComponent*> m_AllColliders;
	};
}
