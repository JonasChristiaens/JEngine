#pragma once
#include "BaseComponent.h"
#include "EventQueue/IObserver.h"
#include <memory>
#include <string>
#include <vector>

namespace dae
{
	class GameObject;
	class Texture2D;

	class HudComponent final : public BaseComponent, public IObserver
	{
	public:
		HudComponent(GameObject* pOwner, float hudWidth, float hudHeight, const std::vector<GameObject*>& players);
		~HudComponent() override;

		void Update() override;
		void Render() const override;
		void Notify(GameObject& actor, Event event) override;

	private:
		static constexpr int kCharSize{ 8 };
		static constexpr int kMaxTime{ 200 };
		static constexpr int kTimerScale{ 1550 };

		float m_Width{};
		float m_Height{};
		std::vector<GameObject*> m_Players{};
		std::vector<int> m_Scores{};
		std::vector<int> m_Lives{};
		std::unique_ptr<Texture2D> m_pFontTexture{};

		float m_RemainingMs{ static_cast<float>(kMaxTime * kTimerScale) };

		void RenderText(const std::string& text, float x, float y, float scale) const;
		void RefreshStats();
	};
}
