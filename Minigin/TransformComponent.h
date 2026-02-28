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

		void Update(float /*deltaTime*/) override;

		const glm::vec3& GetWorldPosition();
		const glm::vec3& GetLocalPosition() const { return m_localPosition; }

		void SetLocalPosition(float x, float y, float z = 0.0f);
		void SetLocalPosition(const glm::vec3& position);

		void SetPositionDirty();
		void UpdateWorldPosition();

	private:
		glm::vec3 m_localPosition{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_worldPosition{ 0.0f, 0.0f, 0.0f };
		bool m_positionIsDirty{ false };
	};
}