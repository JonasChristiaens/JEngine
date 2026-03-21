#pragma once
#include "BaseComponent.h"
#include <functional>
#include <vector>

namespace dae
{
	class CollisionComponent final : public BaseComponent
	{
	public:
		CollisionComponent(GameObject* pOwner, float width, float height);
		virtual ~CollisionComponent();

		void Update() override;

		void SetOnCollisionCallback(std::function<void(GameObject*)> callback) { m_onCollision = callback; }

		float GetWidth() const { return m_width; }
		float GetHeight() const { return m_height; }

	private:
		float m_width;
		float m_height;
		std::function<void(GameObject*)> m_onCollision{ nullptr };

		static std::vector<CollisionComponent*> m_allColliders;
	};
}
