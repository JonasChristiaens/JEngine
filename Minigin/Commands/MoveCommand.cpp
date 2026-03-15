#include "MoveCommand.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "GameTime.h"
#include <glm/geometric.hpp>

MoveCommand::MoveCommand(const glm::vec3& direction, float speed)
	: m_direction(glm::length(direction) > 0.0f ? glm::normalize(direction) : direction)
	, m_speed(speed)
{
}

void MoveCommand::Execute()
{
	float deltaTime = dae::GameTime::GetInstance().GetDeltaTime();

	if (m_pGameActor == nullptr)
		return;

	auto transform = m_pGameActor->GetComponent<dae::TransformComponent>();
	if (transform == nullptr)
		return;

	glm::vec3 movement = m_direction * m_speed * deltaTime;
	transform->SetLocalPosition(transform->GetLocalPosition() + movement);
}
