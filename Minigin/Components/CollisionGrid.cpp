#include "CollisionGrid.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "../Scene/GameObject.h"
#include <algorithm>

namespace dae
{
	void CollisionGrid::Initialize(float originX, float originY, float cellSize, int cols, int rows)
	{
		s_CellSize = cellSize;
		s_OriginX = originX;
		s_OriginY = originY;
		s_Cols = cols;
		s_Rows = rows;
		s_Cells.assign(static_cast<size_t>(rows), std::vector<std::vector<CollisionComponent*>>(static_cast<size_t>(cols)));
	}

	void CollisionGrid::Register(const CollisionComponent* pCollider)
	{
		if (s_Cells.empty())
			return;

		const auto* pOwner = pCollider->GetOwner();
		if (!pOwner || pOwner->IsMarkedForDeletion())
			return;

		auto* pTx = pOwner->GetComponent<TransformComponent>();
		if (!pTx)
			return;

		const auto& pos = pTx->GetWorldPosition();
		const auto& offset = pCollider->GetOffset();
		const float left = pos.x + offset.x - s_OriginX;
		const float top = pos.y + offset.y - s_OriginY;
		const float right = left + pCollider->GetWidth();
		const float bottom = top + pCollider->GetHeight();

		const int colStart = std::max(0, static_cast<int>(left / s_CellSize));
		const int colEnd = std::min(s_Cols - 1, static_cast<int>((right - 0.001f) / s_CellSize));
		const int rowStart = std::max(0, static_cast<int>(top / s_CellSize));
		const int rowEnd = std::min(s_Rows - 1, static_cast<int>((bottom - 0.001f) / s_CellSize));

		for (int r = rowStart; r <= rowEnd; ++r)
		{
			for (int c = colStart; c <= colEnd; ++c)
			{
				s_Cells[static_cast<size_t>(r)][static_cast<size_t>(c)].push_back(const_cast<CollisionComponent*>(pCollider));
			}
		}
	}

	void CollisionGrid::Unregister(const CollisionComponent* pCollider)
	{
		for (auto& row : s_Cells)
		{
			for (auto& cell : row)
			{
				cell.erase(std::remove(cell.begin(), cell.end(), const_cast<CollisionComponent*>(pCollider)), cell.end());
			}
		}
	}

	void CollisionGrid::Query(const glm::vec3& worldPos, float width, float height, std::vector<CollisionComponent*>& outResults)
	{
		if (s_Cells.empty())
			return;

		++s_QueryId;

		const float left = worldPos.x - s_OriginX;
		const float top = worldPos.y - s_OriginY;
		const float right = left + width;
		const float bottom = top + height;

		const int colStart = std::max(0, static_cast<int>(left / s_CellSize));
		const int colEnd = std::min(s_Cols - 1, static_cast<int>((right - 0.001f) / s_CellSize));
		const int rowStart = std::max(0, static_cast<int>(top / s_CellSize));
		const int rowEnd = std::min(s_Rows - 1, static_cast<int>((bottom - 0.001f) / s_CellSize));

		for (int r = rowStart; r <= rowEnd; ++r)
		{
			for (int c = colStart; c <= colEnd; ++c)
			{
				for (auto* pOther : s_Cells[static_cast<size_t>(r)][static_cast<size_t>(c)])
				{
					if (pOther->m_LastQueryId != s_QueryId)
					{
						pOther->m_LastQueryId = s_QueryId;
						outResults.push_back(pOther);
					}
				}
			}
		}
	}

	void CollisionGrid::Cleanup()
	{
		for (auto& row : s_Cells)
		{
			for (auto& cell : row)
			{
				cell.clear();
			}
		}
		s_Cells.clear();
	}
}
