#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent final : public BaseComponent
	{
	public:
		explicit TransformComponent(GameObject* pOwner);
		virtual ~TransformComponent() = default;

		void Update() override;

		const glm::vec3& GetWorldPosition() const;
		const glm::vec3& GetLocalPosition() const { return m_LocalPosition; }

		void SetLocalPosition(float x, float y, float z = 0.0f);
		void SetLocalPosition(const glm::vec3& position);

		void SetPositionDirty();
		void UpdateWorldPosition() const;

	private:
		glm::vec3 m_LocalPosition{ 0.0f, 0.0f, 0.0f };
		mutable glm::vec3 m_WorldPosition{ 0.0f, 0.0f, 0.0f };
		mutable bool m_PositionIsDirty{ false };
	};
}