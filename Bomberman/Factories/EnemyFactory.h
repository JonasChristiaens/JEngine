#pragma once
#include <vector>
#include <glm/vec3.hpp>

namespace dae
{
    class GameObject;
    class Scene;

    namespace EnemyFactory
    {
        GameObject* CreateBalloom(Scene& scene, GameObject& parent, int gridColumns, int gridRows, float tileWorldSize, float moveSpeed, const std::vector<glm::vec3>& reservedWorldPositions = {});
    }
}
