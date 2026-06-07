#pragma once
#include "EventQueue/IObserver.h"
#include <glm/vec2.hpp>
#include <unordered_map>
#include <vector>

namespace dae
{
	class Scene;

	class BombEventObserver final : public IObserver
	{
	public:
		explicit BombEventObserver(Scene& scene, float tileWorldSize);
		~BombEventObserver() override;

		void Notify(GameObject& actor, Event event) override;

	private:
		static constexpr int kDefaultExplosionRange{ 1 };

		Scene* m_pScene{};
		float m_TileWorldSize{ 0.0f };
		std::vector<glm::vec2> m_ActiveBombPositions{};
		std::unordered_map<GameObject*, std::vector<GameObject*>> m_PlayerBombs{};

		bool IsBombAtPosition(const glm::vec2& pos) const;
		void DetonateBomb(GameObject& bomb);
	};
}