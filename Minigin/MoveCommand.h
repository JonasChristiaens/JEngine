#pragma once
#include "Command.h"
#include <glm/glm.hpp>

namespace dae
{
	class SpriteAnimatorComponent;
}

class MoveCommand final : public Command
{
public:
	MoveCommand(const glm::vec3& direction, float speed);
	virtual ~MoveCommand() = default;

	void Execute() override;

private:
	glm::vec3 m_direction;
	float m_speed;
	dae::SpriteAnimatorComponent* m_pAnimator{ nullptr };
};
