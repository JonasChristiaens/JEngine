#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent final : public BaseComponent
	{
	public:
		TransformComponent(GameObject* pOwner);
		virtual ~TransformComponent() = default;

		void Update(float /*deltaTime*/) override {}

		const glm::vec3& GetPosition() const { return m_position; }
		void SetPosition(float x, float y, float z = 0.0f);
		void SetPosition(const glm::vec3& position);

	private:
		glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
	};
}