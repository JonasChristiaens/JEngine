#include "MoveCommand.h"
#include "GameObject.h"
#include "TransformComponent.h"

MoveCommand::MoveCommand(const glm::vec3& direction, float speed)
	: m_direction(direction)
	, m_speed(speed)
{
}

void MoveCommand::Execute()
{
	if (m_pGameActor == nullptr)
		return;

	auto transform = m_pGameActor->GetComponent<dae::TransformComponent>();
	if (transform == nullptr)
		return;

	glm::vec3 movement = m_direction * m_speed;
	transform->SetLocalPosition(transform->GetLocalPosition() + movement);
}
