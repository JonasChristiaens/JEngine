#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace dae
{
	class PlayfieldGrid final
	{
	public:
		void Initialize(int columns, int rows);
		void SetOccupied(int row, int column, bool occupied = true);
		void ClearTile(float localX, float localY, float tileWorldSize);
		bool IsReservedTile(int column, int row, const std::vector<glm::ivec2>& reservedTiles) const;

		const std::vector<std::vector<bool>>& GetGrid() const { return m_OccupiedTiles; }

		void RegisterEnemySpawned() { ++m_AliveEnemyCount; }
		void DecrementEnemyCount() { if (m_AliveEnemyCount > 0) --m_AliveEnemyCount; }
		bool AreAllEnemiesDead() const { return m_AliveEnemyCount <= 0; }

	private:
		std::vector<std::vector<bool>> m_OccupiedTiles{};
		int m_AliveEnemyCount{ 0 };
	};
}
