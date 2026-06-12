#pragma once
#include "Command.h"
#include <glm/glm.hpp>

namespace dae
{
	class TransformComponent;

	class MoveCommand final : public Command
	{
	public:
		MoveCommand(const glm::vec3& direction, float speed);
		virtual ~MoveCommand() = default;

		void Execute() override;

	private:
		static void ClampToCameraBounds(TransformComponent* transform);

		glm::vec3 m_Direction;
		float m_Speed;
	};
}
