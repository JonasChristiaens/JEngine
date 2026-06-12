#include "MoveCommand.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/SkateComponent.h"
#include "Core/GameTime.h"
#include "Rendering/Renderer.h"
#include <algorithm>
#include <glm/geometric.hpp>

namespace dae
{
	MoveCommand::MoveCommand(const glm::vec3& direction, float speed)
		: m_Direction(glm::length(direction) > 0.0f ? glm::normalize(direction) : direction)
		, m_Speed(speed)
	{}

	void MoveCommand::Execute()
	{
		if (m_pGameActor == nullptr || m_pGameActor->IsMarkedForDeletion())
			return;

		auto* health = m_pGameActor->GetComponent<HealthComponent>();
		if (health && health->IsDead())
			return;

		const float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

		auto transform = m_pGameActor->GetComponent<dae::TransformComponent>();
		if (transform == nullptr)
			return;

		float speed = m_Speed;
		if (auto* skate = m_pGameActor->GetComponent<SkateComponent>())
		{
			if (skate->HasSkate())
				speed *= 1.5f;
		}
		glm::vec3 movement = m_Direction * speed * deltaTime;
		const auto currentPosition = transform->GetLocalPosition();
		const auto nextPosition = currentPosition + movement;
		const auto currentWorldPosition = transform->GetWorldPosition();
		const auto nextWorldPosition = currentWorldPosition + movement;
		if (auto* collider = m_pGameActor->GetComponent<dae::CollisionComponent>())
		{
			if (!collider->WouldCollide(nextWorldPosition))
			{
				transform->SetLocalPosition(nextPosition);
			}
		}
		else
		{
			transform->SetLocalPosition(nextPosition);
		}

		ClampToCameraBounds(transform);
	}

	void MoveCommand::ClampToCameraBounds(dae::TransformComponent* transform)
	{
		float cameraOffsetX = 0.0f;
		float cameraOffsetY = 0.0f;
		Renderer::GetInstance().GetCameraOffset(cameraOffsetX, cameraOffsetY);
		const auto windowSize = Renderer::GetInstance().GetWindowSize();
		const float visibleLeft = -cameraOffsetX;
		const float visibleRight = -cameraOffsetX + static_cast<float>(windowSize.x);

		glm::vec3 pos = transform->GetLocalPosition();
		pos.x = std::clamp(pos.x, visibleLeft, visibleRight);
		transform->SetLocalPosition(pos);
	}
}
