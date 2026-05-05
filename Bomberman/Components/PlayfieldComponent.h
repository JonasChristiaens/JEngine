#pragma once
#include "BaseComponent.h"
#include <vector>

namespace dae
{
	class Scene;

	class PlayfieldComponent final : public BaseComponent
	{
	public:
		PlayfieldComponent(GameObject* pOwner, Scene& scene, float playfieldWidth, float playfieldHeight, float playfieldScale);
		virtual ~PlayfieldComponent() = default;

		void Update() override {}
		const std::vector<std::vector<bool>>& GetOccupiedTiles() const { return m_occupiedTiles; }

	private:
		Scene* m_pScene{};
		float m_playfieldWidth{};
		float m_playfieldHeight{};
		float m_playfieldScale{};
		std::vector<std::vector<bool>> m_occupiedTiles{};

		void BuildPlayfield();
	};
}
