#include "MoveCommand.h"
#include "Scene/GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "Components/HealthComponent.h"
#include "Core/GameTime.h"
#include <glm/geometric.hpp>

namespace dae
{
	MoveCommand::MoveCommand(const glm::vec3& direction, float speed)
		: m_Direction(glm::length(direction) > 0.0f ? glm::normalize(direction) : direction)
		, m_Speed(speed)
	{}

	void MoveCommand::Execute()
	{
		if (m_pGameActor == nullptr)
			return;

		auto* health = m_pGameActor->GetComponent<HealthComponent>();
		if (health && health->IsDead())
			return;

		const float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

		auto transform = m_pGameActor->GetComponent<dae::TransformComponent>();
		if (transform == nullptr)
			return;

		glm::vec3 movement = m_Direction * m_Speed * deltaTime;
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
	}
}
