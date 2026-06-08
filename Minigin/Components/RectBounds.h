#pragma once

namespace dae
{
	struct RectBounds
	{
		float left{};
		float right{};
		float top{};
		float bottom{};

		static RectBounds FromOffset(float x, float y, float width, float height, float offsetX, float offsetY)
		{
			return { x + offsetX, x + offsetX + width, y + offsetY, y + offsetY + height };
		}

		static RectBounds FromCenterSize(float centerX, float centerY, float width, float height)
		{
			const float halfW = width * 0.5f;
			const float halfH = height * 0.5f;
			return { centerX - halfW, centerX + halfW, centerY - halfH, centerY + halfH };
		}

		bool Overlaps(const RectBounds& other) const
		{
			return left < other.right && right > other.left && top < other.bottom && bottom > other.top;
		}

		bool Contains(float px, float py) const
		{
			return px >= left && px < right && py >= top && py < bottom;
		}
	};
}
