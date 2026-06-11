#include "PlayfieldGrid.h"
#include <algorithm>
#include <cmath>

namespace dae
{
	void PlayfieldGrid::Initialize(int columns, int rows)
	{
		m_OccupiedTiles.assign(rows, std::vector<bool>(columns, false));
		m_AliveEnemyCount = 0;
	}

	void PlayfieldGrid::SetOccupied(int row, int column, bool occupied)
	{
		if (row >= 0 && static_cast<size_t>(row) < m_OccupiedTiles.size() &&
			column >= 0 && static_cast<size_t>(column) < m_OccupiedTiles[row].size())
		{
			m_OccupiedTiles[row][column] = occupied;
		}
	}

	void PlayfieldGrid::ClearTile(float localX, float localY, float tileWorldSize)
	{
		constexpr float kEpsilon = 0.001f;
		const int col = static_cast<int>(localX / tileWorldSize + kEpsilon);
		const int row = static_cast<int>(localY / tileWorldSize + kEpsilon);
		SetOccupied(row, col, false);
	}

	bool PlayfieldGrid::IsReservedTile(int column, int row, const std::vector<glm::ivec2>& reservedTiles) const
	{
		for (const auto& tile : reservedTiles)
		{
			if (tile.x == column && tile.y == row)
			{
				return true;
			}
		}
		return false;
	}
}
