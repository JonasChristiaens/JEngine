#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "Components/CollisionComponent.h"
#include "GameTime.h"
#include <glm/geometric.hpp>

MoveCommand::MoveCommand(const glm::vec3& direction, float speed)
	: m_direction(glm::length(direction) > 0.0f ? glm::normalize(direction) : direction)
	, m_speed(speed)
{}

void MoveCommand::Execute()
{
	float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

	if (m_pGameActor == nullptr)
		return;

	auto transform = m_pGameActor->GetComponent<dae::TransformComponent>();
	if (transform == nullptr)
		return;

	glm::vec3 movement = m_direction * m_speed * deltaTime;
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
