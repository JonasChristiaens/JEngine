#pragma once
#include <vector>
#include <glm/vec3.hpp>

namespace dae
{
	class CollisionComponent;

	class CollisionGrid final
	{
	public:
		static void Initialize(float originX, float originY, float cellSize, int cols, int rows);
		static void Register(const CollisionComponent* pCollider);
		static void Unregister(const CollisionComponent* pCollider);
		static void Query(const glm::vec3& worldPos, float width, float height, std::vector<CollisionComponent*>& outResults);
		static void Cleanup();

	private:
		inline static std::vector<std::vector<std::vector<CollisionComponent*>>> s_Cells{};
		inline static float s_CellSize{};
		inline static float s_OriginX{};
		inline static float s_OriginY{};
		inline static int s_Cols{};
		inline static int s_Rows{};
		inline static int s_QueryId{};
	};
}
