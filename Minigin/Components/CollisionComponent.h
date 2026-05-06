#pragma once
#include "BaseComponent.h"
#include <functional>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace dae
{
	class CollisionComponent final : public BaseComponent
	{
	public:
		CollisionComponent(GameObject* pOwner, float width, float height, bool isTrigger = false);
		virtual ~CollisionComponent();

		void Update() override;
		void Render() const override;

		void SetOnCollisionCallback(std::function<void(GameObject*)> callback) { m_onCollision = callback; }

		float GetWidth() const { return m_width; }
		float GetHeight() const { return m_height; }
		bool IsTrigger() const { return m_isTrigger; }
        const glm::vec2& GetOffset() const { return m_offset; }
		void SetOffset(const glm::vec2& offset);
		bool WouldCollide(const glm::vec3& worldPosition) const;

	private:
		float m_width;
		float m_height;
		bool m_isTrigger;
		glm::vec2 m_offset{ 0.0f, 0.0f };
		std::function<void(GameObject*)> m_onCollision{ nullptr };

		static std::vector<CollisionComponent*> m_allColliders;
	};
}
